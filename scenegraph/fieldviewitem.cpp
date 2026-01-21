// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Scene graph-based field view renderer implementation

#include "fieldviewitem.h"
#include "aogmaterial.h"
#include "aoggeometry.h"

#include "cvehicle.h"
#include "cboundary.h"
#include "backend.h"
#include "settingsmanager.h"
#include "backend/worldgrid.h"

#include <QSGGeometryNode>
#include <QSGFlatColorMaterial>
#include <QSGTextureMaterial>
#include <QSGOpaqueTextureMaterial>
#include <QSGTexture>
#include <QQuickWindow>
#include <QImage>
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
    // Create child node containers
    fieldSurfaceNode = new QSGNode();
    backgroundNode = new QSGNode();
    boundaryNode = new QSGNode();
    coverageNode = new QSGNode();
    guidanceNode = new QSGNode();
    vehicleNode = new QSGNode();
    uiNode = new QSGNode();

    // Add children in render order (back to front)
    appendChildNode(fieldSurfaceNode);  // Field surface first (furthest back)
    appendChildNode(backgroundNode);    // Grid lines
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

    // Connect to update() when properties change
    connect(this, &FieldViewItem::zoomChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::cameraXChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::cameraYChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::cameraRotationChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::cameraPitchChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::boundaryColorChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::guidanceColorChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::gridColorChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::fieldColorChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::backgroundColorChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::vehicleColorChanged, this, &QQuickItem::update);
    connect(this, &FieldViewItem::isTextureOnChanged, this, &QQuickItem::update);
}

FieldViewItem::~FieldViewItem()
{
}

// ============================================================================
// Camera Property Accessors
// ============================================================================

double FieldViewItem::zoom() const { return m_zoom; }
void FieldViewItem::setZoom(double value) { m_zoom = value; }
QBindable<double> FieldViewItem::bindableZoom() { return &m_zoom; }

double FieldViewItem::cameraX() const { return m_cameraX; }
void FieldViewItem::setCameraX(double value) { m_cameraX = value; }
QBindable<double> FieldViewItem::bindableCameraX() { return &m_cameraX; }

double FieldViewItem::cameraY() const { return m_cameraY; }
void FieldViewItem::setCameraY(double value) { m_cameraY = value; }
QBindable<double> FieldViewItem::bindableCameraY() { return &m_cameraY; }

double FieldViewItem::cameraRotation() const { return m_cameraRotation; }
void FieldViewItem::setCameraRotation(double value) { m_cameraRotation = value; }
QBindable<double> FieldViewItem::bindableCameraRotation() { return &m_cameraRotation; }

double FieldViewItem::cameraPitch() const { return m_cameraPitch; }
void FieldViewItem::setCameraPitch(double value) { m_cameraPitch = value; }
QBindable<double> FieldViewItem::bindableCameraPitch() { return &m_cameraPitch; }

double FieldViewItem::fovDegrees() const { return m_fovDegrees; }
void FieldViewItem::setFovDegrees(double value) { m_fovDegrees = value; }
QBindable<double> FieldViewItem::bindableFovDegrees() { return &m_fovDegrees; }

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

bool FieldViewItem::showGrid() const { return m_showGrid; }
void FieldViewItem::setShowGrid(bool value) { m_showGrid = value; }
QBindable<bool> FieldViewItem::bindableShowGrid() { return &m_showGrid; }

bool FieldViewItem::isTextureOn() const { return m_isTextureOn; }
void FieldViewItem::setIsTextureOn(bool value) { m_isTextureOn = value; m_fieldSurfaceDirty = true; }
QBindable<bool> FieldViewItem::bindableIsTextureOn() { return &m_isTextureOn; }

// ============================================================================
// Color Property Accessors
// ============================================================================

QColor FieldViewItem::boundaryColor() const { return m_boundaryColor; }
void FieldViewItem::setBoundaryColor(const QColor &color) { m_boundaryColor = color; }
QBindable<QColor> FieldViewItem::bindableBoundaryColor() { return &m_boundaryColor; }

QColor FieldViewItem::guidanceColor() const { return m_guidanceColor; }
void FieldViewItem::setGuidanceColor(const QColor &color) { m_guidanceColor = color; }
QBindable<QColor> FieldViewItem::bindableGuidanceColor() { return &m_guidanceColor; }

QColor FieldViewItem::gridColor() const { return m_gridColor; }
void FieldViewItem::setGridColor(const QColor &color) { m_gridColor = color; }
QBindable<QColor> FieldViewItem::bindableGridColor() { return &m_gridColor; }

