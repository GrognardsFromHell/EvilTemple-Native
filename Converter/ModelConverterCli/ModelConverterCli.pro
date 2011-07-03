
TEMPLATE = app

CONFIG += console

SOURCES += \
    ModelConverterCli.cpp

include(../../Common.pri)
include(../Converters/Converters.pri)
include(../LegacyFormats/LegacyFormats.pri)
