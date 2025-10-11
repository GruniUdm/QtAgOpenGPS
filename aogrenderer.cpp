#include "aogrenderer.h"
#include <QOpenGLContext>
#include <QQuickWindow>
#include <QOpenGLFramebufferObject>

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

