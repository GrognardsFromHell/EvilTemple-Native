
#include <QtCore/QEvent>
#include <QtCore/QDebug>
#include <QtCore/QObject>
#include <QtCore/QTextStream>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QScopedArrayPointer>
#include <QtGui/QApplication>
#include <QtDeclarative/QDeclarativeView>
#include <QtDeclarative/QDeclarativeError>
#include <QtDeclarative/QDeclarativeEngine>

#define QT_NO_EGL
#include <QtOpenGL/private/qgl_p.h>

#include <Ogre.h>

#include "nativeengine.h"
#include "grounddisc.h"
#include "ogrenetworkaccessmanager.h"
#include "scene.h"

static void saveGlState()
{
    /* TODO: Shader, FBO, VBO */
    /* Store as much GL state as possible */
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushClientAttrib(GL_ALL_ATTRIB_BITS);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
}

static void restoreGlState()
{
    /* TODO: Shader, FBO, VBO */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glPopAttrib();
    glPopClientAttrib();
}

class NativeEngineGLWidget : public QGLWidget
{
public:
    NativeEngineGLWidget(NativeEnginePrivate *_d, const QGLFormat &format) : QGLWidget(format), d(_d)
    {
    }

protected:
    void resizeGL(int w, int h);

private:
    NativeEnginePrivate* d;
};

class NativeEnginePrivate : public QObject, public Ogre::LogListener
{
Q_OBJECT
public:
    QGLWidget *glWidget;
    QGLWidgetPrivate *glWidgetPrivate;
    QDeclarativeView *uiView;
    Ogre::Root *ogreRoot;
    Ogre::RenderWindow *ogreWindow;
    Scene *sceneManager;
    Ogre::Camera *camera;
    Ogre::Viewport *viewport;

    KeyEventFn keyPressCallback;
    KeyEventFn keyReleaseCallback;
    MouseEventFn mouseDoubleClickCallback;
    MouseEventFn mousePressCallback;
    MouseEventFn mouseReleaseCallback;
    MouseEventFn mouseMoveCallback;
    WheelEventFn wheelCallback;
    LogCallback logCallback;

    NativeEnginePrivate(const NativeEngineSettings *settings);
    ~NativeEnginePrivate();

    void initQApplication(const NativeEngineSettings *settings);
    void initOgre();

    bool eventFilter(QObject *, QEvent *);
    bool handleGlWidgetEvent(QEvent*);
    void handleGlWidgetResize(int w, int h);

    void messageLogged(const std::string& message, Ogre::LogMessageLevel lml, bool maskDebug, const std::string &logName);

public slots:
    void viewStatusChanged();
};

NativeEngine::NativeEngine(const NativeEngineSettings *settings) : d(new NativeEnginePrivate(settings))
{
}

NativeEngine::~NativeEngine()
{
    delete d;
}

void NativeEngine::processEvents()
{
    QApplication::processEvents(QEventLoop::AllEvents|QEventLoop::EventLoopExec);
}

void NativeEngine::renderFrame()
{
    if (!d->ogreRoot->renderOneFrame())
        return;

    d->glWidgetPrivate->autoSwap = false;
    d->glWidgetPrivate->disable_clear_on_painter_begin = true;

    saveGlState();

    glColor4f(1, 1, 1, 1);

    QGLContext *ctx = d->glWidgetPrivate->glcx;
    glUseProgram(0);

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClear(GL_STENCIL_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(0x8458); // GL_COLOR_SUM
    glEnable(GL_DITHER);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
    glLightModeli(0x81F8, 0x81F9);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glBindFramebuffer(0x8CA9, 0);
    glBindFramebuffer(0x8CA8, 0);
    glBindFramebuffer(0x8D40, 0);
    glBindRenderbuffer(0x8D41, 0);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_EDGE_FLAG_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glScissor(0, 0, d->glWidget->width(), d->glWidget->height());
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);

    int w = d->glWidget->width();
    int h = d->glWidget->height();

    d->uiView->resize(w, h);
    glViewport(0, -h, w, 0);
    QPainter p(d->glWidget);
    p.beginNativePainting();
    p.endNativePainting();

    d->uiView->scene()->render(&p, QRectF(0, 0, w, h), QRectF(0, 0, w, h), Qt::IgnoreAspectRatio);
    p.end();

    restoreGlState();

    d->glWidget->swapBuffers();
}

