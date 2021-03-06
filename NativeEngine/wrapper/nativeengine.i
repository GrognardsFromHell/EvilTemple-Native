%module NativeEngineInterop

%{
#include "../nativeengine.h"
#include "../resourcemanager.h"
%}

// Language independent exception handler
%include exception.i

%exception {
    try {
        $action
    } catch(std::exception &e) {
        SWIG_exception(SWIG_UnknownError, e.what());
    }
}

// Make classes partial
%typemap(csclassmodifiers) NativeEngine "public partial class"
%typemap(csclassmodifiers) ResourceManager "public partial class"

// Typemaps for passing QObject by pointer
%typemap(in)                           QObject* "/* in* */ $1 = $input;"
%typemap(out, null="/* null* */ NULL") QObject* "/* out* */ $result = $1;"
%typemap(argout)                       QObject* "/* argout* */"
%typemap(cstype, out="/* cstype* out */ IntPtr") QObject* "/* cstype* */ IntPtr"
%typemap(imtype, out="/* imtype* out */ IntPtr") QObject* "/* imtype* */ IntPtr"
%typemap(ctype,  out="/* ctype* out */ QObject*") QObject* "/* ctype* */ QObject*"
%typemap(csin)                         QObject* "/* csin* */ $csinput"
%typemap(csvarin, excode=SWIGEXCODE2)  QObject*
%{
        set { $imcall;$excode }
%}
%typemap(csout, excode=SWIGEXCODE)     QObject*
  {
        /* csout* */ IntPtr ret = $imcall;$excode
        return ret;
  }
%typemap(csvarout, excode=SWIGEXCODE)  QObject*
%{
  get {
        /* csvarout* */ IntPtr ret = $imcall;$excode
        return ret;
  }
%}

// Typemaps for passing void* by pointer
%typemap(in)                           void* "/* in* */ $1 = static_cast<$1_type>($input);"
%typemap(out, null="/* null* */ NULL") void* "/* out* */ $result = $1;"
%typemap(argout)                       void* "/* argout* */"
%typemap(cstype, out="/* cstype* out */ IntPtr") void* "/* cstype* */ IntPtr"
%typemap(imtype, out="/* imtype* out */ IntPtr") void* "/* imtype* */ IntPtr"
%typemap(ctype,  out="/* ctype* out */ void*") void* "/* ctype* */ void*"
%typemap(csin)                         void* "/* csin* */ $csinput"
%typemap(csvarin, excode=SWIGEXCODE2)  void*
%{
        set { $imcall;$excode }
%}
%typemap(csout, excode=SWIGEXCODE)     void*
  {
        /* csout* */ IntPtr ret = $imcall;$excode
        return ret;
  }
%typemap(csvarout, excode=SWIGEXCODE)  void*
%{
  get {
        /* csvarout* */ IntPtr ret = $imcall;$excode
        return ret;
  }
%}


%typemap(cscode) NativeEngine %{

    private UserInterface _userInterface;

    public UserInterface UserInterface {
        get {
            if (_userInterface == null)
                _userInterface = new UserInterface(interfaceRoot());
            return _userInterface;
        }
    }
%}

typedef long long int qint64;

/* Typemapping for QByteArray::data() */
%typemap(cstype, excode=SWIGEXCODE2) const char* constData() "byte[]"
%typemap(imtype, excode=SWIGEXCODE2) const char* constData() "IntPtr"

%typemap(csout, excode=SWIGEXCODE2) const char* constData() %{
    {
        byte[] ret = new byte[this.size()];
        IntPtr data = $imcall;
        System.Runtime.InteropServices.Marshal.Copy(data, ret, 0, this.size());
        $excode
        return ret;
    }
%}

class QByteArray {
public:
    ~QByteArray();

    int size();
    const char *constData();
private:
    QByteArray();
};


%include "std_string.i"

%{
#include <Ogre.h>
#include <typeinfo>

#include "../scene.h"
#include "../backgroundmap.h"
using Ogre::ColourValue;
using Ogre::Radian;

%}