QColor FieldViewItem::fieldColor() const { return m_fieldColor; }
void FieldViewItem::setFieldColor(const QColor &color) { m_fieldColor = color; }
QBindable<QColor> FieldViewItem::bindableFieldColor() { return &m_fieldColor; }

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
    m_currentMvp = buildMvpMatrix();

    // Note: We don't set the matrix on the transform node because QSGTransformNode
    // is for 2D scene graph transforms, not 3D MVP matrices. Instead, we pass
    // the MVP matrix to each material's shader.

    // Always update field surface (it changes with camera position)
    updateFieldSurfaceNode(rootNode);

    // Update child nodes based on dirty flags
    if (m_showBoundary && m_boundaryDirty) {
        updateBoundaryNode(rootNode);
        m_boundaryDirty = false;
    }

    if (m_showCoverage && m_coverageDirty) {
        updateCoverageNode(rootNode);
        m_coverageDirty = false;
    }

    if (m_showGuidance && m_guidanceDirty) {
        updateGuidanceNode(rootNode);
        m_guidanceDirty = false;
    }

    if (m_showGrid) {
        // Grid always updates when camera moves (position-dependent)
        updateGridNode(rootNode);
        m_gridDirty = false;
    }

    // Vehicle always updates (position changes frequently)
    if (m_showVehicle) {
        updateVehicleNode(rootNode);
    }

    return rootNode;
}

// ============================================================================
// Matrix Building
// ============================================================================

QMatrix4x4 FieldViewItem::buildMvpMatrix() const
{
    // Standard 3D MVP: world coords -> NDC (-1 to 1)
    QMatrix4x4 mvp3d = buildProjectionMatrix() * buildViewMatrix();

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

    if (w <= 0 || h <= 0)
        return mvp3d;  // Fallback if size not set yet

    QMatrix4x4 ndcToItem;
    // First translate NDC origin to item center, then scale
    ndcToItem.translate(w / 2.0f, h / 2.0f, 0.0f);
    ndcToItem.scale(w / 2.0f, -h / 2.0f, 1.0f);  // Negative Y to flip (Qt Y is down)

    return ndcToItem * mvp3d;
}

QMatrix4x4 FieldViewItem::buildProjectionMatrix() const
{
    QMatrix4x4 projection;

    // Set up perspective projection
    float aspect = static_cast<float>(width()) / static_cast<float>(height());
    if (aspect <= 0)
        aspect = 1.0f;

    // FOV is set via fovDegrees property (default 40.1 degrees matches OpenGL's fovy = 0.7 radians)
    // Far plane uses camDistanceFactor * camSetDistance where camDistanceFactor = -2
    // Since m_zoom is positive (abs of camSetDistance), far = 2 * m_zoom
    float fov = static_cast<float>(m_fovDegrees);
    float farPlane = static_cast<float>(2.0 * m_zoom);
    if (farPlane < 100.0f) farPlane = 100.0f;  // Minimum far plane

    projection.perspective(fov, aspect, 1.0f, farPlane);

    return projection;
}

QMatrix4x4 FieldViewItem::buildViewMatrix() const
{
    QMatrix4x4 view;

    // Match OpenGL version: camera distance = camSetDistance * 0.5
    // m_zoom is positive (abs of camSetDistance), so we use -m_zoom * 0.5
    view.translate(0, 0, static_cast<float>(-m_zoom * 0.5));

    // Apply pitch (tilt)
    view.rotate(static_cast<float>(m_cameraPitch), 1.0f, 0.0f, 0.0f);

    // Apply rotation (yaw) - camera follows vehicle heading
    view.rotate(static_cast<float>(-m_cameraRotation), 0.0f, 0.0f, 1.0f);

    // Translate to camera position (center on vehicle/target)
    view.translate(static_cast<float>(-m_cameraX), static_cast<float>(-m_cameraY), 0.0f);

    return view;
}

// ============================================================================
// Node Update Methods
// ============================================================================

