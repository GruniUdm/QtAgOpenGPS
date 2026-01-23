// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Boundary node implementation

#include "boundarynode.h"
#include "materials.h"
#include "aoggeometry.h"

BoundaryNode::BoundaryNode()
{
}

BoundaryNode::~BoundaryNode()
{
}

void BoundaryNode::clearChildren()
{
    while (childCount() > 0) {
        QSGNode *child = firstChild();
        removeChildNode(child);
        delete child;
    }
}

void BoundaryNode::update(const QMatrix4x4 &mvp,
                           const QColor &boundaryColor,
                           const QVector<QVector<QVector3D>> &boundaries)
{
    // Clear existing boundary geometry
    clearChildren();

    // Create geometry for each boundary
    for (const auto &boundary : boundaries) {
        if (boundary.size() < 3)
            continue;

        // Create line loop geometry
        auto *geometry = AOGGeometry::createLineLoopGeometry(boundary);
        if (!geometry)
            continue;

        auto *geomNode = new QSGGeometryNode();
        geomNode->setGeometry(geometry);
        geomNode->setFlag(QSGNode::OwnsGeometry);

        // Create material with MVP matrix
        auto *material = new AOGFlatColorMaterial();
        material->setColor(boundaryColor);
        material->setMvpMatrix(mvp);

        geomNode->setMaterial(material);
        geomNode->setFlag(QSGNode::OwnsMaterial);

        appendChildNode(geomNode);
    }
}
