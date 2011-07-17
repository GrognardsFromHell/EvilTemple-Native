#ifndef SCENE_H
#define SCENE_H

#include <string>

#include <OgreSceneManager.h>
#include <OgreSceneNode.h>

#include "backgroundmap.h"
#include "grounddisc.h"
#include "selectiondata.h"

struct PickResult {
    qint64 id;
    float distance;
};

typedef QList<PickResult> PickResultList;

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

    /*
      Please note that x and y are in [0,1] normalized screen coordinates.
      */
    PickResultList *pick(float x, float y);

    BackgroundMap *createBackgroundMap(const std::string &directory);

    GroundDisc *createGroundDisc(const std::string &material);
private:
    Ogre::Camera *mMainCamera;
    Ogre::Vector3 mCameraOrigin;
    float mZoomLevel;
};

#endif // SCENE_H
