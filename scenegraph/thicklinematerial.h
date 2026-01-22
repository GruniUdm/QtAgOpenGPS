// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Thick line material - renders lines with constant screen-pixel width

#ifndef THICKLINEMATERIAL_H
#define THICKLINEMATERIAL_H

#include <QSGMaterial>
#include <QSGMaterialShader>
#include <QMatrix4x4>
#include <QColor>
#include <QSGGeometry>

#include "aoggeometry.h"  // For ThickLineVertex struct

class ThickLineMaterial : public QSGMaterial
{
public:
    ThickLineMaterial();

    QSGMaterialType *type() const override;
    QSGMaterialShader *createShader(QSGRendererInterface::RenderMode renderMode) const override;
    int compare(const QSGMaterial *other) const override;

    void setColor(const QColor &color);
    QColor color() const { return m_color; }

    // Separate matrices for proper thick line calculation
    void setModelViewProjectionMatrix(const QMatrix4x4 &mvp);
    QMatrix4x4 modelViewProjectionMatrix() const { return m_mvpMatrix; }

    void setLineWidth(float width);
    float lineWidth() const { return m_lineWidth; }

    // Geometry attribute set for thick lines
    static const QSGGeometry::AttributeSet &attributes();

private:
    QColor m_color = Qt::white;
    QMatrix4x4 m_mvpMatrix;
    float m_lineWidth = 2.0f;  // Width in screen pixels
    QSize m_viewportSize;
};

class ThickLineMaterialShader : public QSGMaterialShader
{
public:
    ThickLineMaterialShader();

    bool updateUniformData(RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial) override;
};

#endif // THICKLINEMATERIAL_H
