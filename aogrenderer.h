// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// OpenGL renderer. Displayed in QML
#ifndef OPENGLCONTROL_H
#define OPENGLCONTROL_H

#include <QQuickWindow>
//#include <QOpenGLContext>
#include <QQuickFramebufferObject>
#include <QtQml/qqmlregistration.h>
#include <QProperty>
#include <QBindable>

#include <functional>
Q_DECLARE_METATYPE(std::function<void (void)>)

class AOGRenderer : public QQuickFramebufferObject::Renderer
{

protected:
    virtual void synchronize(QQuickFramebufferObject *);

    bool isInitialized;

public:
    AOGRenderer();
    ~AOGRenderer();

    void render();

    QOpenGLFramebufferObject *createFramebufferObject(const QSize &size);

    /*
    void registerPaintCallback(std::function<void ()> callback);
    void registerInitCallback(std::function<void ()> callback);
    void registerCleanupCallback(std::function<void ()> callback);
    */

private:
    QQuickWindow *win;
    bool calledInit;
    //FormGPS *mf;
    int samples;

    //callback in main form to do actual rendering
    void *callback_object;
    std::function<void (void)> paintCallback;
    std::function<void (void)> initCallback;
    std::function<void (void)> cleanupCallback;
};

class AOGRendererInSG : public QQuickFramebufferObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(AOGRenderer)

    // ===== QML PROPERTIES - Qt 6.8 Rectangle Pattern =====
    Q_PROPERTY(double shiftX READ shiftX WRITE setShiftX NOTIFY shiftXChanged BINDABLE bindableShiftX)
    Q_PROPERTY(double shiftY READ shiftY WRITE setShiftY NOTIFY shiftYChanged BINDABLE bindableShiftY)

public:
    AOGRenderer *theRenderer;

    AOGRendererInSG(QQuickItem* parent = nullptr);

    AOGRenderer *createRenderer() const;

    // ===== Qt 6.8 Rectangle Pattern READ/WRITE/BINDABLE Methods =====
    double shiftX() const;
    void setShiftX(double value);
    QBindable<double> bindableShiftX();

    double shiftY() const;
    void setShiftY(double value);
    QBindable<double> bindableShiftY();

signals:
    // Qt 6.8 Rectangle Pattern NOTIFY signals
    void shiftXChanged();
    void shiftYChanged();

    // NOTE: clicked, dragged, zoomOut, zoomIn signals are defined in QML
    // and work via Qt's meta-object system - no C++ declaration needed

private:
    // ===== Qt 6.8 Q_OBJECT_BINDABLE_PROPERTY Private Members =====
    Q_OBJECT_BINDABLE_PROPERTY(AOGRendererInSG, double, m_shiftX, &AOGRendererInSG::shiftXChanged)
    Q_OBJECT_BINDABLE_PROPERTY(AOGRendererInSG, double, m_shiftY, &AOGRendererInSG::shiftYChanged)
};

#endif // OPENGLCONTROL_H
