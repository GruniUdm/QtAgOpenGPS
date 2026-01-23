// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Custom QSGMaterial classes for scene graph rendering with 3D projection

#ifndef AOGMATERIAL_H
#define AOGMATERIAL_H

#include <QSGMaterial>
#include <QSGMaterialShader>
#include <QMatrix4x4>
#include <QColor>
#include <QSGTexture>

// ============================================================================b
// AOGMaterial - incomplete base class for all our materials
// Contains a common setFullMatrix method that all our nodes will
// use to make updating the matrix easier.
// ============================================================================

class AOGMaterial : public QSGMaterial
{
public:
    AOGMaterial()=default;

    void setMvpMatrix(const QMatrix4x4 &mvpMatrix);
    QMatrix4x4 mvpMatrix() const { return m_mvpMatrix; }

protected:
    QMatrix4x4 m_mvpMatrix;
};

#endif // AOGMATERIAL_H
