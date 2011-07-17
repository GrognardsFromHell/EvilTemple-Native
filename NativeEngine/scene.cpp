
#include <OgreRay.h>
#include <OgreSceneNode.h>

#include "scene.h"
#include "intersectutils.h"

const std::string SceneTypeName("Scene");

Ogre::SceneManager *SceneFactory::createInstance(const std::string &instanceName)
{
    return new Scene(instanceName);
}

void SceneFactory::destroyInstance(Ogre::SceneManager *instance)
{
    delete instance;
}

void SceneFactory::initMetaData(void) const
{
    mMetaData.typeName = SceneTypeName;
    mMetaData.description = "EvilTemple specific scene manager.";
    mMetaData.sceneTypeMask = Ogre::ST_GENERIC;
    mMetaData.worldGeometrySupported = false;
}

Scene::Scene(const std::string &name) : SceneManager(name), mZoomLevel(1)
{    
    mMainCamera = createCamera("MainCamera");

    // Position it at 500 in Z direction
    mMainCamera->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
    mMainCamera->setNearClipDistance(1);
    mMainCamera->setFarClipDistance(5000);

    // Old: -44
    Ogre::Quaternion rot1;
    rot1.FromAngleAxis(Ogre::Degree(-44.42700648682643f), Ogre::Vector3::NEGATIVE_UNIT_X);
    Ogre::Matrix4 rotate1matrix(rot1);

    // Old: 90-135
    Ogre::Quaternion rot2;
    rot2.FromAngleAxis(Ogre::Degree(135.0000005619373f), Ogre::Vector3::NEGATIVE_UNIT_Y);
    Ogre::Matrix4 rotate2matrix(rot2);

    mMainCamera->setOrientation(rot2.Inverse() * rot1.Inverse());
    mMainCamera->setPosition((rotate1matrix * rotate2matrix).inverse() * Ogre::Vector3(0, 0, 3000));

    mCameraOrigin = mMainCamera->getPosition();
}

Scene::~Scene()
{
}

const std::string &Scene::getTypeName() const
{
    return SceneTypeName;
}

BackgroundMap *Scene::createBackgroundMap(const std::string &directory)
{
    BackgroundMap *map = new BackgroundMap(mMainCamera, this, mCameraOrigin, QString::fromStdString(directory));
    return map;
}

Ogre::Camera *Scene::getMainCamera() const
{
    return mMainCamera;
}

void Scene::setZoomLevel(float zoomLevel)
{
    mZoomLevel = zoomLevel;
    /* Reset Ortho Window of main cam */
}

float Scene::getZoomLevel() const
{
    return mZoomLevel;
}

void Scene::resizeWindow(int w, int h)
{
    mMainCamera->setOrthoWindow(w / mZoomLevel, h / mZoomLevel);
}

const Ogre::Vector3 &Scene::getCameraOrigin() const
{
    return mCameraOrigin;
}

PickResultList *Scene::pick(float x, float y)
{
    Ogre::Ray ray;
    mMainCamera->getCameraToViewportRay(x, y, &ray);

    Ogre::RaySceneQuery *query = createRayQuery(ray);
    Ogre::RaySceneQueryResult &results = query->execute();

    PickResultList *resultList = new PickResultList;

    qDebug("Results: %d", results.size());
    foreach (const Ogre::RaySceneQueryResultEntry &result, results) {
        const Ogre::Any &userAny = result.movable->getUserAny();

        if (userAny.getType() != typeid(SelectionData))
            continue;

        SelectionData selection = Ogre::any_cast<SelectionData>(userAny);

        /* Do a cylinder intersection */
        IntersectResult cylinderResult = IntersectionUtils::intersect(ray,
                                                                      result.movable->getParentSceneNode()->getPosition(),
                                                                      selection.radius,
                                                                      selection.height);
        qDebug("Cylinder: %d %f", cylinderResult.intersects, cylinderResult.distance);

        if (cylinderResult.intersects) {
            PickResult pickResult;
            pickResult.distance = cylinderResult.distance;
            pickResult.id = selection.id;
            resultList->append(pickResult);
        }
    }

    destroyQuery(query);

    return resultList;
}

GroundDisc *Scene::createGroundDisc(const std::string &material)
{
    Ogre::NameValuePairList params;
    params["material"] = material;

    return static_cast<GroundDisc*>(createMovableObject(GroundDiscFactory::FACTORY_TYPE_NAME, &params));
}
