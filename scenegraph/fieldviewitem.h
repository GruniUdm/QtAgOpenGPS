// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Scene graph-based field view renderer using native Qt scene graph nodes

#ifndef FIELDVIEWITEM_H
#define FIELDVIEWITEM_H

#include <QQuickItem>
#include <QMatrix4x4>
#include <QSGNode>
#include <QSGGeometryNode>
#include <QSGTransformNode>
#include <QtQml/qqmlregistration.h>
#include <QProperty>
#include <QBindable>
#include <QColor>

class QSGTexture;

// Forward declarations for AOG classes
class CVehicle;
class CBoundary;
class CCamera;
class Backend;
class SettingsManager;

// ============================================================================
// FieldViewNode - Root node for the field view scene graph
// ============================================================================

class FieldViewNode : public QSGTransformNode
{
public:
    FieldViewNode();
    ~FieldViewNode();

    // Child nodes for different render layers (rendered back to front)
    QSGNode *fieldSurfaceNode = nullptr; // Field surface (solid color or textured)
    QSGNode *backgroundNode = nullptr;   // World grid lines
    QSGNode *boundaryNode = nullptr;     // Field boundaries
    QSGNode *coverageNode = nullptr;     // Coverage patches
    QSGNode *guidanceNode = nullptr;     // Guidance lines (AB line, contour, etc.)
    QSGNode *vehicleNode = nullptr;      // Vehicle representation
    QSGNode *uiNode = nullptr;           // UI overlays (markers, flags)
};

// ============================================================================
// FieldViewItem - Main QQuickItem for field rendering
// ============================================================================

class FieldViewItem : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

    // ===== Camera/View Properties - Exposed to QML =====
    Q_PROPERTY(double zoom READ zoom WRITE setZoom NOTIFY zoomChanged BINDABLE bindableZoom)
    Q_PROPERTY(double cameraX READ cameraX WRITE setCameraX NOTIFY cameraXChanged BINDABLE bindableCameraX)
    Q_PROPERTY(double cameraY READ cameraY WRITE setCameraY NOTIFY cameraYChanged BINDABLE bindableCameraY)
    Q_PROPERTY(double cameraRotation READ cameraRotation WRITE setCameraRotation NOTIFY cameraRotationChanged BINDABLE bindableCameraRotation)
    Q_PROPERTY(double cameraPitch READ cameraPitch WRITE setCameraPitch NOTIFY cameraPitchChanged BINDABLE bindableCameraPitch)
    Q_PROPERTY(double fovDegrees READ fovDegrees WRITE setFovDegrees NOTIFY fovDegreesChanged BINDABLE bindableFovDegrees)

    // ===== Rendering State Properties =====
    Q_PROPERTY(bool showBoundary READ showBoundary WRITE setShowBoundary NOTIFY showBoundaryChanged BINDABLE bindableShowBoundary)
    Q_PROPERTY(bool showCoverage READ showCoverage WRITE setShowCoverage NOTIFY showCoverageChanged BINDABLE bindableShowCoverage)
    Q_PROPERTY(bool showGuidance READ showGuidance WRITE setShowGuidance NOTIFY showGuidanceChanged BINDABLE bindableShowGuidance)
    Q_PROPERTY(bool showVehicle READ showVehicle WRITE setShowVehicle NOTIFY showVehicleChanged BINDABLE bindableShowVehicle)
    Q_PROPERTY(bool showGrid READ showGrid WRITE setShowGrid NOTIFY showGridChanged BINDABLE bindableShowGrid)
    Q_PROPERTY(bool isTextureOn READ isTextureOn WRITE setIsTextureOn NOTIFY isTextureOnChanged BINDABLE bindableIsTextureOn)

    // ===== Color Properties =====
    Q_PROPERTY(QColor boundaryColor READ boundaryColor WRITE setBoundaryColor NOTIFY boundaryColorChanged BINDABLE bindableBoundaryColor)
    Q_PROPERTY(QColor guidanceColor READ guidanceColor WRITE setGuidanceColor NOTIFY guidanceColorChanged BINDABLE bindableGuidanceColor)
    Q_PROPERTY(QColor gridColor READ gridColor WRITE setGridColor NOTIFY gridColorChanged BINDABLE bindableGridColor)
    Q_PROPERTY(QColor fieldColor READ fieldColor WRITE setFieldColor NOTIFY fieldColorChanged BINDABLE bindableFieldColor)
    Q_PROPERTY(QColor backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundColorChanged BINDABLE bindableBackgroundColor)
    Q_PROPERTY(QColor vehicleColor READ vehicleColor WRITE setVehicleColor NOTIFY vehicleColorChanged BINDABLE bindableVehicleColor)

