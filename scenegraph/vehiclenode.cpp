// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Vehicle node implementation

#include "vehiclenode.h"
#include "aogmaterial.h"
#include "aoggeometry.h"
#include "materials.h"
#include "thicklinematerial.h"
#include "glm.h"

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

void VehicleNode::update(const QMatrix4x4 &mv,
                         const QMatrix4x4 &p,
                         const QMatrix4x4 &ncd,
                         const QColor &vehicleColor,
                         const QSize &viewportSize,
                         QSGTexture *texture,
                         double vehicleX, double vehicleY,
                         double vehicleHeading,
                         const VehicleProperties *properties)
{
    QMatrix4x4 vehicleMv = mv;
    vehicleMv.translate(vehicleX, vehicleY, 0);
    vehicleMv.rotate(glm::toDegrees(-vehicleHeading),0.0,0.0,1.0);

    if (childCount() < 1 /*|| we need to change something */) {
        // construct geometry. either this is the initial display, or
        // something in the vehicle's geometry changed and we need to
        // update it.

        clearChildren();
        const float trackWidth = properties->trackWidth();   // 2m total width
        const float wheelBase = properties->wheelBase();


        QVector<QVector3D> hitches;

        //draw front hitch
        if (properties->frontHitchLength()) {
            //currently we don't draw a front hitch
        }

        if (properties->drawbarLength()) {

            hitches.append({ 0, static_cast<float>(-properties->drawbarLength()), 0});
            hitches.append({ 0, 0, 0});
        }

        if (properties->threePtLength()) {
            hitches.append( {-0.35, -properties->threePtLength(),0} );
            hitches.append( {-0.35, 0 ,0} );
            hitches.append( {0.35, -properties->threePtLength(),0} );
            hitches.append( {0.35, 0 ,0} );
        }

        if (hitches.count()) {
            //shadow
            auto *geometry = AOGGeometry::createThickLinesGeometry(hitches);
            if (geometry) {
                m_geomNode = new QSGGeometryNode();
                m_geomNode->setGeometry(geometry);
                m_geomNode->setFlag(QSGNode::OwnsGeometry);

                auto *material = new ThickLineMaterial();
                material->setColor(QColor::fromRgbF(0,0,0,1));
                material->setLineWidth(3.0f);
                material->setMvpMatrix(ncd * p * vehicleMv);
                material->setViewportSize(viewportSize);

                m_geomNode->setMaterial(material);
                m_geomNode->setFlag(QSGNode::OwnsMaterial);

                appendChildNode(m_geomNode);
            }

            //lines themselves
            geometry = AOGGeometry::createThickLinesGeometry(hitches);
            if (geometry) {
                m_geomNode = new QSGGeometryNode();
                m_geomNode->setGeometry(geometry);
                m_geomNode->setFlag(QSGNode::OwnsGeometry);

                //auto *material = new AOGFlatColorMaterial();
                auto *material = new ThickLineMaterial();
                material->setColor(QColor::fromRgbF(1.237f, 0.037f, 0.0397f));
                material->setLineWidth(1.0f);
                material->setMvpMatrix(ncd * p * vehicleMv);
                material->setViewportSize(viewportSize);

                m_geomNode->setMaterial(material);
                m_geomNode->setFlag(QSGNode::OwnsMaterial);

                appendChildNode(m_geomNode);
            }
        }

        if (texture && properties->type() == 1) {
            // Create textured quad for vehicle using local coordinates
            // Local coords: centered at origin, front is +Y, right is +X
            // Texture: bottom of image is back of vehicle, top is front

            auto *geometry = new QSGGeometry(AOGGeometry::texturedVertexAttributes(), 4);
            geometry->setDrawingMode(QSGGeometry::DrawTriangleStrip);

            TexturedVertex *data = static_cast<TexturedVertex *>(geometry->vertexData());

            // Triangle strip order: back-left, back-right, front-left, front-right
            // Back-left (texture bottom-left: u=0, v=1)
            data[0].x = -properties->trackWidth();
            data[0].y = -properties->wheelBase() * 0.5;
            data[0].z = 0.0f;
            data[0].u = 0.0f;
            data[0].v = 1.0f;

            // Back-right (texture bottom-right: u=1, v=1)
            data[1].x = properties->trackWidth();
            data[1].y = -properties->wheelBase() * 0.5;
            data[1].z = 0.0f;
            data[1].u = 1.0f;
            data[1].v = 1.0f;

            // Front-left (texture top-left: u=0, v=0)
            data[2].x = -properties->trackWidth();
            data[2].y = properties->wheelBase()*1.5;
            data[2].z = 0.0f;
            data[2].u = 0.0f;
            data[2].v = 0.0f;

            // Front-right (texture top-right: u=1, v=0)
            data[3].x = properties->trackWidth();
            data[3].y = properties->wheelBase() * 1.5;
            data[3].z = 0.0f;
            data[3].u = 1.0f;
            data[3].v = 0.0f;

            m_geomNode = new QSGGeometryNode();
            m_geomNode->setGeometry(geometry);
            m_geomNode->setFlag(QSGNode::OwnsGeometry);

            auto *material = new AOGTextureMaterial();
            material->setTexture(texture);
            material->setMvpMatrix(ncd * p * vehicleMv);
            // Don't use color tinting - show texture as-is
            material->setUseColor(false);

            m_geomNode->setMaterial(material);
            m_geomNode->setFlag(QSGNode::OwnsMaterial);

            appendChildNode(m_geomNode);
        } else {
            //just draw a triangle


            // Fallback to colored triangle using local coordinates
            QVector<QVector3D> vehicleTriangle;
            vehicleTriangle.append(QVector3D(0.0f, wheelBase * 2, 0.0f));       // Front
            vehicleTriangle.append(QVector3D(-wheelBase, -wheelBase, 0.0f));    // Back left
            vehicleTriangle.append(QVector3D(wheelBase, -wheelBase, 0.0f));     // Back right

            auto *geometry = AOGGeometry::createTrianglesGeometry(vehicleTriangle);
            if (!geometry)
                return;

            m_geomNode = new QSGGeometryNode();
            m_geomNode->setGeometry(geometry);
            m_geomNode->setFlag(QSGNode::OwnsGeometry);

            auto *material = new AOGFlatColorMaterial();
            material->setColor(vehicleColor);
            material->setMvpMatrix(ncd * p * vehicleMv);

            m_geomNode->setMaterial(material);
            m_geomNode->setFlag(QSGNode::OwnsMaterial);

            appendChildNode(m_geomNode);
        }
    } else {
        //Vehicle hasn't changed, so we just set up the new matrices
        QSGNode *child = firstChild();
        QSGGeometryNode *gchild;
        int i = 0;

        while(child) {
            // go through the geometry nodes in the order they were created
            // above and set the full mvp Matrix on each one. Using our
            // AOGMaterial base class to polymorphically set the matrix
            // on every derived class.  Oh for Python's duck typing!

            gchild = static_cast<QSGGeometryNode *>(child);
            auto *material = static_cast<AOGMaterial *>(gchild->material());
            material->setMvpMatrix(ncd * p * vehicleMv);
            material->setViewportSize(viewportSize);

            child = child->nextSibling();
        }
    }
}
