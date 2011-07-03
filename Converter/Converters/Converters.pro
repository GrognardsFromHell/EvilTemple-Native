
TEMPLATE = lib

INCLUDEPATH += include

include(../../Common.pri)
include(../../ThirdParty/Ogre.pri)
include(../LegacyFormats/LegacyFormats.pri)

DEFINES += CONVERTERS_LIBRARY

DEF_FILE = src/converters.def

SOURCES += \
    src/modelconverter.cpp \
    src/modelconverter_c.cpp \
    src/clippingconverter.cpp \
    src/ogresystems.cpp

HEADERS += \
    include/Converters/global.h \
    include/Converters/modelconverter.h \
    include/Converters/clippingconverter.h \
    include/Converters/ogresystems.h \
    include/Converters/Output/conversionoutput.h

OTHER_FILES += \
    Converters.pri \
    src/converters.def
