
#
# Included by projects that need to link against the Ogre SDK.
#
OGRE_SDK_PATH = C:/code/ogre/

INCLUDEPATH += $$OGRE_SDK_PATH/OgreMain/include/ $$OGRE_SDK_PATH/build/include/
CONFIG(debug, debug|release) {
    LIBS += -L$$OGRE_SDK_PATH/build/lib/debug/ -lOgreMain_d
} else {
    LIBS += -L$$OGRE_SDK_PATH/build/lib/release/ -lOgreMain
}
