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
                      const QColor &gridColor, double lineWidth,
                      double eastingMin, double eastingMax,
                      double northingMin, double northingMax,
                      double gridSpacing)
{
    // Clear existing geometry and rebuild
    clearChildren();

    // MVP matrix for clipping calculations
    QMatrix4x4 mvp = pMatrix * mvMatrix;

    // Minimum w value - points with w below this are considered "behind" camera
    // Using a small positive value provides a margin from the near plane
    const float minW = 0.1f;

    // Helper: Clip a line segment to ensure both endpoints are in front of camera
    // Returns true if any portion of the line is visible, with clipped endpoints in outA/outB
    auto clipLineToNearPlane = [&mvp, minW](const QVector3D &a, const QVector3D &b,
                                             QVector3D &outA, QVector3D &outB) -> bool {
        QVector4D clipA = mvp * QVector4D(a, 1.0f);
        QVector4D clipB = mvp * QVector4D(b, 1.0f);

        bool aInFront = clipA.w() >= minW;
        bool bInFront = clipB.w() >= minW;

        if (aInFront && bInFront) {
            // Both in front - keep as-is
            outA = a;
            outB = b;
            return true;
        }

        if (!aInFront && !bInFront) {
            // Both behind camera - skip entirely
            return false;
        }

        // One in front, one behind - interpolate to find near plane crossing
        // We want to find t where: lerp(clipA.w, clipB.w, t) = minW
        // clipA.w + t * (clipB.w - clipA.w) = minW
        // t = (minW - clipA.w) / (clipB.w - clipA.w)
        float t = (minW - clipA.w()) / (clipB.w() - clipA.w());

        // Interpolate in world space
        QVector3D intersection = a + t * (b - a);

        if (aInFront) {
            outA = a;
            outB = intersection;
        } else {
            outA = intersection;
            outB = b;
        }
        return true;
    };

    // Create grid lines with near-plane clipping
    QVector<QVector3D> gridLines;

    // Helper function to round mid away from zero (like glm::roundMidAwayFromZero)
    auto roundMidAwayFromZero = [](double val) -> double {
        return (val >= 0) ? qFloor(val + 0.5) : qCeil(val - 0.5);
    };

    // Create vertical lines (constant easting, varying northing)
    for (double num = roundMidAwayFromZero(eastingMin / gridSpacing) * gridSpacing;
         num < eastingMax; num += gridSpacing) {
        if (num < eastingMin) continue;

        QVector3D a(static_cast<float>(num), static_cast<float>(northingMax), 0.1f);
        QVector3D b(static_cast<float>(num), static_cast<float>(northingMin), 0.1f);
        QVector3D clippedA, clippedB;

        if (clipLineToNearPlane(a, b, clippedA, clippedB)) {
            gridLines.append(clippedA);
            gridLines.append(clippedB);
        }
    }

    // Create horizontal lines (constant northing, varying easting)
    for (double num2 = roundMidAwayFromZero(northingMin / gridSpacing) * gridSpacing;
         num2 < northingMax; num2 += gridSpacing) {
        if (num2 < northingMin) continue;

        QVector3D a(static_cast<float>(eastingMax), static_cast<float>(num2), 0.1f);
        QVector3D b(static_cast<float>(eastingMin), static_cast<float>(num2), 0.1f);
        QVector3D clippedA, clippedB;

        if (clipLineToNearPlane(a, b, clippedA, clippedB)) {
            gridLines.append(clippedA);
            gridLines.append(clippedB);
        }
    }

    if (gridLines.isEmpty())
        return;

    if (lineWidth <= 0.2) {
        //use normal lines
        auto *geometry = AOGGeometry::createLinesGeometry(gridLines);
        if (!geometry)
            return;

        m_geomNode = new QSGGeometryNode();
        m_geomNode->setGeometry(geometry);
        m_geomNode->setFlag(QSGNode::OwnsGeometry);

        auto *material = new AOGFlatColorMaterial();
        material->setColor(gridColor);
        material->setMvpMatrix(ndcMatrix * pMatrix * mvMatrix);

        m_geomNode->setMaterial(material);
        m_geomNode->setFlag(QSGNode::OwnsMaterial);

    } else {
        // Grid lines are disconnected segments (pairs of points), not a connected polyline
        auto *geometry = AOGGeometry::createThickLinesGeometry(gridLines);
        //auto *geometry = AOGGeometry::createLinesGeometry2(gridLines);
        if (!geometry)
            return;

        m_geomNode = new QSGGeometryNode();
        m_geomNode->setGeometry(geometry);
        m_geomNode->setFlag(QSGNode::OwnsGeometry);

        //auto *material = new AOGFlatColorMaterial();
        auto *material = new ThickLineMaterial();
        material->setColor(gridColor);
        material->setLineWidth(lineWidth);
        material->setMvpMatrix(pMatrix * mvMatrix);  // Standard MVP (NDC is [-1,1])
        //material->setMvpMatrix(ndcMatrix * pMatrix * mvMatrix);
        material->setNdcMatrix(ndcMatrix);            // Viewport transform applied at end
        material->setViewportSize(viewportSize);

        m_geomNode->setMaterial(material);
        m_geomNode->setFlag(QSGNode::OwnsMaterial);
    }

    appendChildNode(m_geomNode);
}
