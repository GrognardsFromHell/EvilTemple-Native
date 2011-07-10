#ifndef BACKGROUNDMAP_H
#define BACKGROUNDMAP_H

#include <Ogre.h>
#include <QtCore/QHash>
#include <QtCore/QPoint>

class BackgroundMapPrivate;

class BackgroundMap
{
public:
    BackgroundMap(Ogre::Camera *camera,
                  Ogre::SceneManager *sceneManager,
                  const Ogre::Vector3 &cameraOrigin,
                  const QString &directory);
    ~BackgroundMap();

    void update();

private:
    BackgroundMapPrivate *d;
};

#endif // BACKGROUNDMAP_H