public:
    explicit FieldViewItem(QQuickItem *parent = nullptr);
    ~FieldViewItem() override;

    // ===== Camera Property Accessors =====
    double zoom() const;
    void setZoom(double value);
    QBindable<double> bindableZoom();

    double cameraX() const;
    void setCameraX(double value);
    QBindable<double> bindableCameraX();

    double cameraY() const;
    void setCameraY(double value);
    QBindable<double> bindableCameraY();

    double cameraRotation() const;
    void setCameraRotation(double value);
    QBindable<double> bindableCameraRotation();

    double cameraPitch() const;
    void setCameraPitch(double value);
    QBindable<double> bindableCameraPitch();

    double fovDegrees() const;
    void setFovDegrees(double value);
    QBindable<double> bindableFovDegrees();

    // ===== Visibility Property Accessors =====
    bool showBoundary() const;
    void setShowBoundary(bool value);
    QBindable<bool> bindableShowBoundary();

    bool showCoverage() const;
    void setShowCoverage(bool value);
    QBindable<bool> bindableShowCoverage();

    bool showGuidance() const;
    void setShowGuidance(bool value);
    QBindable<bool> bindableShowGuidance();

    bool showVehicle() const;
    void setShowVehicle(bool value);
    QBindable<bool> bindableShowVehicle();

    bool showGrid() const;
    void setShowGrid(bool value);
    QBindable<bool> bindableShowGrid();

    bool isTextureOn() const;
    void setIsTextureOn(bool value);
    QBindable<bool> bindableIsTextureOn();

    // ===== Color Property Accessors =====
    QColor boundaryColor() const;
    void setBoundaryColor(const QColor &color);
    QBindable<QColor> bindableBoundaryColor();

    QColor guidanceColor() const;
    void setGuidanceColor(const QColor &color);
    QBindable<QColor> bindableGuidanceColor();

    QColor gridColor() const;
    void setGridColor(const QColor &color);
    QBindable<QColor> bindableGridColor();

    QColor fieldColor() const;
    void setFieldColor(const QColor &color);
    QBindable<QColor> bindableFieldColor();

    QColor backgroundColor() const;
    void setBackgroundColor(const QColor &color);
    QBindable<QColor> bindableBackgroundColor();

    QColor vehicleColor() const;
    void setVehicleColor(const QColor &color);
    QBindable<QColor> bindableVehicleColor();

    // ===== Public Methods =====
    Q_INVOKABLE void markBoundaryDirty();
    Q_INVOKABLE void markCoverageDirty();
    Q_INVOKABLE void markGuidanceDirty();
    Q_INVOKABLE void markAllDirty();

signals:
    // Camera signals
    void zoomChanged();
    void cameraXChanged();
    void cameraYChanged();
    void cameraRotationChanged();
    void cameraPitchChanged();
    void fovDegreesChanged();

    // Visibility signals
    void showBoundaryChanged();
    void showCoverageChanged();
    void showGuidanceChanged();
    void showVehicleChanged();
    void showGridChanged();
    void isTextureOnChanged();

    // Color signals
    void boundaryColorChanged();
    void guidanceColorChanged();
    void gridColorChanged();
    void fieldColorChanged();
    void backgroundColorChanged();
    void vehicleColorChanged();

protected:
    // ===== Core Scene Graph Method =====
    QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *) override;

private:
    // ===== Node Update Methods =====
    void updateFieldSurfaceNode(FieldViewNode *rootNode);
    void updateBoundaryNode(FieldViewNode *rootNode);
    void updateCoverageNode(FieldViewNode *rootNode);
    void updateGuidanceNode(FieldViewNode *rootNode);
    void updateVehicleNode(FieldViewNode *rootNode);
    void updateGridNode(FieldViewNode *rootNode);

    // ===== Matrix Building =====
    QMatrix4x4 buildMvpMatrix() const;
    QMatrix4x4 buildProjectionMatrix() const;
    QMatrix4x4 buildViewMatrix() const;

    // ===== Dirty Tracking =====
    bool m_fieldSurfaceDirty = true;
    bool m_boundaryDirty = true;
    bool m_coverageDirty = true;
    bool m_guidanceDirty = true;
    bool m_gridDirty = true;

    // ===== Current MVP Matrix (set each frame for materials) =====
    QMatrix4x4 m_currentMvp;

    // ===== Texture for field surface =====
    QSGTexture *m_floorTexture = nullptr;

    // ===== Singleton Access (cached for thread safety) =====
    // These are populated in updatePaintNode from the main thread
    struct RenderData {
        // Vehicle position
        double vehicleX = 0;
        double vehicleY = 0;
        double vehicleHeading = 0;
        double steerAngle = 0;

        // Boundary data
        QVector<QVector<QVector3D>> boundaries;

        // Coverage data
        QVector<QVector3D> coverageVertices;
        QVector<QColor> coverageColors;

        // Guidance data
        QVector<QVector3D> guidanceLine;
        bool hasGuidance = false;
    };
    RenderData m_renderData;

    void syncFromSingletons();

    // ===== Qt 6.8 Q_OBJECT_BINDABLE_PROPERTY Members =====
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, double, m_zoom, 15.0, &FieldViewItem::zoomChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, double, m_cameraX, 0.0, &FieldViewItem::cameraXChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, double, m_cameraY, 0.0, &FieldViewItem::cameraYChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, double, m_cameraRotation, 0.0, &FieldViewItem::cameraRotationChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, double, m_cameraPitch, -20.0, &FieldViewItem::cameraPitchChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, double, m_fovDegrees, 40.1, &FieldViewItem::fovDegreesChanged)

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, bool, m_showBoundary, true, &FieldViewItem::showBoundaryChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, bool, m_showCoverage, true, &FieldViewItem::showCoverageChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, bool, m_showGuidance, true, &FieldViewItem::showGuidanceChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, bool, m_showVehicle, true, &FieldViewItem::showVehicleChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, bool, m_showGrid, true, &FieldViewItem::showGridChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, bool, m_isTextureOn, false, &FieldViewItem::isTextureOnChanged)

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, QColor, m_boundaryColor, QColor(255, 255, 0), &FieldViewItem::boundaryColorChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, QColor, m_guidanceColor, QColor(0, 255, 0), &FieldViewItem::guidanceColorChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, QColor, m_gridColor, QColor(0, 0, 0), &FieldViewItem::gridColorChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, QColor, m_fieldColor, QColor(145, 145, 145), &FieldViewItem::fieldColorChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, QColor, m_backgroundColor, QColor(69, 102, 179), &FieldViewItem::backgroundColorChanged)  // Day sky blue
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(FieldViewItem, QColor, m_vehicleColor, QColor(255, 255, 255), &FieldViewItem::vehicleColorChanged)
};

#endif // FIELDVIEWITEM_H