void FieldViewItem::updateBoundaryNode(FieldViewNode *rootNode)
{
    // Clear existing boundary geometry
    while (rootNode->boundaryNode->childCount() > 0) {
        QSGNode *child = rootNode->boundaryNode->firstChild();
        rootNode->boundaryNode->removeChildNode(child);
        delete child;
    }

    // Create geometry for each boundary
    for (const auto &boundary : m_renderData.boundaries) {
        if (boundary.size() < 3)
            continue;

        // Create geometry node for this boundary
        auto *geomNode = new QSGGeometryNode();

        // Create line loop geometry
        auto *geometry = AOGGeometry::createLineLoopGeometry(boundary);
        if (!geometry)
            continue;

        geomNode->setGeometry(geometry);
        geomNode->setFlag(QSGNode::OwnsGeometry);

        // Create material with MVP matrix
        auto *material = new AOGFlatColorMaterial();
        material->setColor(m_boundaryColor);
        material->setMvpMatrix(m_currentMvp);

        geomNode->setMaterial(material);
        geomNode->setFlag(QSGNode::OwnsMaterial);

        rootNode->boundaryNode->appendChildNode(geomNode);
    }
}

void FieldViewItem::updateCoverageNode(FieldViewNode *rootNode)
{
    // Clear existing coverage geometry
    while (rootNode->coverageNode->childCount() > 0) {
        QSGNode *child = rootNode->coverageNode->firstChild();
        rootNode->coverageNode->removeChildNode(child);
        delete child;
    }

    if (m_renderData.coverageVertices.isEmpty())
        return;

    // Create colored triangles for coverage
    auto *geomNode = new QSGGeometryNode();

    auto *geometry = AOGGeometry::createColoredTrianglesGeometry(
        m_renderData.coverageVertices,
        m_renderData.coverageColors
    );
    if (!geometry) {
        delete geomNode;
        return;
    }

    geomNode->setGeometry(geometry);
    geomNode->setFlag(QSGNode::OwnsGeometry);

    // Use vertex color material for coverage with MVP matrix
    auto *material = new AOGVertexColorMaterial();
    material->setMvpMatrix(m_currentMvp);
    geomNode->setMaterial(material);
    geomNode->setFlag(QSGNode::OwnsMaterial);

    rootNode->coverageNode->appendChildNode(geomNode);
}

void FieldViewItem::updateGuidanceNode(FieldViewNode *rootNode)
{
    // Clear existing guidance geometry
    while (rootNode->guidanceNode->childCount() > 0) {
        QSGNode *child = rootNode->guidanceNode->firstChild();
        rootNode->guidanceNode->removeChildNode(child);
        delete child;
    }

    if (!m_renderData.hasGuidance || m_renderData.guidanceLine.isEmpty())
        return;

    // Create line strip for guidance line
    auto *geomNode = new QSGGeometryNode();

    auto *geometry = AOGGeometry::createLineStripGeometry(m_renderData.guidanceLine);
    if (!geometry) {
        delete geomNode;
        return;
    }

    geomNode->setGeometry(geometry);
    geomNode->setFlag(QSGNode::OwnsGeometry);

    auto *material = new AOGFlatColorMaterial();
    material->setColor(m_guidanceColor);
    material->setMvpMatrix(m_currentMvp);

    geomNode->setMaterial(material);
    geomNode->setFlag(QSGNode::OwnsMaterial);

    rootNode->guidanceNode->appendChildNode(geomNode);
}

void FieldViewItem::updateVehicleNode(FieldViewNode *rootNode)
{
    // Clear existing vehicle geometry
    while (rootNode->vehicleNode->childCount() > 0) {
        QSGNode *child = rootNode->vehicleNode->firstChild();
        rootNode->vehicleNode->removeChildNode(child);
        delete child;
    }

    // Create a simple triangle for vehicle representation
    // This will be replaced with proper vehicle geometry later
    QVector<QVector3D> vehicleTriangle;

    // Transform vehicle shape by position and heading
    double heading = qDegreesToRadians(m_renderData.vehicleHeading);
    double cosH = qCos(heading);
    double sinH = qSin(heading);

    // Vehicle arrow shape (pointing in direction of travel)
    double size = 2.0;  // meters

    // Transform local coordinates to world coordinates
    auto transformPoint = [&](double lx, double ly) -> QVector3D {
        double wx = m_renderData.vehicleX + (lx * cosH - ly * sinH);
        double wy = m_renderData.vehicleY + (lx * sinH + ly * cosH);
        return QVector3D(static_cast<float>(wx), static_cast<float>(wy), 0.0f);
    };

    // Arrow pointing forward (positive Y in local coords)
    vehicleTriangle.append(transformPoint(0, size));      // Front
    vehicleTriangle.append(transformPoint(-size/2, -size/2)); // Back left
    vehicleTriangle.append(transformPoint(size/2, -size/2));  // Back right

    auto *geomNode = new QSGGeometryNode();

    auto *geometry = AOGGeometry::createTrianglesGeometry(vehicleTriangle);
    if (!geometry) {
        delete geomNode;
        return;
    }

    geomNode->setGeometry(geometry);
    geomNode->setFlag(QSGNode::OwnsGeometry);

    auto *material = new AOGFlatColorMaterial();
    material->setColor(m_vehicleColor);  // Use the configurable vehicle color
    material->setMvpMatrix(m_currentMvp);

    geomNode->setMaterial(material);
    geomNode->setFlag(QSGNode::OwnsMaterial);

    rootNode->vehicleNode->appendChildNode(geomNode);
}

