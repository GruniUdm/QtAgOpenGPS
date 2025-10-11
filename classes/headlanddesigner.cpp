#include "headlanddesigner.h"

HeadlandDesigner::HeadlandDesigner(QObject *parent)
    : QObject(parent)
{
    // Initialize Qt 6.8 Q_OBJECT_BINDABLE_PROPERTY members
    m_zoom = 1.0;
    m_sX = 0.0;
    m_sY = 0.0;
    m_lineDistance = 0.0;
    m_curveLine = false;
}

// ===== Qt 6.8 Rectangle Pattern Implementation =====
double HeadlandDesigner::zoom() const {
    return m_zoom;
}

void HeadlandDesigner::setZoom(double value) {
    m_zoom = value;
}

QBindable<double> HeadlandDesigner::bindableZoom() {
    return QBindable<double>(&m_zoom);
}

double HeadlandDesigner::sX() const {
    return m_sX;
}

void HeadlandDesigner::setSX(double value) {
    m_sX = value;
}

QBindable<double> HeadlandDesigner::bindableSX() {
    return QBindable<double>(&m_sX);
}

double HeadlandDesigner::sY() const {
    return m_sY;
}

void HeadlandDesigner::setSY(double value) {
    m_sY = value;
}

QBindable<double> HeadlandDesigner::bindableSY() {
    return QBindable<double>(&m_sY);
}

double HeadlandDesigner::lineDistance() const {
    return m_lineDistance;
}

void HeadlandDesigner::setLineDistance(double value) {
    m_lineDistance = value;
}

QBindable<double> HeadlandDesigner::bindableLineDistance() {
    return QBindable<double>(&m_lineDistance);
}

bool HeadlandDesigner::curveLine() const {
    return m_curveLine;
}

void HeadlandDesigner::setCurveLine(bool value) {
    m_curveLine = value;
}

QBindable<bool> HeadlandDesigner::bindableCurveLine() {
    return QBindable<bool>(&m_curveLine);
}