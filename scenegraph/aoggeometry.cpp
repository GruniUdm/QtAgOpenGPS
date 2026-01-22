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
// Thick Line Geometry (for screen-space width lines)
// ============================================================================

const QSGGeometry::AttributeSet &thickLineAttributes()
{
    // Attribute layout for thick line vertices:
    // - attribute 0: pos (vec4) - current vertex position (with w=1)
    // - attribute 1: nextPos (vec4) - neighbor position for direction calculation
    // - attribute 2: side (float) - which side of line (-1 or +1)
    // For vertices at endpoint A: pos=A, nextPos=B
    // For vertices at endpoint B: pos=B, nextPos=A (swapped so shader always uses pos)

    static QSGGeometry::Attribute attrs[] = {
        QSGGeometry::Attribute::create(0, 4, QSGGeometry::FloatType, true),   // pos (vec4)
        QSGGeometry::Attribute::create(1, 4, QSGGeometry::FloatType, false),  // nextPos (vec4)
        QSGGeometry::Attribute::create(2, 1, QSGGeometry::FloatType, false),  // side
    };

    static QSGGeometry::AttributeSet attrSet = {
        3,                          // attribute count
        sizeof(ThickLineVertex),    // stride
        attrs
    };

    return attrSet;
}

QSGGeometry *createLinesGeometry2(const QVector<QVector3D> &points)
{
    if (points.size() < 2)
        return nullptr;

    /*
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
    */
    if (points.size() < 2)
        return nullptr;

    auto *geometry = new QSGGeometry(thickLineAttributes(), points.size());
    geometry->setDrawingMode(QSGGeometry::DrawLines);
    geometry->setLineWidth(1.0f);

    float *data = static_cast<float *>(geometry->vertexData());
    for (const auto &pt : points) {
        *data++ = pt.x();
        *data++ = pt.y();
        *data++ = pt.z();
        *data++ = 1;
        *data++ = pt.x();
        *data++ = pt.y();
        *data++ = pt.z();
        *data++ = 1;
        *data++ = 1;
    }

    return geometry;
}

QSGGeometry *createThickLineGeometry(const QVector<QVector3D> &points)
{
    // Each consecutive pair of points forms a line segment
    // Each segment needs 4 vertices (triangle strip for a quad)
    // With degenerate triangles to connect segments

    if (points.size() < 2)
        return nullptr;

    int numSegments = points.size() - 1;
    // 4 vertices per segment, plus 2 degenerate vertices between segments (except last)
    int numVertices = numSegments * 4 + (numSegments - 1) * 2;

    if (numSegments == 1) {
        // Single segment, no degenerate triangles needed
        numVertices = 4;
    }

    auto *geometry = new QSGGeometry(thickLineAttributes(), numVertices);
    //geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);
    geometry->setDrawingMode(QSGGeometry::DrawLines);

    ThickLineVertex *data = static_cast<ThickLineVertex *>(geometry->vertexData());
    int idx = 0;

    for (int seg = 0; seg < numSegments; ++seg) {
        const QVector3D &a = points[seg];
        const QVector3D &b = points[seg + 1];

        // First vertex of segment (at A, side -1): pos=A, nextPos=B
        data[idx].ax = a.x(); data[idx].ay = a.y(); data[idx].az = a.z(); data[idx].aw = 1;
        data[idx].bx = b.x(); data[idx].by = b.y(); data[idx].bz = b.z(); data[idx].bw = 1;
        data[idx].side = -1.0f;
        idx++;

        // Second vertex (at A, side +1): pos=A, nextPos=B
        data[idx].ax = a.x(); data[idx].ay = a.y(); data[idx].az = a.z(); data[idx].aw = 1;
        data[idx].bx = b.x(); data[idx].by = b.y(); data[idx].bz = b.z(); data[idx].bw = 1;
        data[idx].side = 1.0f;
        idx++;

        // Third vertex (at B, side -1): pos=B, nextPos=A (swapped!)
        // Side is negated (+1) because swapping positions reverses the direction/normal
        data[idx].ax = b.x(); data[idx].ay = b.y(); data[idx].az = b.z(); data[idx].aw = 1;
        data[idx].bx = a.x(); data[idx].by = a.y(); data[idx].bz = a.z(); data[idx].bw = 1;
        data[idx].side = 1.0f;  // Negated to compensate for reversed normal
        idx++;

        // Fourth vertex (at B, side +1): pos=B, nextPos=A (swapped!)
        // Side is negated (-1) because swapping positions reverses the direction/normal
        data[idx].ax = b.x(); data[idx].ay = b.y(); data[idx].az = b.z(); data[idx].aw = 1;
        data[idx].bx = a.x(); data[idx].by = a.y(); data[idx].bz = a.z(); data[idx].bw = 1;
        data[idx].side = -1.0f;  // Negated to compensate for reversed normal
        idx++;

        // Add degenerate triangles between segments (repeat last vertex, then first of next)
        if (seg < numSegments - 1) {
            // Repeat last vertex
            data[idx] = data[idx - 1];
            idx++;

            // Pre-duplicate first vertex of next segment
            const QVector3D &nextA = points[seg + 1];
            const QVector3D &nextB = points[seg + 2];
            data[idx].ax = nextA.x(); data[idx].ay = nextA.y(); data[idx].az = nextA.z(); data[idx].aw = 1;
            data[idx].bx = nextB.x(); data[idx].by = nextB.y(); data[idx].bz = nextB.z(); data[idx].bw = 1;
            data[idx].side = -1.0f;
            idx++;
        }
    }

    return geometry;
}

QSGGeometry *createThickLineLoopGeometry(const QVector<QVector3D> &points)
{
    if (points.size() < 3)
        return nullptr;

    // Create a closed loop by appending the first point to the end
    QVector<QVector3D> closedPoints = points;
    closedPoints.append(points.first());

    return createThickLineGeometry(closedPoints);
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
