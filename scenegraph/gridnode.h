// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Grid node - renders the world grid lines

#ifndef GRIDNODE_H
#define GRIDNODE_H

#include <QSGNode>
#include <QSGGeometryNode>
#include <QMatrix4x4>
#include <QSize>
#include <QColor>

class GridNode : public QSGNode
{
public:
    GridNode();
    ~GridNode() override;

    void update(const QMatrix4x4 &mvMatrix,
                const QMatrix4x4 &pMatrix,
                const QMatrix4x4 &ndcMatrix,
                const QSize viewportSize,
                const QColor &gridColor,
                double lineWidth,
                double eastingMin, double eastingMax,
                double northingMin, double northingMax,
                double gridSpacing);

private:
    void clearChildren();
    QSGGeometryNode *m_geomNode = nullptr;
};

#endif // GRIDNODE_H
