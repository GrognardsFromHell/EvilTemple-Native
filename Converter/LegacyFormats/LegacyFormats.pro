
TEMPLATE = lib
TARGET = LegacyFormats

include(../../Common.pri)

QT += opengl

CONFIG += precompile_header

DEFINES += LEGACYFORMATS_LIBRARY

PRECOMPILED_HEADER = src/stable.h

RESOURCES += resources/resources.qrc

OTHER_FILES += resources/prototypes_flags.txt \
    LegacyFormats.pri

INCLUDEPATH += include

HEADERS += \
    include/LegacyFormats/zonetemplates.h \
    include/LegacyFormats/zonetemplatereader.h \
    include/LegacyFormats/zonetemplate.h \
    include/LegacyFormats/zonebackgroundmap.h \
    include/LegacyFormats/virtualfilesystem.h \
    include/LegacyFormats/util.h \
    include/LegacyFormats/troikaformatsglobal.h \
    include/LegacyFormats/troikaarchive.h \
    include/LegacyFormats/troika_skeleton.h \
    include/LegacyFormats/troika_model.h \
    include/LegacyFormats/troika_materials.h \
    include/LegacyFormats/troika_material.h \
    include/LegacyFormats/skmreader.h \
    include/LegacyFormats/prototypes.h \
    include/LegacyFormats/objectfilereader.h \
    include/LegacyFormats/modelsource.h \
    include/LegacyFormats/messagefile.h \
    include/LegacyFormats/dagreader.h \
    include/LegacyFormats/constants.h \
    src/stable.h

SOURCES += \
    src/zonetemplates.cpp \
    src/zonetemplatereader.cpp \
    src/zonetemplate.cpp \
    src/zonebackgroundmap.cpp \
    src/virtualfilesystem.cpp \
    src/troikaarchive.cpp \
    src/troika_skeleton.cpp \
    src/troika_model.cpp \
    src/troika_materials.cpp \
    src/troika_material.cpp \
    src/skmreader.cpp \
    src/prototypes.cpp \
    src/objectfilereader.cpp \
    src/messagefile.cpp \
    src/dagreader.cpp
