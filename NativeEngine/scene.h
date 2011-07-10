#ifndef SCENE_H
#define SCENE_H

#include <string>

#include <OgreSceneManager.h>

#include "backgroundmap.h"

class SceneFactory : public Ogre::SceneManagerFactory
{
public:
    Ogre::SceneManager *createInstance(const std::string &instanceName);
    void destroyInstance(Ogre::SceneManager *instance);
protected:
    void initMetaData() const;
};

class Scene : public Ogre::SceneManager
{
public:
    Scene(const std::string &name);
    ~Scene();

    const std::string &getTypeName() const;

    void setZoomLevel(float zoomLevel);
    float getZoomLevel() const;

    void resizeWindow(int w, int h);

    Ogre::Camera *getMainCamera() const;

    const Ogre::Vector3 &getCameraOrigin() const;

    BackgroundMap *createBackgroundMap(const std::string &directory);
private:
    Ogre::Camera *mMainCamera;
    Ogre::Vector3 mCameraOrigin;
    float mZoomLevel;
};

#endif // SCENE_H
