#-------------------------------------------------
#
# Project created by QtCreator 2011-07-01T19:38:08
#
#-------------------------------------------------

TARGET = NativeEngine
TEMPLATE = lib

QT += gui opengl declarative network

DEFINES += NATIVEENGINE_LIBRARY

SOURCES += nativeengine.cpp \
    ogrenetworkaccessmanager.cpp \
    qobject_wrap.cpp \
    connectionmanager.cpp

# ADD SWIG WRAPPERS
SOURCES += wrapper/nativeengine_wrap.cxx

HEADERS += nativeengine.h \
    global.h \
    ogre.h \
    ogrenetworkaccessmanager.h \
    connectionmanager.h

include(../Common.pri)
include(../ThirdParty/Ogre.pri)

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    InterfaceRoot.qml \
    wrapper/nativeengine.i