QObject *NativeEngine::interfaceRoot()
{
    return d->uiView->rootObject();
}

void NativeEngine::setKeyPressCallback(KeyEventFn fn)
{
    d->keyPressCallback = fn;
}

void NativeEngine::setKeyReleaseCallback(KeyEventFn fn)
{
    d->keyReleaseCallback = fn;
}

void NativeEngine::setMouseDoubleClickCallback(MouseEventFn fn)
{
    d->mouseDoubleClickCallback = fn;
}

void NativeEngine::setMousePressCallback(MouseEventFn fn)
{
    d->mousePressCallback = fn;
}

void NativeEngine::setMouseReleaseCallback(MouseEventFn fn)
{
    d->mouseReleaseCallback = fn;
}

void NativeEngine::setMouseMoveCallback(MouseEventFn fn)
{
    d->mouseMoveCallback = fn;
}

void NativeEngine::setWheelCallback(WheelEventFn fn)
{
    d->wheelCallback = fn;
}

Scene *NativeEngine::mainScene()
{
    return d->sceneManager;
}

int NativeEngine::windowWidth() const
{
    return d->glWidget->width();
}

int NativeEngine::windowHeight() const
{
    return d->glWidget->height();
}

NativeEnginePrivate::NativeEnginePrivate(const NativeEngineSettings *settings)
      : glWidget(0), uiView(0), ogreRoot(0), ogreWindow(0), camera(0), viewport(0),
        keyPressCallback(0),keyReleaseCallback(0), mouseDoubleClickCallback(0),
        mousePressCallback(0), mouseReleaseCallback(0), mouseMoveCallback(0), sceneManager(0),
        wheelCallback(0), logCallback(0)
{
    initQApplication(settings);

    logCallback = settings->logCallback;

    /* Initialize OpenGL context format */
    QGLFormat format = QGLFormat::defaultFormat();
    format.setSwapInterval(0);

    /* Initialize OpenGL Widget */
    glWidget = new NativeEngineGLWidget(this, format);
    glWidget->setUpdatesEnabled(false);
    glWidget->installEventFilter(this);
    glWidget->setMouseTracking(true);
    glWidget->show();

    glWidgetPrivate = static_cast<QGLWidgetPrivate*>(QGLWidgetPrivate::get(glWidget));

    /* We will attempt to never change this GL context */
    glWidget->makeCurrent();

    // Paint once to init the OGL extension functions
    QPainter p(glWidget);
    p.end();

    /* Initialize UI View */
    uiView = new QDeclarativeView;
    connect(uiView, SIGNAL(statusChanged(QDeclarativeView::Status)), SLOT(viewStatusChanged()));

    uiView->scene()->setBackgroundBrush(Qt::NoBrush);
    uiView->setMouseTracking(true); // It would otherwise ignore mouse events we forward
    uiView->setViewportUpdateMode(QDeclarativeView::FullViewportUpdate);
    uiView->setOptimizationFlag(QDeclarativeView::DontAdjustForAntialiasing);
    uiView->setResizeMode(QDeclarativeView::SizeRootObjectToView);

    //saveGlState();
    initOgre();
    //restoreGlState();


    /* Set the Ogre Network manager on the view */
    uiView->engine()->setNetworkAccessManagerFactory(new OgreNetworkAccessManagerFactory);

    uiView->setSource(QUrl("qrc:/ui/InterfaceRoot.qml"));
}

NativeEnginePrivate::~NativeEnginePrivate()
{
    delete uiView;
    delete glWidget;

    delete qApp; // This is the QApplication we created in initQApplication
}

