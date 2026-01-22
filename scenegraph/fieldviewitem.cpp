// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Scene graph-based field view renderer implementation

#include "fieldviewitem.h"
#include "cameraproperties.h"
#include "gridproperties.h"
#include "fieldsurfaceproperties.h"

#include "fieldsurfacenode.h"
#include "gridnode.h"
#include "boundarynode.h"
#include "vehiclenode.h"
#include "aogmaterial.h"
#include "aoggeometry.h"

#include "glm.h"

#include "cvehicle.h"
#include "cboundary.h"

#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGTextureMaterial>
#include <QSGOpaqueTextureMaterial>
#include <QSGTexture>
#include <QQuickWindow>
#include <QImage>
#include <QSize>
#include <QtMath>
#include <QDebug>
#include <QLoggingCategory>

// Logging category for this file - must end in ".qtagopengps" to not be suppressed
Q_LOGGING_CATEGORY(fieldviewitem_log, "fieldviewitem.qtagopengps")

// ============================================================================
// FieldViewNode Implementation
// ============================================================================

FieldViewNode::FieldViewNode()
{
    // Create child nodes (typed for refactored ones, generic for others)
    fieldSurfaceNode = new FieldSurfaceNode();
    gridNode = new GridNode();
    boundaryNode = new BoundaryNode();
    coverageNode = new QSGNode();   // Not yet refactored
    guidanceNode = new QSGNode();   // Not yet refactored
    vehicleNode = new VehicleNode();
    uiNode = new QSGNode();

    // Add children in render order (back to front)
    appendChildNode(fieldSurfaceNode);  // Field surface first (furthest back)
    appendChildNode(gridNode);          // Grid lines
    appendChildNode(boundaryNode);
    appendChildNode(coverageNode);
    appendChildNode(guidanceNode);
    appendChildNode(vehicleNode);
    appendChildNode(uiNode);
}

FieldViewNode::~FieldViewNode()
{
    // Child nodes are automatically deleted by QSGNode destructor
}

// ============================================================================
// FieldViewItem Implementation
// ============================================================================

FieldViewItem::FieldViewItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    setFlag(ItemHasContents, true);
    setClip(true);  // Clip rendering to item bounds

    // Create camera settings object (owned by this)
    m_camera = new CameraProperties(this);

    m_grid = new GridProperties(this);

    m_fieldSurface = new FieldSurfaceProperties(this);

    // Connect camera property changes to update()
    connect(m_camera, &CameraProperties::zoomChanged, this, &QQuickItem::update);
    connect(m_camera, &CameraProperties::xChanged, this, &QQuickItem::update);
    connect(m_camera, &CameraProperties::yChanged, this, &QQuickItem::update);
    connect(m_camera, &CameraProperties::rotationChanged, this, &QQuickItem::update);
    connect(m_camera, &CameraProperties::pitchChanged, this, &QQuickItem::update);
    connect(m_camera, &CameraProperties::fovChanged, this, &QQuickItem::update);

    //Connect grid property changes to update()
    connect(m_grid, &GridProperties::sizeChanged, this, &QQuickItem::update);
    connect(m_grid, &GridProperties::colorChanged, this, &QQuickItem::update);
    connect(m_grid, &GridProperties::visibleChanged, this, &QQuickItem::update);

    //Connect field surface property changes to update()
    connect(m_fieldSurface, &FieldSurfaceProperties::visibleChanged, this, &QQuickItem::update);
    connect(m_fieldSurface, &FieldSurfaceProperties::colorChanged, this, &QQuickItem::update);
    connect(m_fieldSurface, &FieldSurfaceProperties::showTextureChanged, this, &QQuickItem::update);

    // Connect other property changes to update()
    connect(this, &FieldViewItem::boundaryColorChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::guidanceColorChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::backgroundColorChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::vehicleColorChanged, this, &QQuickItem::update);
}

FieldViewItem::~FieldViewItem()
{
}

// ============================================================================
// Camera Property Accessor
// ============================================================================

CameraProperties* FieldViewItem::camera() const { return m_camera; }

GridProperties* FieldViewItem::grid() const { return m_grid; }

FieldSurfaceProperties* FieldViewItem::fieldSurface() const { return m_fieldSurface; }

// ============================================================================
// Visibility Property Accessors
// ============================================================================

