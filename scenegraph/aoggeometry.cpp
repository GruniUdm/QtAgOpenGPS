// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Geometry building utilities for scene graph rendering

#include "aoggeometry.h"
#include "vec3.h"

namespace AOGGeometry {

// ============================================================================
// Custom Attribute Sets
// ============================================================================

const QSGGeometry::AttributeSet &positionAttributes()
{
    static QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 3, QSGGeometry::FloatType, true)  // position
    };
    static QSGGeometry::AttributeSet attributeSet = {
        1,                    // attribute count
        sizeof(float) * 3,    // stride (12 bytes per vertex)
        attributes
    };
    return attributeSet;
}

const QSGGeometry::AttributeSet &colorVertexAttributes()
{
    static QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 3, QSGGeometry::FloatType, true),   // position (location 0)
        QSGGeometry::Attribute::create(1, 4, QSGGeometry::FloatType, false)   // color (location 1)
    };
    static QSGGeometry::AttributeSet attributeSet = {
        2,                         // attribute count
        sizeof(ColorVertex),       // stride (28 bytes per vertex)
        attributes
    };
    return attributeSet;
}

const QSGGeometry::AttributeSet &texturedVertexAttributes()
{
    static QSGGeometry::Attribute attributes[] = {
        QSGGeometry::Attribute::create(0, 3, QSGGeometry::FloatType, true),   // position (location 0)
        QSGGeometry::Attribute::create(1, 2, QSGGeometry::FloatType, false)   // texcoord (location 1)
    };
    static QSGGeometry::AttributeSet attributeSet = {
        2,                           // attribute count
        sizeof(TexturedVertex),      // stride (20 bytes per vertex)
        attributes
    };
    return attributeSet;
}

// ============================================================================
// Geometry Creation Functions
// ============================================================================

QSGGeometry *createLineStripGeometry(const QVector<QVector3D> &points)
{
    if (points.isEmpty())
        return nullptr;

    auto *geometry = new QSGGeometry(positionAttributes(), points.size());
    geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
    geometry->setLineWidth(1.0f);

    float *data = static_cast<float *>(geometry->vertexData());
    for (const auto &pt : points) {
        *data++ = pt.x();
        *data++ = pt.y();
        *data++ = pt.z();
    }

    return geometry;
}

QSGGeometry *createLineLoopGeometry(const QVector<QVector3D> &points)
{
    if (points.isEmpty())
        return nullptr;

    auto *geometry = new QSGGeometry(positionAttributes(), points.size());
    geometry->setDrawingMode(QSGGeometry::DrawLineLoop);
    geometry->setLineWidth(1.0f);

    float *data = static_cast<float *>(geometry->vertexData());
    for (const auto &pt : points) {
        *data++ = pt.x();
        *data++ = pt.y();
        *data++ = pt.z();
    }

    return geometry;
}

QSGGeometry *createLinesGeometry(const QVector<QVector3D> &points)
{
    if (points.size() < 2)
        return nullptr;

    auto *geometry = new QSGGeometry(positionAttributes(), points.size());
    geometry->setDrawingMode(QSGGeometry::DrawLines);
    geometry->setLineWidth(1.0f);

    float *data = static_cast<float *>(geometry->vertexData());
    for (const auto &pt : points) {
        *data++ = pt.x();
        *data++ = pt.y();
        *data++ = pt.z();
    }

    return geometry;
}

QSGGeometry *createTriangleStripGeometry(const QVector<QVector3D> &vertices)
{
    if (vertices.size() < 3)
        return nullptr;

    auto *geometry = new QSGGeometry(positionAttributes(), vertices.size());
    geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);

    float *data = static_cast<float *>(geometry->vertexData());
    for (const auto &v : vertices) {
        *data++ = v.x();
        *data++ = v.y();
        *data++ = v.z();
    }

    return geometry;
}

