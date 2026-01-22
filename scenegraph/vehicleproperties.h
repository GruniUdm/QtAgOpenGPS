// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Field surface properties for FieldViewItem - grouped property for QML

#ifndef VEHICLEPROPERTIES_H
#define VEHICLEPROPERTIES_H

#include <QObject>
#include <QProperty>
#include <QBindable>
#include <QColor>

#include "simpleproperty.h"

//Need an enum for type:
//arrow, tractor 2wd, tractor 4wd, combine, dot

class VehicleProperties : public QObject
{
    Q_OBJECT
public:
    explicit VehicleProperties(QObject *parent = nullptr);

    SIMPLE_BINDABLE_PROPERTY(bool, visible)
    SIMPLE_BINDABLE_PROPERTY(QColor, color) //ignored for now
    SIMPLE_BINDABLE_PROPERTY(int, type)

signals:
private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(VehicleProperties, bool, m_visible, true, &VehicleProperties::visibleChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(VehicleProperties, QColor, m_color, QColor(1,0,0,1), &VehicleProperties::colorChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(VehicleProperties, int, m_type, 0, &VehicleProperties::typeChanged)
};

#endif // VEHICLEPROPERTIES_H