/* Convert between float and Radians by using the radians value */
%typemap(in) Radian "$1 = Ogre::Radian($input);"
%typemap(out) Radian "$result = $1.valueRadians();"
%typemap(ctype) Radian "float"
%typemap(cstype) Radian "float"
%typemap(imtype) Radian "float"
%typemap(csin) Radian "$csinput"
%typemap(csout, excode=SWIGEXCODE) Radian
{
    float ret = $imcall;$excode
    return ret;
}

namespace Ogre {


class MovableObject {
private:
    MovableObject();
    ~MovableObject();
};

class Entity : public MovableObject {
private:
    Entity();
    ~Entity();
};

class Node {
public:
    void setInitialState();

private:
    Node();
    ~Node();
};

class Vector3 {
public:
    %rename("X") x;
    float x;
    %rename("Y") y;
    float y;
    %rename("Z") z;
    float z;

    Vector3(float x, float y, float z);
};

class Quaternion {
public:
    %rename("X") x;
    float x;
    %rename("Y") y;
    float y;
    %rename("Z") z;
    float z;
    %rename("W") w;
    float w;

    Quaternion();
    Quaternion(float w, float x, float y, float z);

    void FromAngleAxis(Radian rfAngle, const Vector3 &rkAxis);
};

%{
#include "../selectiondata.h"
%}


class SceneNode : public Node {
public:
    void attachObject(MovableObject *obj);

    unsigned short numAttachedObjects (void) const;
    MovableObject *getAttachedObject (unsigned short index);

    SceneNode *createChildSceneNode();
    SceneNode *createChildSceneNode(std::string name);

    SceneNode *getParentSceneNode (void) const;

    void removeChild(Node *child);
    void addChild(Node *child);

    void setVisible(bool visible, bool cascade=true);

    const std::string &getName (void) const;

    void setPosition(float x, float y, float z);
    const Ogre::Vector3 &getPosition() const;

    void setOrientation(float w, float x, float y, float z);
    const Ogre::Quaternion &getOrientation() const;

    void setScale(float x, float y, float z);
    const Ogre::Vector3 &getScale() const;

    unsigned short numChildren (void) const;
    Node *getChild (unsigned short index) const;

private:
    SceneNode();
    ~SceneNode();
};

%include "selectiondata.i"

class ColourValue {
public:
    float r;
    float g;
    float b;
    float a;
private:
    ColourValue();
    ~ColourValue();
};

class Light : public MovableObject {
public:

    enum LightTypes {
        LT_POINT,
        LT_DIRECTIONAL,
        LT_SPOTLIGHT
    };

    void setType(LightTypes type);
    LightTypes getType();

    void setDiffuseColour(float red, float green, float blue);
    const ColourValue &getDiffuseColour() const;

    void setSpecularColour(float red, float green, float blue);
    const ColourValue &getSpecularColour() const;

    void setAttenuation(float range, float constant, float linear, float quadratic);
    float getAttenuationRange (void) const;
    float getAttenuationConstant (void) const;
    float getAttenuationLinear (void) const;
    float getAttenuationQuadric (void) const;

    void setPosition(float x, float y, float z);
    const Vector3 &getPosition(void) const;

    void setDirection(float x, float y, float z);
    const Vector3 &getDirection() const;

    void setSpotlightRange(Radian innerAngle, Radian outerAngle, float falloff);
    Radian getSpotlightInnerAngle() const;
    Radian getSpotlightOuterAngle() const;
    float getSpotlightFalloff() const;

    void setPowerScale(float power);
    float getPowerScale() const;
private:
    Light();
    ~Light();
};

class TransformKeyFrame {
public:
    void setTranslate(const Vector3 &trans);
    const Vector3 &getTranslate() const;

    void setScale(const Vector3 &scale);
    const Vector3 &getScale() const;