QSGGeometry *createTriangleFanGeometry(const QVector<QVector3D> &vertices)
{
    if (vertices.size() < 3)
        return nullptr;

    auto *geometry = new QSGGeometry(positionAttributes(), vertices.size());
    geometry->setDrawingMode(QSGGeometry::DrawTriangleFan);

    float *data = static_cast<float *>(geometry->vertexData());
    for (const auto &v : vertices) {
        *data++ = v.x();
        *data++ = v.y();
        *data++ = v.z();
    }

    return geometry;
}

QSGGeometry *createTrianglesGeometry(const QVector<QVector3D> &vertices)
{
    if (vertices.size() < 3)
        return nullptr;

    auto *geometry = new QSGGeometry(positionAttributes(), vertices.size());
    geometry->setDrawingMode(QSGGeometry::DrawTriangles);

    float *data = static_cast<float *>(geometry->vertexData());
    for (const auto &v : vertices) {
        *data++ = v.x();
        *data++ = v.y();
        *data++ = v.z();
    }

    return geometry;
}

QSGGeometry *createColoredLineStripGeometry(const QVector<QVector3D> &points,
                                            const QVector<QColor> &colors)
{
    if (points.isEmpty() || points.size() != colors.size())
        return nullptr;

    auto *geometry = new QSGGeometry(colorVertexAttributes(), points.size());
    geometry->setDrawingMode(QSGGeometry::DrawLineStrip);
    geometry->setLineWidth(1.0f);

    ColorVertex *data = static_cast<ColorVertex *>(geometry->vertexData());
    for (int i = 0; i < points.size(); ++i) {
        data[i].x = points[i].x();
        data[i].y = points[i].y();
        data[i].z = points[i].z();
        data[i].r = static_cast<float>(colors[i].redF());
        data[i].g = static_cast<float>(colors[i].greenF());
        data[i].b = static_cast<float>(colors[i].blueF());
        data[i].a = static_cast<float>(colors[i].alphaF());
    }

    return geometry;
}

QSGGeometry *createColoredTrianglesGeometry(const QVector<QVector3D> &vertices,
                                            const QVector<QColor> &colors)
{
    if (vertices.size() < 3 || vertices.size() != colors.size())
        return nullptr;

    auto *geometry = new QSGGeometry(colorVertexAttributes(), vertices.size());
    geometry->setDrawingMode(QSGGeometry::DrawTriangles);

    ColorVertex *data = static_cast<ColorVertex *>(geometry->vertexData());
    for (int i = 0; i < vertices.size(); ++i) {
        data[i].x = vertices[i].x();
        data[i].y = vertices[i].y();
        data[i].z = vertices[i].z();
        data[i].r = static_cast<float>(colors[i].redF());
        data[i].g = static_cast<float>(colors[i].greenF());
        data[i].b = static_cast<float>(colors[i].blueF());
        data[i].a = static_cast<float>(colors[i].alphaF());
    }

    return geometry;
}

QSGGeometry *createTexturedQuadGeometry(const QRectF &rect, const QRectF &texCoords, float z)
{
    auto *geometry = new QSGGeometry(texturedVertexAttributes(), 4);
    geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);

    TexturedVertex *data = static_cast<TexturedVertex *>(geometry->vertexData());

    // Bottom-left
    data[0].x = static_cast<float>(rect.left());
    data[0].y = static_cast<float>(rect.bottom());
    data[0].z = z;
    data[0].u = static_cast<float>(texCoords.left());
    data[0].v = static_cast<float>(texCoords.bottom());

    // Bottom-right
    data[1].x = static_cast<float>(rect.right());
    data[1].y = static_cast<float>(rect.bottom());
    data[1].z = z;
    data[1].u = static_cast<float>(texCoords.right());
    data[1].v = static_cast<float>(texCoords.bottom());

    // Top-left
    data[2].x = static_cast<float>(rect.left());
    data[2].y = static_cast<float>(rect.top());
    data[2].z = z;
    data[2].u = static_cast<float>(texCoords.left());
    data[2].v = static_cast<float>(texCoords.top());

    // Top-right
    data[3].x = static_cast<float>(rect.right());
    data[3].y = static_cast<float>(rect.top());
    data[3].z = z;
    data[3].u = static_cast<float>(texCoords.right());
    data[3].v = static_cast<float>(texCoords.top());

    return geometry;
}

