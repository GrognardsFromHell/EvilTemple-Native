#ifndef NATIVEENGINE_H
#define NATIVEENGINE_H

class NativeEnginePrivate;
class QObject;

struct NativeEngineSettings {
    int argc;
    const char** argv;
};

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

private:
    NativeEnginePrivate *d;
};

#endif // NATIVEENGINE_H