bool FieldViewItem::showBoundary() const { return m_showBoundary; }
void FieldViewItem::setShowBoundary(bool value) { m_showBoundary = value; }
QBindable<bool> FieldViewItem::bindableShowBoundary() { return &m_showBoundary; }

bool FieldViewItem::showCoverage() const { return m_showCoverage; }
void FieldViewItem::setShowCoverage(bool value) { m_showCoverage = value; }
QBindable<bool> FieldViewItem::bindableShowCoverage() { return &m_showCoverage; }

bool FieldViewItem::showGuidance() const { return m_showGuidance; }
void FieldViewItem::setShowGuidance(bool value) { m_showGuidance = value; }
QBindable<bool> FieldViewItem::bindableShowGuidance() { return &m_showGuidance; }

bool FieldViewItem::showVehicle() const { return m_showVehicle; }
void FieldViewItem::setShowVehicle(bool value) { m_showVehicle = value; }
QBindable<bool> FieldViewItem::bindableShowVehicle() { return &m_showVehicle; }

// ============================================================================
// Color Property Accessors
// ============================================================================

QColor FieldViewItem::boundaryColor() const { return m_boundaryColor; }
void FieldViewItem::setBoundaryColor(const QColor &color) { m_boundaryColor = color; }
QBindable<QColor> FieldViewItem::bindableBoundaryColor() { return &m_boundaryColor; }

QColor FieldViewItem::guidanceColor() const { return m_guidanceColor; }
void FieldViewItem::setGuidanceColor(const QColor &color) { m_guidanceColor = color; }
QBindable<QColor> FieldViewItem::bindableGuidanceColor() { return &m_guidanceColor; }

QColor FieldViewItem::backgroundColor() const { return m_backgroundColor; }
void FieldViewItem::setBackgroundColor(const QColor &color) { m_backgroundColor = color; }
QBindable<QColor> FieldViewItem::bindableBackgroundColor() { return &m_backgroundColor; }

QColor FieldViewItem::vehicleColor() const { return m_vehicleColor; }
void FieldViewItem::setVehicleColor(const QColor &color) { m_vehicleColor = color; }
QBindable<QColor> FieldViewItem::bindableVehicleColor() { return &m_vehicleColor; }

// ============================================================================
// Public Methods
// ============================================================================

void FieldViewItem::markBoundaryDirty()
{
    m_boundaryDirty = true;
    update();
}

void FieldViewItem::markCoverageDirty()
{
    m_coverageDirty = true;
    update();
}

void FieldViewItem::markGuidanceDirty()
{
    m_guidanceDirty = true;
    update();
}

void FieldViewItem::markAllDirty()
{
    m_boundaryDirty = true;
    m_coverageDirty = true;
    m_guidanceDirty = true;
    m_gridDirty = true;
    update();
}

// ============================================================================
// Synchronize Data from Singletons
// ============================================================================

void FieldViewItem::syncFromSingletons()
{
    // Access singletons - this runs on the GUI thread before updatePaintNode
    // Copy data needed for rendering to avoid thread-safety issues

    // Get vehicle data
    if (auto *vehicle = CVehicle::instance()) {
        m_renderData.vehicleX = vehicle->pivotAxlePos.easting;
        m_renderData.vehicleY = vehicle->pivotAxlePos.northing;
        m_renderData.vehicleHeading = vehicle->fixHeading();
        // Note: steerAngle would need to come from another source
    }

    // Get boundary data if dirty
    if (m_boundaryDirty) {
        m_renderData.boundaries.clear();
        // Note: CBoundary needs to be accessed via FormGPS currently
        // This will be refactored when CBoundary becomes a singleton
        // For now, boundaries are updated via explicit calls
    }

    // Coverage and guidance would be synced similarly
}

// ============================================================================
// Core Scene Graph Method
// ============================================================================