// ============================================================================
// Geometry Update Functions
// ============================================================================

bool updateLineStripGeometry(QSGGeometry *geometry, const QVector<QVector3D> &points)
{
    if (!geometry || points.isEmpty())
        return false;

    bool resized = false;
    if (geometry->vertexCount() != points.size()) {
        geometry->allocate(points.size());
        resized = true;
    }

    float *data = static_cast<float *>(geometry->vertexData());
    for (const auto &pt : points) {
        *data++ = pt.x();
        *data++ = pt.y();
        *data++ = pt.z();
    }

    geometry->markVertexDataDirty();
    return resized;
}

bool updateTrianglesGeometry(QSGGeometry *geometry, const QVector<QVector3D> &vertices)
{
    if (!geometry || vertices.size() < 3)
        return false;

    bool resized = false;
    if (geometry->vertexCount() != vertices.size()) {
        geometry->allocate(vertices.size());
        resized = true;
    }

    float *data = static_cast<float *>(geometry->vertexData());
    for (const auto &v : vertices) {
        *data++ = v.x();
        *data++ = v.y();
        *data++ = v.z();
    }

    geometry->markVertexDataDirty();
    return resized;
}

bool updateColoredTrianglesGeometry(QSGGeometry *geometry,
                                    const QVector<QVector3D> &vertices,
                                    const QVector<QColor> &colors)
{
    if (!geometry || vertices.size() < 3 || vertices.size() != colors.size())
        return false;

    bool resized = false;
    if (geometry->vertexCount() != vertices.size()) {
        geometry->allocate(vertices.size());
        resized = true;
    }

    ColorVertex *data = static_cast<ColorVertex *>(geometry->vertexData());
    for (int i = 0; i < vertices.size(); ++i) {
        data[i].x = vertices[i].x();
        data[i].y = vertices[i].y();
        data[i].z = vertices[i].z();
        data[i].r = static_cast<float>(colors[i].redF());
        data[i].g = static_cast<float>(colors[i].greenF());
        data[i].b = static_cast<float>(colors[i].blueF());
        data[i].a = static_cast<float>(colors[i].alphaF());
    }

    geometry->markVertexDataDirty();
    return resized;
}

// ============================================================================
// Utility Functions
// ============================================================================

QVector<QVector3D> toQVector3D(const void *vec3Array, int count)
{
    QVector<QVector3D> result;
    result.reserve(count);

    const Vec3 *arr = static_cast<const Vec3 *>(vec3Array);
    for (int i = 0; i < count; ++i) {
        result.append(QVector3D(
            static_cast<float>(arr[i].easting),
            static_cast<float>(arr[i].northing),
            static_cast<float>(arr[i].heading)  // z-value, often used for height
        ));
    }
    return result;
}

QVector<int> triangulateConvex(int vertexCount)
{
    QVector<int> indices;
    if (vertexCount < 3)
        return indices;

    // Fan triangulation: vertex 0 is center
    indices.reserve((vertexCount - 2) * 3);
    for (int i = 1; i < vertexCount - 1; ++i) {
        indices.append(0);
        indices.append(i);
        indices.append(i + 1);
    }
    return indices;
}

QVector<QVector3D> triangulateEarClipping(const QVector<QVector3D> &polygon)
{
    QVector<QVector3D> result;
    if (polygon.size() < 3)
        return result;

    // Simple ear-clipping algorithm for convex/simple polygons
    // For production use, consider using a more robust library

    // Create a working copy of vertex indices
    QVector<int> indices;
    indices.reserve(polygon.size());
    for (int i = 0; i < polygon.size(); ++i)
        indices.append(i);

    // Simple convex assumption: use fan triangulation
    // TODO: Implement full ear-clipping for non-convex polygons
    result.reserve((polygon.size() - 2) * 3);
    for (int i = 1; i < polygon.size() - 1; ++i) {
        result.append(polygon[0]);
        result.append(polygon[i]);
        result.append(polygon[i + 1]);
    }

    return result;
}

} // namespace AOGGeometry
