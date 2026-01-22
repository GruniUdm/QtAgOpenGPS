// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Grid node implementation

#include "gridnode.h"
#include "aogmaterial.h"
#include "aoggeometry.h"
#include "thicklinematerial.h"

#include <QtMath>
#include <QVector3D>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(gridnode_log, "gridnode.qtagopengps")

GridNode::GridNode()
{
}

GridNode::~GridNode()
{
}

void GridNode::clearChildren()
{
    while (childCount() > 0) {
        QSGNode *child = firstChild();
        removeChildNode(child);
        delete child;
    }
    m_geomNode = nullptr;
}

void GridNode::update(const QMatrix4x4 &mvMatrix, const QMatrix4x4 &pMatrix, const QMatrix4x4 &ndcMatrix, const QSize viewportSize,
                      const QColor &gridColor, int lineWidth,
                      double eastingMin, double eastingMax,
                      double northingMin, double northingMax,
                      double gridSpacing)
{
    // Clear existing geometry and rebuild
    // TODO: Could optimize to reuse geometry if line count doesn't change
    clearChildren();

    // Create grid lines
    QVector<QVector3D> gridLines;

    // Helper function to round mid away from zero (like glm::roundMidAwayFromZero)
    auto roundMidAwayFromZero = [](double val) -> double {
        return (val >= 0) ? qFloor(val + 0.5) : qCeil(val - 0.5);
    };

    // Create vertical lines (constant easting, varying northing)
    //for (double num = roundMidAwayFromZero(eastingMin / gridSpacing) * gridSpacing;
    //     num < eastingMax; num += gridSpacing) {
    //    if (num < eastingMin) continue;
        gridLines.append(QVector3D(static_cast<float>(0), static_cast<float>(northingMax), 0.1f));
        gridLines.append(QVector3D(static_cast<float>(0), static_cast<float>(northingMin), 0.1f));
    //}

    /*
    // Create horizontal lines (constant northing, varying easting)
    for (double num2 = roundMidAwayFromZero(northingMin / gridSpacing) * gridSpacing;
         num2 < northingMax; num2 += gridSpacing) {
        if (num2 < northingMin) continue;
        gridLines.append(QVector3D(static_cast<float>(eastingMax), static_cast<float>(num2), 0.1f));
        gridLines.append(QVector3D(static_cast<float>(eastingMin), static_cast<float>(num2), 0.1f));
    }*/

    if (gridLines.isEmpty())
        return;

    // ===== DEBUG: Replicate vertex shader calculations in C++ =====
    // Approach: Do thick line math in standard MVP clip space, then apply ndcMatrix at the end
    if (gridLines.size() >= 2) {
        // Step 1: Use standard MVP (no ndcMatrix) - gives clip space where NDC is [-1, 1]
        QMatrix4x4 mvp = pMatrix * mvMatrix;

        // Take first two points as test line segment
        QVector3D pointA = {0, 100, 0};
        QVector3D pointB = {0, -100, 0};

        // Transform to standard clip space
        QVector4D posA(pointA, 1.0f);
        QVector4D posB(pointB, 1.0f);

        QVector4D currClip = mvp * posA;
        QVector4D nextClip = mvp * posB;

        qDebug(gridnode_log) << "=== Thick Line Debug (MVP then NDC) ===";
        qDebug(gridnode_log) << "Viewport size:" << viewportSize;
        qDebug(gridnode_log) << "Point A (world):" << pointA;
        qDebug(gridnode_log) << "Point B (world):" << pointB;
        qDebug(gridnode_log) << "currClip (A) [MVP]:" << currClip;
        qDebug(gridnode_log) << "nextClip (B) [MVP]:" << nextClip;
        qDebug(gridnode_log) << "currClip.w:" << currClip.w() << (currClip.w() < 0 ? "(BEHIND CAMERA)" : "(in front)");
        qDebug(gridnode_log) << "nextClip.w:" << nextClip.w() << (nextClip.w() < 0 ? "(BEHIND CAMERA)" : "(in front)");

        // Step 2: Calculate line direction using homogeneous coordinates
        // Works correctly even when w is negative (vertex behind camera)
        QVector2D lineVec(
            nextClip.x() * currClip.w() - currClip.x() * nextClip.w(),
            nextClip.y() * currClip.w() - currClip.y() * nextClip.w()
        );
        float len = lineVec.length();

        qDebug(gridnode_log) << "lineVec (homogeneous):" << lineVec << "length:" << len;

        // Guard against degenerate lines
        QVector2D dir = (len > 0.0001f) ? (lineVec / len) : QVector2D(1.0f, 0.0f);
        QVector2D normal(-dir.y(), dir.x());

        qDebug(gridnode_log) << "dir (normalized):" << dir;
        qDebug(gridnode_log) << "normal:" << normal;

        // Step 3: Calculate offset in standard clip space
        // In NDC, range is [-1, 1] = 2 units for viewport width
        // 1 pixel = 2 / viewportWidth NDC units
        // In clip space: offset = pixelOffset * (2 / viewportWidth) * abs(w)
        float testLineWidth = 2.0f;  // 2 pixels total
        float halfWidth = testLineWidth * 0.5f;
        float pixelToNDC_X = 2.0f / viewportSize.width();
        float pixelToNDC_Y = 2.0f / viewportSize.height();

        qDebug(gridnode_log) << "pixelToNDC: X=" << pixelToNDC_X << "Y=" << pixelToNDC_Y;

        // Vertex 0: at A, side = -1
        float side0 = -1.0f;
        QVector2D ndcOffset0(normal.x() * halfWidth * pixelToNDC_X * side0,
                             normal.y() * halfWidth * pixelToNDC_Y * side0);
        QVector2D clipOffset0 = ndcOffset0 * qAbs(currClip.w());
        QVector4D clip0(currClip.x() + clipOffset0.x(), currClip.y() + clipOffset0.y(), currClip.z(), currClip.w());

        // Vertex 1: at A, side = +1
        float side1 = 1.0f;
        QVector2D ndcOffset1(normal.x() * halfWidth * pixelToNDC_X * side1,
                             normal.y() * halfWidth * pixelToNDC_Y * side1);
        QVector2D clipOffset1 = ndcOffset1 * qAbs(currClip.w());
        QVector4D clip1(currClip.x() + clipOffset1.x(), currClip.y() + clipOffset1.y(), currClip.z(), currClip.w());

        // For B vertices, positions are swapped so direction is reversed
        QVector2D lineVecB(
            currClip.x() * nextClip.w() - nextClip.x() * currClip.w(),
            currClip.y() * nextClip.w() - nextClip.y() * currClip.w()
        );
        float lenB = lineVecB.length();
        QVector2D dirB = (lenB > 0.0001f) ? (lineVecB / lenB) : QVector2D(1.0f, 0.0f);
        QVector2D normalB(-dirB.y(), dirB.x());

        qDebug(gridnode_log) << "normalB (reversed):" << normalB;

        // Vertex 2: at B, side was -1 but negated to +1
        float side2 = 1.0f;
        QVector2D ndcOffset2(normalB.x() * halfWidth * pixelToNDC_X * side2,
                             normalB.y() * halfWidth * pixelToNDC_Y * side2);
        QVector2D clipOffset2 = ndcOffset2 * qAbs(nextClip.w());
        QVector4D clip2(nextClip.x() + clipOffset2.x(), nextClip.y() + clipOffset2.y(), nextClip.z(), nextClip.w());

        // Vertex 3: at B, side was +1 but negated to -1
        float side3 = -1.0f;
        QVector2D ndcOffset3(normalB.x() * halfWidth * pixelToNDC_X * side3,
                             normalB.y() * halfWidth * pixelToNDC_Y * side3);
        QVector2D clipOffset3 = ndcOffset3 * qAbs(nextClip.w());
        QVector4D clip3(nextClip.x() + clipOffset3.x(), nextClip.y() + clipOffset3.y(), nextClip.z(), nextClip.w());

        qDebug(gridnode_log) << "Before ndcMatrix:";
        qDebug(gridnode_log) << "  v0 clip:" << clip0 << "NDC:" << QVector2D(clip0.x()/clip0.w(), clip0.y()/clip0.w());
        qDebug(gridnode_log) << "  v1 clip:" << clip1 << "NDC:" << QVector2D(clip1.x()/clip1.w(), clip1.y()/clip1.w());
        qDebug(gridnode_log) << "  v2 clip:" << clip2 << "NDC:" << QVector2D(clip2.x()/clip2.w(), clip2.y()/clip2.w());
        qDebug(gridnode_log) << "  v3 clip:" << clip3 << "NDC:" << QVector2D(clip3.x()/clip3.w(), clip3.y()/clip3.w());

        // Step 4: Apply ndcMatrix to convert to scene graph's expected clip space
        QVector4D final0 = ndcMatrix * clip0;
        QVector4D final1 = ndcMatrix * clip1;
        QVector4D final2 = ndcMatrix * clip2;
        QVector4D final3 = ndcMatrix * clip3;

        qDebug(gridnode_log) << "After ndcMatrix (final gl_Position):";
        qDebug(gridnode_log) << "  v0:" << final0 << "-> pixels:" << QVector2D(final0.x()/final0.w(), final0.y()/final0.w());
        qDebug(gridnode_log) << "  v1:" << final1 << "-> pixels:" << QVector2D(final1.x()/final1.w(), final1.y()/final1.w());
        qDebug(gridnode_log) << "  v2:" << final2 << "-> pixels:" << QVector2D(final2.x()/final2.w(), final2.y()/final2.w());
        qDebug(gridnode_log) << "  v3:" << final3 << "-> pixels:" << QVector2D(final3.x()/final3.w(), final3.y()/final3.w());
    }
    // ===== END DEBUG =====

    auto *geometry = AOGGeometry::createThickLineGeometry(gridLines);
    //auto *geometry = AOGGeometry::createLinesGeometry(gridLines);
    if (!geometry)
        return;

    m_geomNode = new QSGGeometryNode();
    m_geomNode->setGeometry(geometry);
    m_geomNode->setFlag(QSGNode::OwnsGeometry);

    //auto *material = new AOGFlatColorMaterial();
    auto *material = new ThickLineMaterial();
    material->setColor(gridColor);
    material->setLineWidth(lineWidth);
    QMatrix4x4 mvp = ndcMatrix * pMatrix * mvMatrix;
    //material->setMvpMatrix(mvp);
    material->setModelViewProjectionMatrix(mvp);

    m_geomNode->setMaterial(material);
    m_geomNode->setFlag(QSGNode::OwnsMaterial);

    appendChildNode(m_geomNode);
}