void FieldViewItem::updateGridNode(FieldViewNode *rootNode)
{
    // Clear existing grid geometry
    while (rootNode->backgroundNode->childCount() > 0) {
        QSGNode *child = rootNode->backgroundNode->firstChild();
        rootNode->backgroundNode->removeChildNode(child);
        delete child;
    }

    // Create grid lines similar to WorldGrid::DrawWorldGrid
    QVector<QVector3D> gridLines;

    // Get extents from WorldGrid singleton if available
    double eastingMin = m_cameraX - 200;
    double eastingMax = m_cameraX + 200;
    double northingMin = m_cameraY - 200;
    double northingMax = m_cameraY + 200;

    // Calculate grid spacing based on zoom (similar to Camera::SetZoom)
    double gridZoom = 10.0;  // Default 10 meter spacing
    double camDistance = m_zoom;
    if (camDistance <= 20000 && camDistance > 10000) gridZoom = 2012;
    else if (camDistance <= 10000 && camDistance > 5000) gridZoom = 1006;
    else if (camDistance <= 5000 && camDistance > 2000) gridZoom = 503;
    else if (camDistance <= 2000 && camDistance > 1000) gridZoom = 201.2;
    else if (camDistance <= 1000 && camDistance > 500) gridZoom = 100.6;
    else if (camDistance <= 500 && camDistance > 250) gridZoom = 50.3;
    else if (camDistance <= 250 && camDistance > 150) gridZoom = 25.15;
    else if (camDistance <= 150 && camDistance > 50) gridZoom = 10.06;
    else if (camDistance <= 50 && camDistance > 1) gridZoom = 5.03;

    // Helper function to round mid away from zero (like glm::roundMidAwayFromZero)
    auto roundMidAwayFromZero = [](double val) -> double {
        return (val >= 0) ? qFloor(val + 0.5) : qCeil(val - 0.5);
    };

    // Create vertical lines (constant easting, varying northing)
    // Match WorldGrid: for (double num = roundMidAwayFromZero(eastingMin / gridZoom) * gridZoom; num < eastingMax; num += gridZoom)
    for (double num = roundMidAwayFromZero(eastingMin / gridZoom) * gridZoom; num < eastingMax; num += gridZoom) {
        if (num < eastingMin) continue;
        gridLines.append(QVector3D(static_cast<float>(num), static_cast<float>(northingMax), 0.1f));
        gridLines.append(QVector3D(static_cast<float>(num), static_cast<float>(northingMin), 0.1f));
    }

    // Create horizontal lines (constant northing, varying easting)
    // Match WorldGrid: for (double num2 = roundMidAwayFromZero(northingMin / gridZoom) * gridZoom; num2 < northingMax; num2 += gridZoom)
    for (double num2 = roundMidAwayFromZero(northingMin / gridZoom) * gridZoom; num2 < northingMax; num2 += gridZoom) {
        if (num2 < northingMin) continue;
        gridLines.append(QVector3D(static_cast<float>(eastingMax), static_cast<float>(num2), 0.1f));
        gridLines.append(QVector3D(static_cast<float>(eastingMin), static_cast<float>(num2), 0.1f));
    }

    if (gridLines.isEmpty())
        return;

    auto *geomNode = new QSGGeometryNode();

    auto *geometry = AOGGeometry::createLinesGeometry(gridLines);
    if (!geometry) {
        delete geomNode;
        return;
    }

    geomNode->setGeometry(geometry);
    geomNode->setFlag(QSGNode::OwnsGeometry);

    auto *material = new AOGFlatColorMaterial();
    material->setColor(m_gridColor);  // Use the configurable grid color
    material->setMvpMatrix(m_currentMvp);  // Set the MVP matrix!

    geomNode->setMaterial(material);
    geomNode->setFlag(QSGNode::OwnsMaterial);

    rootNode->backgroundNode->appendChildNode(geomNode);
}

