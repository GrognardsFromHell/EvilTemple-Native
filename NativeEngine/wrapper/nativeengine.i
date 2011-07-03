%module NativeEngineInterop

%{
#include "../nativeengine.h"
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

%include arrays_csharp.i

CSHARP_ARRAYS(char*, string)

%apply char* OUTPUT[] {char **}

// Make classes partial
%typemap(csclassmodifiers) ExtendMe "public partial class"

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

%include "../nativeengine.h"

