// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Grid properties for FieldViewItem - grouped property for QML

#ifndef GRIDPROPERTIES_H
#define GRIDPROPERTIES_H

#include <QObject>
#include <QProperty>
#include <QBindable>
#include <QColor>

#include "simpleproperty.h"

class GridProperties : public QObject
{
    Q_OBJECT
public:
    explicit GridProperties(QObject *parent = nullptr);

    SIMPLE_BINDABLE_PROPERTY(double, size)
    SIMPLE_BINDABLE_PROPERTY(QColor, color)

signals:

private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(GridProperties, double, m_size, 6000, &GridProperties::sizeChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(GridProperties, QColor, m_color, QColor(0,0,0), &GridProperties::colorChanged)
};

#endif // GRIDPROPERTIES_H