    void setRotation(const Quaternion &quaternion);
    const Quaternion &getRotation() const;
private:
    TransformKeyFrame();
    ~TransformKeyFrame();
};

class NodeAnimationTrack {
public:
    TransformKeyFrame *createNodeKeyFrame(float timePos);
private:
    NodeAnimationTrack();
    ~NodeAnimationTrack();
};

class Animation {
public:
    NodeAnimationTrack *createNodeTrack(unsigned short handle, Node *node);
private:
    Animation();
    ~Animation();
};

class AnimationState {
public:
    void setEnabled(bool enabled);
    bool getEnabled();

    void setLoop(bool loop);
    bool getLoop() const;

    void addTime(float time);
private:
    AnimationState();
    ~AnimationState();
};

class SceneManager {
public:
    %rename("CreateEntity") createEntity;
    Entity *createEntity(std::string entityName, std::string meshFilename);
    Entity *createEntity(std::string meshFilename);

    %rename("CreateSceneNode") createSceneNode;
    SceneNode *createSceneNode();
    SceneNode *createSceneNode(std::string name);

    %rename("GetRootSceneNode") getRootSceneNode;
    SceneNode *getRootSceneNode();

    %rename("CreateLight") createLight;
    Light *createLight(std::string name);
    Light *createLight();

    %rename("CreateAnimation") createAnimation;
    Animation *createAnimation(const std::string &name, float length);
    %rename("GetAnimation") getAnimation;
    Animation *getAnimation(const std::string &name) const;
    %rename("HasAnimation") hasAnimation;
    bool hasAnimation(const std::string &name) const;
    %rename("DestroyAnimation") destroyAnimation;
    void destroyAnimation (const std::string &name);

    %rename("CreateAnimationState") createAnimationState;
    AnimationState *createAnimationState(const std::string &animName);
    void destroyAnimationState(const std::string &name);
    AnimationState *getAnimationState(const std::string &name);
private:
    SceneManager();
    ~SceneManager();
};

class Camera
{
public:

    %rename("SetPosition") setPosition;
    void setPosition(float x, float y, float z);

    %rename("GetPosition") getPosition;
    const Vector3 &getPosition() const;

    %rename("Move") move;
    void move(const Vector3 &vec);

private:
    Camera();
    ~Camera();
};

}

class BackgroundMap
{
public:
    /* TODO Color and stuff */
private:
    BackgroundMap();
    ~BackgroundMap();
};

class GroundDisc : public Ogre::MovableObject
{
public:
    void setMaterial(const std::string &material);
private:
    GroundDisc();
    ~GroundDisc();
};

template<typename T>
class QList {
public:
    const T &at(int i) const;
    int size() const;
};

struct PickResult {
    qint64 id;
    float distance;
};

typedef QList<PickResult> PickResultList;
%template(PickResultList) QList<PickResult>;

class Scene : public Ogre::SceneManager {
public:

    %rename("CreateBackgroundMap") createBackgroundMap;
    BackgroundMap *createBackgroundMap(const std::string &directory);

    %rename("CreateGroundDisc") createGroundDisc;
    GroundDisc *createGroundDisc(const std::string &material);

    %rename("GetMainCamera") getMainCamera;
    Ogre::Camera *getMainCamera() const;

    %rename("GetCameraOrigin") getCameraOrigin;
    const Ogre::Vector3 &getCameraOrigin() const;

    %newobject pick;
    PickResultList* pick(float x, float y);

private:
    Scene();
    ~Scene();
};

%{
typedef void (__stdcall *LogCallback)(const char *name, const char *message, int level, bool maskDebug);
%}
typedef void* LogCallback;

struct NativeEngineSettings {
    LogCallback logCallback;
    int argc;
    const char** argv;
};

class NativeEngine
{
public:
    NativeEngine(const NativeEngineSettings *settings);
    ~NativeEngine();

    void processEvents();

    void renderFrame();

    Scene *mainScene();

    QObject *interfaceRoot();

    int windowWidth();
    int windowHeight();
};

%include "../resourcemanager.h"


