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
#include <QSize>
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

    // MVP matrix (projection * modelview) - standard clip space
    void setMvpMatrix(const QMatrix4x4 &mvp);
    QMatrix4x4 mvpMatrix() const { return m_mvpMatrix; }

    // NDC matrix - viewport transform applied at the end
    void setNdcMatrix(const QMatrix4x4 &ndc);
    QMatrix4x4 ndcMatrix() const { return m_ndcMatrix; }

    void setViewportSize(const QSize &size);
    QSize viewportSize() const { return m_viewportSize; }

    void setLineWidth(float width);
    float lineWidth() const { return m_lineWidth; }

    // Geometry attribute set for thick lines
    static const QSGGeometry::AttributeSet &attributes();

private:
    QColor m_color = Qt::white;
    QMatrix4x4 m_mvpMatrix;
    QMatrix4x4 m_ndcMatrix;
    QSize m_viewportSize;
    float m_lineWidth = 2.0f;  // Width in screen pixels
};

class ThickLineMaterialShader : public QSGMaterialShader
{
public:
    ThickLineMaterialShader();

    bool updateUniformData(RenderState &state, QSGMaterial *newMaterial, QSGMaterial *oldMaterial) override;
};

#endif // THICKLINEMATERIAL_H
