
#
# Redirects build output to Build/debug/bin or Build/release/bin
#
CONFIG(debug, debug|release) {
    DESTDIR = $${PWD}/Build/debug/bin/
} else {
    DESTDIR = $${PWD}/Build/release/bin/
}

# Generate PDB files in release mode
win32 {
    QMAKE_CFLAGS_RELEASE += -Zi
    QMAKE_CXXFLAGS_RELEASE += -Zi
    QMAKE_LFLAGS_RELEASE += -debug -opt:ref -opt:icf
}
