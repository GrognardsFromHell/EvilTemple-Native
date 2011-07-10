
#include <QtCore/QRectF>
#include <QtCore/QSet>
#include <QtCore/QDebug>

#include "backgroundmap.h"
#include "utils.h"

static const QPoint MapOrigin(-8428, -4366);

class CameraListener : public Ogre::Camera::Listener
{
public:
    BackgroundMap *q;

    void cameraPreRenderScene(Ogre::Camera* cam) {
        q->update();
    }

};

class MapTileRenderable : public Ogre::Rectangle2D {
public:
    MapTileRenderable();

    bool preRender(Ogre::SceneManager *sm, Ogre::RenderSystem *rsys);

};

class BackgroundMapPrivate {
public:
    Ogre::Camera *camera;
    Ogre::SceneManager *sceneManager;
    QHash<QPoint, MapTileRenderable*> activeTiles;
    Ogre::Vector3 cameraOrigin;
    Ogre::SceneNode *backgroundMapNode;
    CameraListener listener;

    QString directory;
    QSet<QPoint> existingTiles;

    QRectF lastSeenView;

    QRectF visibleRectangle();
    void loadTiles();
};

inline uint qHash(const QPoint &key)
{
    return qHash((key.x() << 16) & 0xFFFF0000 | (key.y() & 0xFFFF));
}

BackgroundMap::BackgroundMap(Ogre::Camera *camera,
                             Ogre::SceneManager *sceneManager,
                             const Ogre::Vector3 &cameraOrigin,
                             const QString &directory) : d(new BackgroundMapPrivate)
{
    d->camera = camera;
    d->sceneManager = sceneManager;
    d->cameraOrigin = cameraOrigin;
    d->directory = directory;

    d->loadTiles();
    qDebug() << "Loaded" << d->existingTiles.size() << "background tiles for directory" << d->directory;

    d->lastSeenView = d->visibleRectangle();

    d->backgroundMapNode = d->sceneManager->getRootSceneNode()->createChildSceneNode("BackgroundMap");

    d->listener.q = this;
    camera->addListener(&d->listener);
}

BackgroundMap::~BackgroundMap()
{
    delete d;
}

