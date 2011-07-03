
#
# Redirects build output to Build/debug/bin or Build/release/bin
#
CONFIG(debug, debug|release) {
    DESTDIR = $${PWD}/Build/debug/bin/
} else {
    DESTDIR = $${PWD}/Build/release/bin/
}
