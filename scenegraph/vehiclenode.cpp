// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Vehicle node implementation

#include "vehiclenode.h"
#include "aogmaterial.h"
#include "aoggeometry.h"

#include <QtMath>
#include <QVector3D>

VehicleNode::VehicleNode()
{
}

VehicleNode::~VehicleNode()
{
}

void VehicleNode::clearChildren()
{
    while (childCount() > 0) {
        QSGNode *child = firstChild();
        removeChildNode(child);
        delete child;
    }
    m_geomNode = nullptr;
}

void VehicleNode::update(const QMatrix4x4 &mvp,
                          const QColor &vehicleColor,
                          double vehicleX, double vehicleY,
                          double vehicleHeading)
{
    // Clear and rebuild each frame (position changes frequently)
    clearChildren();

    // Transform vehicle shape by position and heading
    double heading = qDegreesToRadians(vehicleHeading);
    double cosH = qCos(heading);
    double sinH = qSin(heading);

    // Vehicle arrow shape (pointing in direction of travel)
    double size = 2.0;  // meters

    // Transform local coordinates to world coordinates
    auto transformPoint = [&](double lx, double ly) -> QVector3D {
        double wx = vehicleX + (lx * cosH - ly * sinH);
        double wy = vehicleY + (lx * sinH + ly * cosH);
        return QVector3D(static_cast<float>(wx), static_cast<float>(wy), 0.0f);
    };

    // Arrow pointing forward (positive Y in local coords)
    QVector<QVector3D> vehicleTriangle;
    vehicleTriangle.append(transformPoint(0, size));           // Front
    vehicleTriangle.append(transformPoint(-size/2, -size/2));  // Back left
    vehicleTriangle.append(transformPoint(size/2, -size/2));   // Back right

    auto *geometry = AOGGeometry::createTrianglesGeometry(vehicleTriangle);
    if (!geometry)
        return;

    m_geomNode = new QSGGeometryNode();
    m_geomNode->setGeometry(geometry);
    m_geomNode->setFlag(QSGNode::OwnsGeometry);

    auto *material = new AOGFlatColorMaterial();
    material->setColor(vehicleColor);
    material->setMvpMatrix(mvp);

    m_geomNode->setMaterial(material);
    m_geomNode->setFlag(QSGNode::OwnsMaterial);

    appendChildNode(m_geomNode);
}
