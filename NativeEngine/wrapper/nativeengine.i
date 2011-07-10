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

#include "../scene.h"
#include "../backgroundmap.h"
using Ogre::ColourValue;
using Ogre::Radian;

%}

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

class SceneNode {
public:
    %rename("AttachObject") attachObject;
    void attachObject(MovableObject *obj);

    %rename("CreateChildSceneNode") createChildSceneNode;
    SceneNode *createChildSceneNode();
    SceneNode *createChildSceneNode(std::string name);

    void setPosition(float x, float y, float z);
    void setOrientation(float w, float x, float y, float z);
    void setScale(float x, float y, float z);

private:
    SceneNode();
    ~SceneNode();
};

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

class Vector3 {
public:
    float x;
    float y;
    float z;

    Vector3(float x, float y, float z);
};

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


class Scene : public Ogre::SceneManager {
public:

    %rename("CreateBackgroundMap") createBackgroundMap;
    BackgroundMap *createBackgroundMap(const std::string &directory);

    %rename("GetMainCamera") getMainCamera;
    Ogre::Camera *getMainCamera() const;

    %rename("GetCameraOrigin") getCameraOrigin;
    const Ogre::Vector3 &getCameraOrigin() const;

private:
    Scene();
    ~Scene();
};

struct NativeEngineSettings {
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
};

%include "../resourcemanager.h"


