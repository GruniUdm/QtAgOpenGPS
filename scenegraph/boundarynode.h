// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Boundary node - renders field boundaries

#ifndef BOUNDARYNODE_H
#define BOUNDARYNODE_H

#include <QSGNode>
#include <QSGGeometryNode>
#include <QMatrix4x4>
#include <QColor>
#include <QVector>
#include <QVector3D>

class BoundaryNode : public QSGNode
{
public:
    BoundaryNode();
    ~BoundaryNode() override;

    void update(const QMatrix4x4 &mvp,
                const QColor &boundaryColor,
                const QVector<QVector<QVector3D>> &boundaries);

private:
    void clearChildren();
};

#endif // BOUNDARYNODE_H
