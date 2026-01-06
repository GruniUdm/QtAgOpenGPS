// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QtQml/qqml.h>
#include <QObjectBindableProperty>
#include "sectionsmodel.h"
#include "simpleproperty.h"

class Tool : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(SectionsModel* sections READ sections CONSTANT)

public:
    explicit Tool(QObject *parent = nullptr);

    SectionsModel* sections() const { return m_sections; }

    SIMPLE_BINDABLE_PROPERTY(double, easting)
    SIMPLE_BINDABLE_PROPERTY(double, northing)
    SIMPLE_BINDABLE_PROPERTY(double, latitude)
    SIMPLE_BINDABLE_PROPERTY(double, longitude)
    SIMPLE_BINDABLE_PROPERTY(double, heading)

private:
    SectionsModel *m_sections;

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Tool, double, m_easting, 0, &Tool::eastingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Tool, double, m_northing, 0, &Tool::northingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Tool, double, m_latitude, 0, &Tool::latitudeChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Tool, double, m_longitude, 0, &Tool::longitudeChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Tool, double, m_heading, 0, &Tool::headingChanged)
};

#endif // TOOL_H