QSGNode *FieldViewItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    //qCDebug(fieldviewitem_log) << "FieldViewItem::updatePaintNode called, oldNode:" << oldNode;

    // Sync data from singletons (GUI thread, safe to access)
    syncFromSingletons();

    // Get or create root node
    FieldViewNode *rootNode = static_cast<FieldViewNode *>(oldNode);
    if (!rootNode) {
        rootNode = new FieldViewNode();
        //qCDebug(fieldviewitem_log) << "FieldViewItem: Created new FieldViewNode";
    }

    // Build the MVP matrix once for all children
    m_currentMv = buildViewMatrix();
    m_currentP = buildProjectionMatrix();
    m_currentNcd = buildNdcMatrix();

    // Note: We don't set the matrix on the transform node because QSGTransformNode
    // is for 2D scene graph transforms, not 3D MVP matrices. Instead, we pass
    // the MVP matrix to each material's shader.

    // Load floor texture if needed (requires window())
    loadFloorTexture();

    // Get WorldGrid data for field surface and grid
    //adjust zoom based on cam distance
    double _gridZoom = m_camera->zoom();
    int count;

    if (_gridZoom> 100) count = 4;
    else if (_gridZoom> 80) count = 8;
    else if (_gridZoom> 50) count = 16;
    else if (_gridZoom> 20) count = 32;
    else if (_gridZoom> 10) count = 64;
    else count = 80;

    double n = glm::roundMidAwayFromZero(camera()->y() / (m_grid->size() / count * 2)) * (m_grid->size() / count * 2);
    double e = glm::roundMidAwayFromZero(camera()->x() / (m_grid->size() / count * 2)) * (m_grid->size() / count * 2);
    double eastingMin = e - m_grid->size();
    double eastingMax = e + m_grid->size();
    double northingMin = n - m_grid->size();
    double northingMax = n + m_grid->size();

    QMatrix4x4 currentMvp = m_currentNcd * m_currentP * m_currentMv;

    if (m_fieldSurface->visible()) {
        // Always update field surface (it changes with camera position)
        rootNode->fieldSurfaceNode->update(
            currentMvp,
            m_fieldSurface->color(),
            m_fieldSurface->showTexture(),
            m_floorTexture,
            eastingMin, eastingMax,
            northingMin, northingMax,
            count
        );
    }

    // Update grid if visible
    if (m_grid->visible()) {
        // Calculate grid spacing based on zoom
        double gridSpacing = 10.0;
        double camDistance = m_camera->zoom();
        if (camDistance <= 20000 && camDistance > 10000) gridSpacing = 2012;
        else if (camDistance <= 10000 && camDistance > 5000) gridSpacing = 1006;
        else if (camDistance <= 5000 && camDistance > 2000) gridSpacing = 503;
        else if (camDistance <= 2000 && camDistance > 1000) gridSpacing = 201.2;
        else if (camDistance <= 1000 && camDistance > 500) gridSpacing = 100.6;
        else if (camDistance <= 500 && camDistance > 250) gridSpacing = 50.3;
        else if (camDistance <= 250 && camDistance > 150) gridSpacing = 25.15;
        else if (camDistance <= 150 && camDistance > 50) gridSpacing = 10.06;
        else if (camDistance <= 50 && camDistance > 1) gridSpacing = 5.03;

        QSize viewportSize(static_cast<int>(width()), static_cast<int>(height()));
        rootNode->gridNode->update(
            m_currentMv,
            m_currentP,
            m_currentNcd,
            viewportSize,
            m_grid->color(),
            static_cast<float>(m_grid->thickness()),
            eastingMin, eastingMax,
            northingMin, northingMax,
            gridSpacing
        );
        m_gridDirty = false;
    }

    // Update boundary if visible and dirty
    if (m_showBoundary && m_boundaryDirty) {
        rootNode->boundaryNode->update(
            currentMvp,
            m_boundaryColor,
            m_renderData.boundaries
        );
        m_boundaryDirty = false;
    }

    // Update coverage and guidance (not yet refactored)
    if (m_showCoverage && m_coverageDirty) {
        //updateCoverageNode(rootNode);
        m_coverageDirty = false;
    }

    if (m_showGuidance && m_guidanceDirty) {
        //updateGuidanceNode(rootNode);
        m_guidanceDirty = false;
    }

    // Vehicle always updates (position changes frequently)
    if (m_showVehicle) {
        rootNode->vehicleNode->update(
            m_currentMv,
            m_currentP,
            m_currentNcd,
            m_vehicleColor,
            m_renderData.vehicleX,
            m_renderData.vehicleY,
            m_renderData.vehicleHeading
        );
    }

    return rootNode;
}

