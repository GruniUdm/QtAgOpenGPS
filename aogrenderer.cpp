#include "aogrenderer.h"
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QOpenGLFramebufferObject>
#include <QtCore/QRunnable>

#include <functional>

void AOGRenderer::render()
{
    //update();
    if(callback_object && initCallback) {
        if (!isInitialized) {
            //call the main form initialize gl function
            //initCallback( callback_object );
            initCallback();
            isInitialized = true;
        }
        //paintCallback( callback_object );
        paintCallback();
    }
    //win->resetOpenGLState();
}

AOGRenderer::AOGRenderer():
    isInitialized(false),win(0),calledInit(false), samples(0)
{
    callback_object = NULL;
    initCallback = NULL;
    paintCallback = NULL;
    cleanupCallback = NULL;

    qDebug() << "AOGRenderer constructor here.";
}

AOGRenderer::~AOGRenderer()
{
    //call gl cleanup method in main form.
}

void AOGRenderer::synchronize(QQuickFramebufferObject *fbo)
{
    //get window
    win = fbo->window();

    QVariant prop = fbo->property("callbackObject");
    if (prop.isValid()) {
        callback_object = (void *)prop.value<void *>();
    }

    prop = fbo->property("initCallback");
    if (prop.isValid()) {
        initCallback = prop.value<std::function<void (void)>>();
    }

    prop = fbo->property("paintCallback");
    if (prop.isValid()) {
        paintCallback = prop.value<std::function<void (void)>>();
    }

    prop = fbo->property("cleanupCallback");
    if (prop.isValid()) {
        cleanupCallback = prop.value<std::function<void (void)>>();
    }

    prop = fbo->property("samples");
    if (prop.isValid()) {
        samples = prop.toInt();
    }
}

AOGRendererInSG::AOGRendererInSG(QQuickItem* parent) : QQuickFramebufferObject(parent)
{
    theRenderer = NULL;
    // Initialize Qt 6.8 Q_OBJECT_BINDABLE_PROPERTY members
    m_shiftX = 0.0;
    m_shiftY = 0.0;
    qDebug() << "🏗️ AOGRendererInSG constructor called, parent:" << parent;
}

QOpenGLFramebufferObject *AOGRenderer::createFramebufferObject(const QSize &size)
{
    QOpenGLFramebufferObjectFormat format;
    format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
    if(samples) {
        format.setSamples(samples);
    }
    return new QOpenGLFramebufferObject(size,format);
}

AOGRenderer *AOGRendererInSG::createRenderer() const
{
    return new AOGRenderer();
}

// ===== Qt 6.8 Rectangle Pattern Implementation =====
double AOGRendererInSG::shiftX() const {
    return m_shiftX;
}

void AOGRendererInSG::setShiftX(double value) {
    // Preserve qFuzzyCompare validation for floating point precision
    if (qFuzzyCompare(m_shiftX, value)) return;
    m_shiftX = value;
}

QBindable<double> AOGRendererInSG::bindableShiftX() {
    return QBindable<double>(&m_shiftX);
}

double AOGRendererInSG::shiftY() const {
    return m_shiftY;
}

void AOGRendererInSG::setShiftY(double value) {
    // Preserve qFuzzyCompare validation for floating point precision
    if (qFuzzyCompare(m_shiftY, value)) return;
    m_shiftY = value;
}

QBindable<double> AOGRendererInSG::bindableShiftY() {
    return QBindable<double>(&m_shiftY);
}

/**************************************************/
/* NEW QQuickItem-based renderer                  */
/**************************************************/
AOGRendererItem::AOGRendererItem() : m_renderer(nullptr) {
    connect(this, &QQuickItem::windowChanged,
            this, &AOGRendererItem::handleWindowChanged);
}

void AOGRendererItem::handleWindowChanged(QQuickWindow *win) {
    if (win) {
        connect(win, &QQuickWindow::beforeSynchronizing,
                this, &AOGRendererItem::sync);
        connect(win, &QQuickWindow::sceneGraphInvalidated,
                this, &AOGRendererItem::cleanup);
        //win->setColor(Qt::black); //start with black window
        win->setPersistentGraphics(true);
    }
}

void AOGRendererItem::cleanup() {
    delete m_renderer;
    m_renderer = nullptr;
}

/* not sure about this vs the cleanup() above */
//private class to schedule removing renderernew instance
class RendererCleanupJob: public QRunnable {
public:
    RendererCleanupJob(AOGRendererNew *renderer) : m_renderer(renderer) { }
    void run() override { qDebug() << "deleting renderer"; delete m_renderer; }
private:
    AOGRendererNew *m_renderer;
};

void AOGRendererItem::releaseResources() {
    window()->scheduleRenderJob(new RendererCleanupJob(m_renderer),
                                QQuickWindow::BeforeSynchronizingStage);
    m_renderer = nullptr;
}

void AOGRendererItem::sync() {
    if (!m_renderer) {
        qDebug() << "Creating renderer.";
        //create new renderer
        m_renderer = new AOGRendererNew();
        connect(window(), &QQuickWindow::beforeRendering,
                m_renderer, &AOGRendererNew::init, Qt::DirectConnection);
        connect(window(), &QQuickWindow::beforeRenderPassRecording,
                m_renderer, &AOGRendererNew::paint, Qt::DirectConnection);
    }

    m_renderer->setViewportSize(window()->size() * window()->devicePixelRatio());
    m_renderer->setWindow(window());
    m_renderer->setItem(this);
}

void AOGRendererItem::setInitCallback(std::function<void ()> callback) {
    initCallback = callback;
    emit initCallbackChanged();
}

void AOGRendererItem::setPaintCallback(std::function<void ()> callback) {
    paintCallback = callback;
    emit paintCallbackChanged();
}

void AOGRendererItem::setCleanupCallback(std::function<void ()> callback) {
    cleanupCallback = callback;
    emit cleanupCallbackChanged();
}

void AOGRendererItem::setCallbackObject(void *object) {
    callback_object = object;
    emit callbackObjectChanged();
}

void AOGRendererItem::setSamples(int samples) {
    this->samples = samples;
    emit samplesChanged();
}

/**************
 * Renderer
 **************/
AOGRendererNew::~AOGRendererNew() {
    //if we have a valid cleanup callback, run it
    if (item && item->cleanupCallback)
        item->cleanupCallback();
}

void AOGRendererNew::init() {
    if (!initDone && item && item->initCallback) {
        QSGRendererInterface *rif = m_window->rendererInterface();
        Q_ASSERT(rif->graphicsApi() == QSGRendererInterface::OpenGL);
        item->initCallback();
        initDone = true;
    }
}

void AOGRendererNew::paint() {
    m_window->beginExternalCommands();

    if (!initDone) init(); //call it before we paint

    if (item && item->paintCallback)
        item->paintCallback();

    m_window->endExternalCommands();
}