void BackgroundMap::update()
{
    using namespace Ogre;

    QRectF visibleRect = d->visibleRectangle();

    int minX = qAbs(MapOrigin.x() - visibleRect.left()) / 256;
    int minY = qAbs(MapOrigin.y() - visibleRect.bottom()) / 256;
    int maxX = (qAbs(MapOrigin.x() - visibleRect.right()) + 255) / 256;
    int maxY = (qAbs(MapOrigin.y() - visibleRect.top()) + 255) / 256;

    QPointF viewportCenter = visibleRect.center();

    /* Deactivate tiles that are no longer visible */
    QList<MapTileRenderable*> inactiveTiles;

    QHash<QPoint,MapTileRenderable*>::iterator it = d->activeTiles.begin();
    while (it != d->activeTiles.end()) {
        const QPoint &p = it.key();
        if (p.x() < minX || p.x() > maxX || p.y() < minY || p.y() > maxY) {
            inactiveTiles.append(it.value());
            it = d->activeTiles.erase(it);
        } else {
            it++;
        }
    }

    for (int i = minX; i <= maxX; ++i) {
        for (int j = minY; j <= maxY; ++j) {

            QPoint tile(i, j);

            if (d->activeTiles.contains(tile)) {
                d->activeTiles[tile]->setCorners(MapOrigin.x() + i * 256 - viewportCenter.x(),
                                                 MapOrigin.y() + j * -256 - viewportCenter.y(),
                                                 MapOrigin.x() + (i+1) * 256 - viewportCenter.x(),
                                                 MapOrigin.y() + (j+1) * -256 - viewportCenter.y(),
                                                 false);

                continue; // Already active
            }

            if (!d->existingTiles.contains(tile))
                continue; // The tile doesn't exist.

            QString filename = QString("%1/%3-%2.jpg").arg(d->directory).arg(i).arg(j);

            MapTileRenderable *rect;
            if (!inactiveTiles.isEmpty()) {
                rect = inactiveTiles.takeLast();
            } else {
                rect = new MapTileRenderable();

                Ogre::AxisAlignedBox box;
                box.setInfinite();;
                rect->setBoundingBox(box);

                std::string tileMaterialName = ("MapTile" + QString::number(reinterpret_cast<intptr_t>(rect))).toStdString();

                /* Lazily clone the base map tile material */
                if (!MaterialManager::getSingleton().resourceExists(tileMaterialName)) {
                    MaterialPtr baseMaterial = MaterialManager::getSingleton().load("backgroundTileMaterial", "General");
                    baseMaterial->clone(tileMaterialName);
                }

                rect->setMaterial(tileMaterialName);
                d->backgroundMapNode->attachObject(rect);
            }

            d->activeTiles[tile] = rect;

            if (rect->getMaterial().isNull())
                throw new std::exception("Couldn't set material on map tile.");

            if (!rect->getMaterial()->getTechnique(0))
                throw new std::exception("Material on map tile has no techniques.");

            if (!rect->getMaterial()->getTechnique(0)->getPass(0))
                throw new std::exception("Material on map tile has no passes.");

            if (!rect->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0))
                throw new std::exception("Material on map tile has no texture unit state.");

            rect->getMaterial()->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureName(filename.toStdString());
            
            rect->setCorners(MapOrigin.x() + i * 256 - viewportCenter.x(),
                             MapOrigin.y() + j * -256 - viewportCenter.y(),
                             MapOrigin.x() + (i+1) * 256 - viewportCenter.x(),
                             MapOrigin.y() + (j+1) * -256 - viewportCenter.y(),
                             false);
        }
    }

    /* Delete any remaining inactive tiles */
    foreach (MapTileRenderable *renderable, inactiveTiles) {
        d->backgroundMapNode->detachObject(renderable);
        delete renderable;
    }

}

void BackgroundMapPrivate::loadTiles()
{
    QByteArray indexData = loadFile(directory + "/index.dat");
    QDataStream stream(indexData);
    stream.setByteOrder(QDataStream::LittleEndian);

    int count;
    stream >> count;

    for (int i = 0; i < count; ++i) {
        short x, y;
        stream >> x >> y;
        existingTiles.insert(QPoint(x,y));
    }
}

QRectF BackgroundMapPrivate::visibleRectangle()
{
    using namespace Ogre;

    Vector3 oldPosition = camera->getPosition();
    camera->setPosition(cameraOrigin);

    // Translate the center of the screen space (0,0,0) using the inverted view matrix
    Matrix4 viewMatrix = camera->getViewMatrix();
    Matrix4 projMatrix = camera->getProjectionMatrix();

    camera->setPosition(oldPosition);

    Vector4 pos(oldPosition - cameraOrigin);
    Vector4 translatedCenter = viewMatrix * pos;

    float transX = translatedCenter.x;
    float transY = translatedCenter.y;

    float w = (1 / projMatrix[0][0]);
    float h = (1 / projMatrix[1][1]);

    return QRectF(transX - w, transY - h, 2 * w, 2 * h);
}

MapTileRenderable::MapTileRenderable() : Rectangle2D(true)
{
    mUseIdentityProjection = false;
    setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);

    using namespace Ogre;
    static const float bias = 0.5f / 256.0f;
    setUVs( Vector2(bias, bias), Vector2(bias, 1 - bias), Vector2(1 - bias, bias), Vector2(1 - bias, 1 - bias));
}

bool MapTileRenderable::preRender(Ogre::SceneManager *sm, Ogre::RenderSystem *rsys)
{
    return Rectangle2D::preRender(sm, rsys);
}
