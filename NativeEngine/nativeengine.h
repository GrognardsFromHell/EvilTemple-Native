#ifndef NATIVEENGINE_H
#define NATIVEENGINE_H

#include <QtGui/QKeyEvent>
class Scene;
class NativeEnginePrivate;
class QObject;

typedef void (__stdcall *LogCallback)(const char *name, const char *message, int level, bool maskDebug);

struct NativeEngineSettings {
    LogCallback logCallback;
    int argc;
    const char** argv;

    NativeEngineSettings() : logCallback(0), argc(0), argv(0) {}
};

#pragma pack(push)
#pragma pack(1)
struct MouseEvent {
    int button;
    int buttons;
    int x;
    int y;
};

struct WheelEvent {
    int x;
    int y;
    int buttons;
    int delta;
    int orientation;
};
#pragma pack(pop)

typedef void (__stdcall *KeyEventFn)(int count, bool isAutoRepeat, int key, int modifiers, const wchar_t *text);

typedef void (__stdcall *MouseEventFn)(const MouseEvent &e);

typedef void (__stdcall *WheelEventFn)(const WheelEvent &e);

class NativeEngine {
public:
    NativeEngine(const NativeEngineSettings *settings);
    ~NativeEngine();

    /**
      Processes all events that are queued in the application. Call this periodically,
      if possible before every frame is drawn.
      */
    void processEvents();

    /**
      Renders a single frame.
      */
    void renderFrame();

    /**
      Returns the root interface element.
      */
    QObject *interfaceRoot();

    Scene *mainScene();

    int windowWidth() const;
    int windowHeight() const;

    /**
      Sets a function to be called for key events.
      */
    void setKeyPressCallback(KeyEventFn fn);

    void setKeyReleaseCallback(KeyEventFn fn);

    void setMouseDoubleClickCallback(MouseEventFn fn);

    void setMousePressCallback(MouseEventFn fn);

    void setMouseReleaseCallback(MouseEventFn fn);

    void setMouseMoveCallback(MouseEventFn fn);

    void setWheelCallback(WheelEventFn fn);

private:
    NativeEnginePrivate *d;
};

#endif // NATIVEENGINE_H