void NativeEnginePrivate::initQApplication(const NativeEngineSettings *settings)
{
    // Make a copy of argc and argv since Qt will attempt to modify them
    QVector<char*> argsPtrs;

    for (int i = 0; i < settings->argc; ++i) {
        size_t len = strlen(settings->argv[i]) + 1;
        char *argStr = new char[len];
        memcpy(argStr, settings->argv[i], len);
        argsPtrs.append(argStr);
    }

    QVector<char*> mutableArgsPtrs(argsPtrs);

    int argc = settings->argc;
    new QApplication(argc, mutableArgsPtrs.data());

    qDeleteAll(argsPtrs);
}

void NativeEnginePrivate::initOgre()
{
#if !defined(NDEBUG)
    Ogre::String pluginsCfg = "plugins_d.cfg";
#else
    Ogre::String pluginsCfg = "plugins.cfg";
#endif

    Ogre::LogManager * lm = new Ogre::LogManager();
    Ogre::Log *log = lm->createLog("", true, false, false);
    log->addListener(this);

    /* Initialize Ogre */
    ogreRoot = new Ogre::Root(pluginsCfg);

    /* Set OpenGL render system on Ogre */
    Ogre::RenderSystem *renderSystem = ogreRoot->getRenderSystemByName("OpenGL Rendering Subsystem");
    if (!renderSystem)
        throw std::exception("Unable to find OpenGL Render System!");
    renderSystem->setConfigOption("Colour Depth", "32");
    renderSystem->setConfigOption("Fixed Pipeline Enabled", "Yes");
    renderSystem->setConfigOption("VSync", "No");
    ogreRoot->setRenderSystem(renderSystem);

    ogreRoot->initialise(false);

    /* Initialize the render window to use the GLWidget internally */
    Ogre::NameValuePairList params;
    params["externalGLControl"] = "true";
    params["externalGLContext"] = QString::number((uint)wglGetCurrentContext()).toStdString(); // TODO: Platform dependent
    params["externalWindowHandle"] = QString::number((uint)glWidget->winId()).toStdString();
    ogreWindow = ogreRoot->createRenderWindow("", glWidget->width(), glWidget->height(), false, &params);

    /* We don't use mip maps since our camera distance from all objects is fixed */
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(0);

    /* here we could load "builtin" resources, but all other resources should be controlled by c# */
    /*Ogre::ResourceGroupManager &resourceManager = Ogre::ResourceGroupManager::getSingleton();
    resourceManager.addResourceLocation("C:/Users/Sebastian/AppData/Local/EvilTemple/data.zip", "Zip", "General", true);
    resourceManager.initialiseAllResourceGroups();*/

    /* Create the actual scene manager */
    ogreRoot->addSceneManagerFactory(new SceneFactory);
    ogreRoot->addMovableObjectFactory(new GroundDiscFactory);
    sceneManager = static_cast<Scene*>(ogreRoot->createSceneManager("Scene"));
    sceneManager->resizeWindow(glWidget->width(), glWidget->height());

    /* Create and initialize the main camera */
    camera = sceneManager->getMainCamera();

    /* Create the viewport */
    viewport = ogreWindow->addViewport(camera);
    viewport->setBackgroundColour(Ogre::ColourValue(0.5, 0.5, 0.5));

    sceneManager->setAmbientLight(Ogre::ColourValue(0, 0, 0));

    /* TEST */
    /*Ogre::Entity* model = sceneManager->createEntity("meshes/pcs/pc_human_male/pc_human_male.mesh");

    Ogre::SceneNode* headNode = sceneManager->getRootSceneNode()->createChildSceneNode();
    headNode->setPosition(0, 0, 0);
    headNode->attachObject(model);

    Ogre::AnimationState *animState = model->getAnimationState("unarmed_unarmed_idle");
    animState->setEnabled(true);
    animState->setLoop(true);
    animState->setWeight(1.0f);

    sceneManager->setAmbientLight(Ogre::ColourValue(0, 0, 0));

    // Create a light
    Ogre::Light* l = sceneManager->createLight("MainLight");
    l->setType(Ogre::Light::LT_DIRECTIONAL);
    l->setDirection(-0.6324093645670703858428703903848,
                    -0.77463436252716949786709498111783,
                    0);*/

    /* /TEST */
}

