// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Boundary node implementation

#include "boundariesnode.h"
#include "boundariesproperties.h"
#include "materials.h"
#include "thicklinematerial.h"
#include "dashedthicklinematerial.h"
#include "aoggeometry.h"

BoundariesNode::BoundariesNode()
{
}

BoundariesNode::~BoundariesNode()
{
}

void BoundariesNode::clearChildren()
{
    while (childCount() > 0) {
        QSGNode *child = firstChild();
        removeChildNode(child);
        delete child;
    }

    m_boundaryNodes.clear();
}

void BoundariesNode::update(const QMatrix4x4 &mv,
                            const QMatrix4x4 &p,
                            const QMatrix4x4 &ndc,
                            const QSize &viewportSize,
                            const BoundariesProperties *properties)
{
    // Clear existing boundary geometry
    clearChildren();

    if (childCount() < 1) {
        // Draw outer boundaries

        // Create geometry for each boundary
        for (const BoundaryProperties *boundary : std::as_const(properties->outer())) {

            if (!boundary->visible() || boundary->points().count() < 3)
                continue;

            // Create line loop geometry
            auto *geometry = AOGGeometry::createDashedThickLineLoopGeometry(boundary->points());
            if (!geometry)
                continue;

            auto *geomNode = new QSGGeometryNode();
            geomNode->setGeometry(geometry);
            geomNode->setFlag(QSGNode::OwnsGeometry);

            // Create material with MVP matrix
            auto *material = new DashedThickLineMaterial();
            material->setColor(properties->colorOuter());
            material->setDashLength(20);
            material->setGapLength(20);
            material->setLineWidth(1);

            geomNode->setMaterial(material);
            geomNode->setFlag(QSGNode::OwnsMaterial);

            appendChildNode(geomNode);
            m_boundaryNodes.append(geomNode);
        }
    }

    //update uniforms

    for (QSGGeometryNode *node:m_boundaryNodes) {
        updateNodeMvp(node, ndc * p * mv, viewportSize);
    }
}

void BoundariesNode::updateNodeMvp(QSGGeometryNode *node,
                                   const QMatrix4x4 mvp,
                                   const QSize &viewportSize)
{
    auto *material = static_cast<AOGMaterial *>(node->material());
    if (material) {
        material->setMvpMatrix(mvp);
        material->setViewportSize(viewportSize);
    }

}
