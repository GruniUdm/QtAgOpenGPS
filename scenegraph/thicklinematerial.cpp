// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Thick line material implementation

#include "thicklinematerial.h"
#include <QFile>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(thicklinematerial_log, "thicklinematerial.qtagopengps")

// ============================================================================
// ThickLineMaterial
// ============================================================================

ThickLineMaterial::ThickLineMaterial()
{
    setFlag(Blending);  // Support transparent lines
}

QSGMaterialType *ThickLineMaterial::type() const
{
    static QSGMaterialType type;
    return &type;
}

QSGMaterialShader *ThickLineMaterial::createShader(QSGRendererInterface::RenderMode) const
{
    return new ThickLineMaterialShader();
}

int ThickLineMaterial::compare(const QSGMaterial *other) const
{
    const auto *o = static_cast<const ThickLineMaterial *>(other);

    if (m_color != o->m_color)
        return m_color.rgba() < o->m_color.rgba() ? -1 : 1;

    if (m_lineWidth != o->m_lineWidth)
        return m_lineWidth < o->m_lineWidth ? -1 : 1;

    // Matrices differ per frame, always return non-equal to force update
    if (m_mvpMatrix != o->m_mvpMatrix)
        return -1;

    return 0;
}

void ThickLineMaterial::setColor(const QColor &color)
{
    m_color = color;
}

void ThickLineMaterial::setModelViewProjectionMatrix(const QMatrix4x4 &mvp)
{
    m_mvpMatrix = mvp;
}

void ThickLineMaterial::setLineWidth(float width)
{
    m_lineWidth = width;
}

const QSGGeometry::AttributeSet &ThickLineMaterial::attributes()
{
    // Use the attribute set defined in AOGGeometry
    return AOGGeometry::thickLineAttributes();
}

// ============================================================================
// ThickLineMaterialShader
// ============================================================================

ThickLineMaterialShader::ThickLineMaterialShader()
{
    setShaderFileName(VertexStage, QLatin1String(":/AOG/shaders/thickline.vert.qsb"));
    setShaderFileName(FragmentStage, QLatin1String(":/AOG/shaders/thickline.frag.qsb"));
}

bool ThickLineMaterialShader::updateUniformData(RenderState &state,
                                                 QSGMaterial *newMaterial,
                                                 QSGMaterial *oldMaterial)
{
    Q_UNUSED(oldMaterial);

    bool changed = false;
    QByteArray *buf = state.uniformData();

    auto *material = static_cast<ThickLineMaterial *>(newMaterial);

    // Uniform buffer layout (must match shader, std140):
    // mat4 combined   @ offset 0 (64 bytes) - state.combinedMatrix() for final transform
    // vec4 color      @ offset 64 (16 bytes)
    // float lineWidth @ offset 80 (4 bytes)
    // (padding to 224 bytes for std140 alignment)

    // ModelView matrix
    //const QMatrix4x4 &mvp = material->modelViewMatrix();
    //memcpy(buf->data(), mvp.constData(), 64);

    const QMatrix4x4 &mvp = material->modelViewProjectionMatrix();

    memcpy(buf->data(), mvp.constData(), 64);

    // Color (as vec4)
    const QColor &c = material->color();
    float color[4] = {
        static_cast<float>(c.redF()),
        static_cast<float>(c.greenF()),
        static_cast<float>(c.blueF()),
        static_cast<float>(c.alphaF())
    };
    memcpy(buf->data() + 64, color, 16);

    // Line width
    float lineWidth = material->lineWidth();
    memcpy(buf->data() + 80, &lineWidth, 4);

    changed = true;
    return changed;
}
