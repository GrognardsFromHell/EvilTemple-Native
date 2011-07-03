
INCLUDEPATH += $${PWD}/include
LIBS += -lLegacyFormats

CONFIG(debug, debug|release) {
    LIBS += -L$${PWD}/../../Build/debug/bin
} else {
    LIBS += -L$${PWD}/../../Build/release/bin
}
