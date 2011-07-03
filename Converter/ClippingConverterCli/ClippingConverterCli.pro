#-------------------------------------------------
#
# Project created by QtCreator 2011-06-24T02:57:00
#
#-------------------------------------------------

QT       += core

TARGET = ClippingConverterCli
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += \
    ClippingConverterCli.cpp

include(../../Common.pri)
include(../Converters/Converters.pri)
include(../LegacyFormats/LegacyFormats.pri)
