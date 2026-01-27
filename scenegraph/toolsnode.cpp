// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Tools node implementation

#include "toolsnode.h"
#include "toolsproperties.h"
#include "sectionproperties.h"
#include "texturefactory.h"
#include "aogmaterial.h"
#include "aoggeometry.h"
#include "materials.h"
#include "thicklinematerial.h"
#include "glm.h"

ToolsNode::ToolsNode()
{
}

ToolsNode::~ToolsNode()
{
}

void ToolsNode::clearChildren()
{
    while (childCount() > 0) {
        QSGNode *child = firstChild();
        removeChildNode(child);
        delete child;
    }
    m_toolNodes.clear();
}

void ToolsNode::update(const QMatrix4x4 &mv,
                       const QMatrix4x4 &p,
                       const QMatrix4x4 &ndc,
                       const QSize &viewportSize,
                       TextureFactory *textureFactory,
                       ToolsProperties *properties,
                       double camSetDistance)
{
    Q_UNUSED(textureFactory)

    if (!properties || !properties->visible())
        return;

    //mv is currently at tractor pivot axle coordinates, facing north.

    if (childCount() < 1) {
        //set up geometry

        for (Tool *tool: properties->tools()) {
            QList<QSGGeometryNode*> toolNodes;

            QMatrix4x4 toolMv = mv;
            //0,0 is the center of the tool.  Hitch goes forward from there
            //towards the tractor

            if (tool->isTBTTank()) {
                QVector<QVector3D> hitches;

                hitches.append( { -0.57, 0, 0 } );
                hitches.append( {0, -tool->hitchLength(), 0} );
                hitches.append( { 0.57, 0, 0 } );

                //shadow under the line
                auto *geometry = AOGGeometry::createThickLineLoopGeometry(hitches);
                auto geomNode = new QSGGeometryNode();
                geomNode->setGeometry(geometry);
                geomNode->setFlag(QSGNode::OwnsGeometry);

                auto *material = new ThickLineMaterial();
                material->setColor(QColor::fromRgbF(0,0,0,1));
                material->setLineWidth(3.0f);
                geomNode->setMaterial(material);
                geomNode->setFlag(QSGNode::OwnsMaterial);

                appendChildNode(geomNode);
                toolNodes.append(geomNode);

                //the line itself
                geometry = AOGGeometry::createThickLineLoopGeometry(hitches);
                geomNode = new QSGGeometryNode();
                geomNode->setGeometry(geometry);
                geomNode->setFlag(QSGNode::OwnsGeometry);

                material = new ThickLineMaterial();
                material->setColor(QColor::fromRgbF(0.765f, 0.76f, 0.32f));
                material->setLineWidth(1.0f);
                geomNode->setMaterial(material);
                geomNode->setFlag(QSGNode::OwnsMaterial);

                appendChildNode(geomNode);
                toolNodes.append(geomNode);

            } else {
                if (tool->trailing()) {
                    float offset = tool->offset();
                    QVector<QVector3D> hitches;

                    hitches.append( { -0.4f + offset, 0.0f, 0.0f } );
                    hitches.append( {0.0f, -tool->hitchLength(), 0.0f} );
                    hitches.append( { 0.4f + offset, 0.0f, 0.0f } );

                    //shadow under line
                    auto *geometry = AOGGeometry::createThickLineLoopGeometry(hitches);
                    auto geomNode = new QSGGeometryNode();
                    geomNode->setGeometry(geometry);
                    geomNode->setFlag(QSGNode::OwnsGeometry);

                    auto *material = new ThickLineMaterial();
                    material->setColor(QColor::fromRgbF(0,0,0,1));
                    material->setLineWidth(3.0f);

                    geomNode->setMaterial(material);
                    geomNode->setFlag(QSGNode::OwnsMaterial);

                    appendChildNode(geomNode);
                    toolNodes.append(geomNode);

                    //the line itself

                    geometry = AOGGeometry::createThickLineLoopGeometry(hitches);
                    geomNode = new QSGGeometryNode();
                    geomNode->setGeometry(geometry);
                    geomNode->setFlag(QSGNode::OwnsGeometry);

                    material = new ThickLineMaterial();
                    material->setColor(QColor::fromRgbF(0.7, 0.4f, 0.2f));
                    material->setLineWidth(1.0f);

                    geomNode->setMaterial(material);
                    geomNode->setFlag(QSGNode::OwnsMaterial);

                    appendChildNode(geomNode);
                    toolNodes.append(geomNode);

                    //tool wheels
                    geometry = new QSGGeometry(AOGGeometry::texturedVertexAttributes(), 4);
                    geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);

                    geomNode = new QSGGeometryNode();
                    geomNode->setGeometry(geometry);
                    geomNode->setFlag(QSGNode::OwnsGeometry);

                    auto *texMaterial = new AOGTextureMaterial();

                    TexturedVertex *data = static_cast<TexturedVertex *>(geometry->vertexData());

                    float hitch = static_cast<float>(tool->hitchLength());
                    float off = static_cast<float>(offset);
                    data[0] = { -1.5f + off, - 1.0f, 0.0f, 0.0f, 1.0f };  // Back-left
                    data[1] = {  1.5f + off, - 1.0f, 0.0f, 1.0f, 1.0f };  // Back-right
                    data[2] = { -1.5f + off, 1.0f, 0.0f, 0.0f, 0.0f };  // Front-left
                    data[3] = {  1.5f + off,  1.0f, 0.0f, 1.0f, 0.0f };  // Front-right

                    texMaterial->setUseColor(true);
                    texMaterial->setColor(QColor::fromRgbF(1,1,1,0.75));
                    QSGTexture *texture = textureFactory->texture(TextureId::ToolWheels);
                    if (texture) {
                        texMaterial->setTexture(texture);
                    }
                    geomNode->setMaterial(texMaterial);
                    geomNode->setFlag(QSGNode::OwnsMaterial);
                    appendChildNode(geomNode);
                    toolNodes.append(geomNode);
                }

                //now do sections
                float hite = camSetDistance / -150;
                if (hite > 12) hite = 12;
                if (hite < 1) hite = 1;

                float trailingTool = tool->pivotToToolLength();

                for (auto &section: tool->sections()) {



                }
                //if zones, mark them with lines

                //trams?

                //TODO lookahead lines, maybe do elsewhere

            }
            m_toolNodes.append(toolNodes);
        }
    }


    //iterate through tools, setting matrices for each node
    int i=0;
    for (auto &nodeList: m_toolNodes) {
        auto &tool = properties->tools()[i];
        QMatrix4x4 toolMv = mv;

        toolMv.translate(tool->easting(), tool->northing(), 0);
        toolMv.rotate(-tool->heading(),0,0,1);

        int n=0;
        for (auto &node : nodeList) {
            //first three nodes (if we have three) are hitches and axle texture
            if (n < 3) {
                updateNodeMvp(node, ndc * p * toolMv, viewportSize);
            } else {

                //it'a section and needs the color set as well

            }

            n++;
        }

        i++;
    }
}

void ToolsNode::updateNodeMvp(QSGGeometryNode *node,
                                const QMatrix4x4 mvp,
                                const QSize &viewportSize)
{
    auto *material = static_cast<AOGMaterial *>(node->material());
    if (material) {
        material->setMvpMatrix(mvp);
        material->setViewportSize(viewportSize);
    }
}

void ToolsNode::updateNodeColor(QSGGeometryNode *node,
                                const QColor &color)
{
    auto *material = static_cast<AOGFlatColorMaterial *>(node->material());
    if (material) {
        material->setColor(color);
    }
}