void FieldViewItem::updateFieldSurfaceNode(FieldViewNode *rootNode)
{
    //qCDebug(fieldviewitem_log) << "FieldViewItem::updateFieldSurfaceNode - isTextureOn:" << m_isTextureOn.value()
    //           << "window:" << window();

    // Clear existing field surface geometry
    while (rootNode->fieldSurfaceNode->childCount() > 0) {
        QSGNode *child = rootNode->fieldSurfaceNode->firstChild();
        rootNode->fieldSurfaceNode->removeChildNode(child);
        delete child;
    }

    // Get field extents from WorldGrid (6000 meter grid size by default)
    auto *worldGrid = WorldGrid::instance();
    double eastingMin = worldGrid->eastingMin();
    double eastingMax = worldGrid->eastingMax();
    double northingMin = worldGrid->northingMin();
    double northingMax = worldGrid->northingMax();

    // Z position slightly behind (negative Z is further away in our coordinate system)
    const float surfaceZ = -0.10f;

    // Check if we should use texture and if it's available
    bool useTexture = m_isTextureOn && m_floorTexture;

    if (m_isTextureOn && !m_floorTexture && window()) {
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
                useTexture = true;
            }
        }
    }

    if (useTexture) {
        // Textured field surface
        // Use WorldGrid's count for texture tiling (default 40, adjusted based on zoom)
        int count = static_cast<int>(worldGrid->count());

        // Create textured quad geometry manually (4 vertices for triangle strip)
        auto *geometry = new QSGGeometry(AOGGeometry::texturedVertexAttributes(), 4);
        geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);

        TexturedVertex *data = static_cast<TexturedVertex *>(geometry->vertexData());

        // Vertex order for triangle strip: top-left, top-right, bottom-left, bottom-right
        // This matches WorldGrid's vertex order
        data[0].x = static_cast<float>(eastingMin);
        data[0].y = static_cast<float>(northingMax);
        data[0].z = surfaceZ;
        data[0].u = 0.0f;
        data[0].v = 0.0f;

        data[1].x = static_cast<float>(eastingMax);
        data[1].y = static_cast<float>(northingMax);
        data[1].z = surfaceZ;
        data[1].u = static_cast<float>(count);
        data[1].v = 0.0f;

        data[2].x = static_cast<float>(eastingMin);
        data[2].y = static_cast<float>(northingMin);
        data[2].z = surfaceZ;
        data[2].u = 0.0f;
        data[2].v = static_cast<float>(count);

        data[3].x = static_cast<float>(eastingMax);
        data[3].y = static_cast<float>(northingMin);
        data[3].z = surfaceZ;
        data[3].u = static_cast<float>(count);
        data[3].v = static_cast<float>(count);

        auto *geomNode = new QSGGeometryNode();
        geomNode->setGeometry(geometry);
        geomNode->setFlag(QSGNode::OwnsGeometry);

        auto *material = new AOGTextureMaterial();
        material->setTexture(m_floorTexture);
        material->setColor(m_fieldColor);  // Tint color for the texture
        material->setUseColor(true);       // Enable color tinting (like OpenGL version)
        material->setMvpMatrix(m_currentMvp);

        geomNode->setMaterial(material);
        geomNode->setFlag(QSGNode::OwnsMaterial);

        rootNode->fieldSurfaceNode->appendChildNode(geomNode);
    } else {
        // Solid color field surface
        QVector<QVector3D> surfaceQuad;
        // Vertex order for triangle strip: top-left, top-right, bottom-left, bottom-right
        surfaceQuad.append(QVector3D(static_cast<float>(eastingMin), static_cast<float>(northingMax), surfaceZ));
        surfaceQuad.append(QVector3D(static_cast<float>(eastingMax), static_cast<float>(northingMax), surfaceZ));
        surfaceQuad.append(QVector3D(static_cast<float>(eastingMin), static_cast<float>(northingMin), surfaceZ));
        surfaceQuad.append(QVector3D(static_cast<float>(eastingMax), static_cast<float>(northingMin), surfaceZ));

        auto *geometry = AOGGeometry::createTriangleStripGeometry(surfaceQuad);
        if (!geometry)
            return;

        auto *geomNode = new QSGGeometryNode();
        geomNode->setGeometry(geometry);
        geomNode->setFlag(QSGNode::OwnsGeometry);

        auto *material = new AOGFlatColorMaterial();
        material->setColor(m_fieldColor);
        material->setMvpMatrix(m_currentMvp);

        geomNode->setMaterial(material);
        geomNode->setFlag(QSGNode::OwnsMaterial);

        rootNode->fieldSurfaceNode->appendChildNode(geomNode);
    }
}
