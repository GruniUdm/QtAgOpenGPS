#ifndef HEADLANDDESIGNER_H
#define HEADLANDDESIGNER_H

#include <QObject>
#include <QPoint>
#include <QProperty>
#include <QBindable>
#include <QtQml/qqmlregistration.h>

/**
 * HeadlandDesigner - Qt 6.8 Rectangle Pattern class for headland design properties
 *
 * Phase 6.0.20 - Migration to Q_OBJECT_BINDABLE_PROPERTY for automatic change tracking.
 * Provides modern Qt 6.8 property bindings for headland design interface.
 */
class HeadlandDesigner : public QObject
{
    Q_OBJECT
    // QML registration handled manually in main.cpp

    // ===== QML PROPERTIES - Qt 6.8 Rectangle Pattern =====
    Q_PROPERTY(double zoom READ zoom WRITE setZoom NOTIFY zoomChanged BINDABLE bindableZoom)
    Q_PROPERTY(double sX READ sX WRITE setSX NOTIFY sXChanged BINDABLE bindableSX)
    Q_PROPERTY(double sY READ sY WRITE setSY NOTIFY sYChanged BINDABLE bindableSY)
    Q_PROPERTY(double lineDistance READ lineDistance WRITE setLineDistance NOTIFY lineDistanceChanged BINDABLE bindableLineDistance)
    Q_PROPERTY(bool curveLine READ curveLine WRITE setCurveLine NOTIFY curveLineChanged BINDABLE bindableCurveLine)

public:
    explicit HeadlandDesigner(QObject *parent = nullptr);

    // ===== Qt 6.8 Rectangle Pattern READ/WRITE/BINDABLE Methods =====
    double zoom() const;
    void setZoom(double value);
    QBindable<double> bindableZoom();

    double sX() const;
    void setSX(double value);
    QBindable<double> bindableSX();

    double sY() const;
    void setSY(double value);
    QBindable<double> bindableSY();

    double lineDistance() const;
    void setLineDistance(double value);
    QBindable<double> bindableLineDistance();

    bool curveLine() const;
    void setCurveLine(bool value);
    QBindable<bool> bindableCurveLine();

signals:
    // Qt 6.8 Rectangle Pattern NOTIFY signals
    void zoomChanged();
    void sXChanged();
    void sYChanged();
    void lineDistanceChanged();
    void curveLineChanged();

    // Optional grouped notifications
    void viewChanged();
    void designChanged();

private:
    // ===== Qt 6.8 Q_OBJECT_BINDABLE_PROPERTY Private Members =====
    Q_OBJECT_BINDABLE_PROPERTY(HeadlandDesigner, double, m_zoom, &HeadlandDesigner::zoomChanged)
    Q_OBJECT_BINDABLE_PROPERTY(HeadlandDesigner, double, m_sX, &HeadlandDesigner::sXChanged)
    Q_OBJECT_BINDABLE_PROPERTY(HeadlandDesigner, double, m_sY, &HeadlandDesigner::sYChanged)
    Q_OBJECT_BINDABLE_PROPERTY(HeadlandDesigner, double, m_lineDistance, &HeadlandDesigner::lineDistanceChanged)
    Q_OBJECT_BINDABLE_PROPERTY(HeadlandDesigner, bool, m_curveLine, &HeadlandDesigner::curveLineChanged)
};

#endif // HEADLANDDESIGNER_H