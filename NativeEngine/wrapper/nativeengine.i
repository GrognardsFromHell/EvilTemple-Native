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
%}

%rename("Scene") SceneManager;
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
private:
    SceneManager();
    ~SceneManager();
};

}

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

    Ogre::SceneManager *mainScene();

    QObject *interfaceRoot();
};

%include "../resourcemanager.h"