bool NativeEnginePrivate::eventFilter(QObject *receiver, QEvent *event)
{
    if (receiver == glWidget)
        return handleGlWidgetEvent(event);

    qWarning() << "Filtered event for unknown QObject: " << receiver->objectName();
    return false;
}

static MouseEvent createMouseEvent(QMouseEvent *e)
{
    MouseEvent result;

    result.button = e->button();
    result.buttons = e->buttons();
    result.x = e->x();
    result.y = e->y();

    return result;
}


static WheelEvent createWheelEvent(QWheelEvent *e)
{
    WheelEvent result;

    result.delta = e->delta();
    result.orientation = e->orientation();
    result.buttons = e->buttons();
    result.x = e->x();
    result.y = e->y();

    return result;
}


bool NativeEnginePrivate::handleGlWidgetEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::KeyPress:
        {
            QKeyEvent *ke = static_cast<QKeyEvent*>(event);
            if (keyPressCallback) {
                QScopedArrayPointer<wchar_t> text(new wchar_t[ke->text().length()]);
                ke->text().toWCharArray(text.data());
                keyPressCallback(ke->count(), ke->isAutoRepeat(), ke->key(), ke->modifiers(), text.data());
            }
        }
        return true;

    case QEvent::KeyRelease:
    {
        QKeyEvent *ke = static_cast<QKeyEvent*>(event);
        if (keyReleaseCallback) {
            QScopedArrayPointer<wchar_t> text(new wchar_t[ke->text().length()]);
            ke->text().toWCharArray(text.data());
            keyReleaseCallback(ke->count(), ke->isAutoRepeat(), ke->key(), ke->modifiers(), text.data());
        }
    }
        return true;

    case QEvent::MouseButtonDblClick:
        if (mouseDoubleClickCallback)
            mouseDoubleClickCallback(createMouseEvent(static_cast<QMouseEvent*>(event)));
        return true;

    case QEvent::MouseButtonPress:
        if (mousePressCallback)
            mousePressCallback(createMouseEvent(static_cast<QMouseEvent*>(event)));
        return true;

    case QEvent::MouseButtonRelease:
        if (mouseReleaseCallback)
            mouseReleaseCallback(createMouseEvent(static_cast<QMouseEvent*>(event)));
        return true;

    case QEvent::MouseMove:
        if (mouseMoveCallback)
            mouseMoveCallback(createMouseEvent(static_cast<QMouseEvent*>(event)));
        return true;

    case QEvent::Wheel:
        if (wheelCallback)
            wheelCallback(createWheelEvent(static_cast<QWheelEvent*>(event)));
        return true;

    case QEvent::Paint:
        // Ignore all paint events, we handle this ourselves
        return true;

    default:
        // By default pass unknown events to the widget
        return false;
    }
}

void NativeEngineGLWidget::resizeGL(int w, int h)
{
    d->handleGlWidgetResize(w, h);
}

void NativeEnginePrivate::handleGlWidgetResize(int w, int h)
{
    if (ogreWindow)
        ogreWindow->windowMovedOrResized();

    if (sceneManager)
        sceneManager->resizeWindow(w, h);

    if (uiView)
        uiView->resize(QSize(w, h));
}

void NativeEnginePrivate::viewStatusChanged()
{
    if (uiView->status() == QDeclarativeView::Error) {

        QString message;

        QTextStream out(&message);
        out << "Unable to load root UI element." << endl;
        out << "ERRORs:" << endl;
        foreach (const QDeclarativeError &error, uiView->errors()) {
            out << error.toString() << endl;
        }

        glWidget->close();

        throw std::exception(message.toLatin1().constData());
    }
}

void NativeEnginePrivate::messageLogged(const std::string& message, Ogre::LogMessageLevel lml, bool maskDebug, const std::string &logName)
{
    if (!logCallback)
        return;

    logCallback(logName.c_str(), message.c_str(), lml, maskDebug);
}

#include "nativeengine.moc"