void FieldViewItem::loadFloorTexture()
{
    if (m_fieldSurface->showTexture() && !m_floorTexture && window()) {
        // Load texture if not already loaded
#ifdef LOCAL_QML
        QString texPath = QStringLiteral("local:/images/textures/floor.png");
#else
        QString texPath = QStringLiteral(":/AOG/images/textures/floor.png");
#endif
        QImage floorImage(texPath);
        if (!floorImage.isNull()) {
            // Convert to RGBA format if needed for consistent texture handling
            if (floorImage.format() != QImage::Format_RGBA8888 &&
                floorImage.format() != QImage::Format_RGBA8888_Premultiplied) {
                floorImage = floorImage.convertToFormat(QImage::Format_RGBA8888);
            }
            // Don't use atlas - we need repeating wrap mode
            m_floorTexture = window()->createTextureFromImage(floorImage);
            if (m_floorTexture) {
                m_floorTexture->setHorizontalWrapMode(QSGTexture::Repeat);
                m_floorTexture->setVerticalWrapMode(QSGTexture::Repeat);
                m_floorTexture->setFiltering(QSGTexture::Linear);
                m_floorTexture->setMipmapFiltering(QSGTexture::None);
            }
        }
    }
}


// ============================================================================
// Matrix Building
// ============================================================================

QMatrix4x4 FieldViewItem::buildNdcMatrix() const
{
    // Standard 3D MVP: world coords -> NDC (-1 to 1)
    //QMatrix4x4 mvp3d = buildProjectionMatrix() * buildViewMatrix();

    // Transform from NDC to item-local coordinates
    // NDC: x=-1 to 1, y=-1 to 1
    // Item: x=0 to width, y=0 to height (Qt Y is down, so we flip)
    //
    // Mapping:
    //   x_item = (x_ndc + 1) / 2 * width  = x_ndc * (width/2) + (width/2)
    //   y_item = (1 - y_ndc) / 2 * height = y_ndc * (-height/2) + (height/2)
    //
    // As a matrix (applied after MVP):
    //   scale(width/2, -height/2, 1) then translate(width/2, height/2, 0)
    // But since we're pre-multiplying, we do it in reverse order

    float w = static_cast<float>(width());
    float h = static_cast<float>(height());

    QMatrix4x4 ndcToItem;

    if (w > 0 && h > 0) {
        // First translate NDC origin to item center, then scale
        ndcToItem.translate(w / 2.0f, h / 2.0f, 0.0f);
        ndcToItem.scale(w / 2.0f, -h / 2.0f, 1.0f);  // Negative Y to flip (Qt Y is down)
    }

    return ndcToItem;
}

QMatrix4x4 FieldViewItem::buildProjectionMatrix() const
{
    QMatrix4x4 projection;

    // Set up perspective projection
    float aspect = static_cast<float>(width()) / static_cast<float>(height());
    if (aspect <= 0)
        aspect = 1.0f;

    // FOV is set via camera.fov property (default 40.1 degrees matches OpenGL's fovy = 0.7 radians)
    // Far plane uses camDistanceFactor * camSetDistance where camDistanceFactor = -2
    // Since zoom is positive (abs of camSetDistance), far = 2 * zoom
    float fov = static_cast<float>(m_camera->fov());
    float farPlane = static_cast<float>(2.0 * m_camera->zoom());
    if (farPlane < 100.0f) farPlane = 100.0f;  // Minimum far plane

    projection.perspective(fov, aspect, 1.0f, farPlane);

    return projection;
}

QMatrix4x4 FieldViewItem::buildViewMatrix() const
{
    QMatrix4x4 view;

    // Match OpenGL version: camera distance = camSetDistance * 0.5
    // zoom is positive (abs of camSetDistance), so we use -zoom * 0.5
    view.translate(0, 0, static_cast<float>(-m_camera->zoom() * 0.5));

    // Apply pitch (tilt)
    view.rotate(static_cast<float>(m_camera->pitch()), 1.0f, 0.0f, 0.0f);

    // Apply rotation (yaw) - camera follows vehicle heading
    view.rotate(static_cast<float>(-m_camera->rotation()), 0.0f, 0.0f, 1.0f);

    // Translate to camera position (center on vehicle/target)
    view.translate(static_cast<float>(-m_camera->x()), static_cast<float>(-m_camera->y()), 0.0f);

    return view;
}
