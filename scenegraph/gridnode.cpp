// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Grid node implementation

#include "gridnode.h"
#include "aogmaterial.h"
#include "aoggeometry.h"

#include <QtMath>
#include <QVector3D>

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

void GridNode::update(const QMatrix4x4 &mvp,
                       const QColor &gridColor,
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
    for (double num = roundMidAwayFromZero(eastingMin / gridSpacing) * gridSpacing;
         num < eastingMax; num += gridSpacing) {
        if (num < eastingMin) continue;
        gridLines.append(QVector3D(static_cast<float>(num), static_cast<float>(northingMax), 0.1f));
        gridLines.append(QVector3D(static_cast<float>(num), static_cast<float>(northingMin), 0.1f));
    }

    // Create horizontal lines (constant northing, varying easting)
    for (double num2 = roundMidAwayFromZero(northingMin / gridSpacing) * gridSpacing;
         num2 < northingMax; num2 += gridSpacing) {
        if (num2 < northingMin) continue;
        gridLines.append(QVector3D(static_cast<float>(eastingMax), static_cast<float>(num2), 0.1f));
        gridLines.append(QVector3D(static_cast<float>(eastingMin), static_cast<float>(num2), 0.1f));
    }

    if (gridLines.isEmpty())
        return;

    auto *geometry = AOGGeometry::createLinesGeometry(gridLines);
    if (!geometry)
        return;

    m_geomNode = new QSGGeometryNode();
    m_geomNode->setGeometry(geometry);
    m_geomNode->setFlag(QSGNode::OwnsGeometry);

    auto *material = new AOGFlatColorMaterial();
    material->setColor(gridColor);
    material->setMvpMatrix(mvp);

    m_geomNode->setMaterial(material);
    m_geomNode->setFlag(QSGNode::OwnsMaterial);

    appendChildNode(m_geomNode);
}
