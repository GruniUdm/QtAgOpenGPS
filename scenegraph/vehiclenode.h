// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Vehicle node - renders the vehicle representation

#ifndef VEHICLENODE_H
#define VEHICLENODE_H

#include <QSGNode>
#include <QSGGeometryNode>
#include <QMatrix4x4>
#include <QColor>

class VehicleNode : public QSGNode
{
public:
    VehicleNode();
    ~VehicleNode() override;

    void update(const QMatrix4x4 &mvp,
                const QColor &vehicleColor,
                double vehicleX, double vehicleY,
                double vehicleHeading);

private:
    void clearChildren();
    QSGGeometryNode *m_geomNode = nullptr;
};

#endif // VEHICLENODE_H
