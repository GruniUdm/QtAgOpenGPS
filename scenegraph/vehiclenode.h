// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Vehicle node - renders the vehicle representation

#ifndef VEHICLENODE_H
#define VEHICLENODE_H

#include <QSGNode>
#include <QSGGeometryNode>
#include <QSGTexture>
#include <QMatrix4x4>
#include <QColor>
#include "vehicleproperties.h"

class QQuickWindow;

class VehicleNode : public QSGNode
{
public:
    VehicleNode();
    ~VehicleNode() override;

    void update(const QMatrix4x4 &mv,
                const QMatrix4x4 &p,
                const QMatrix4x4 &ncd,
                const QColor &vehicleColor,
                QSGTexture *texture,
                double vehicleX, double vehicleY,
                double vehicleHeading,
                const VehicleProperties *properties);

private:
    void clearChildren();

    QSGGeometryNode *m_geomNode = nullptr;
    QSGTexture *m_texture = nullptr;
};

#endif // VEHICLENODE_H
