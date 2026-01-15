// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QtQml/qqml.h>
#include <QObjectBindableProperty>
#include "sectionbuttonsmodel.h"
#include "simpleproperty.h"

class Tool : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(SectionButtonsModel* sectionButtonsModel READ sectionButtonsModel CONSTANT)

public:
    explicit Tool(QObject *parent = nullptr);

    SectionButtonsModel* sectionButtonsModel() const { return m_sectionButtons; }

    SIMPLE_BINDABLE_PROPERTY(double, easting)
    SIMPLE_BINDABLE_PROPERTY(double, northing)
    SIMPLE_BINDABLE_PROPERTY(double, latitude)
    SIMPLE_BINDABLE_PROPERTY(double, longitude)
    SIMPLE_BINDABLE_PROPERTY(double, heading)

    Q_INVOKABLE void setSectionButtonState(int sectionButtonNo, SectionButtonsModel::State new_state);
    Q_INVOKABLE void setAllSectionButtonsToState(SectionButtonsModel::State new_state);

signals:
    void sectionButtonStateChanged(int sectionButtonNo, SectionButtonsModel::State new_state);

private:
    SectionButtonsModel *m_sectionButtons;

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Tool, double, m_easting, 0, &Tool::eastingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Tool, double, m_northing, 0, &Tool::northingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Tool, double, m_latitude, 0, &Tool::latitudeChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Tool, double, m_longitude, 0, &Tool::longitudeChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(Tool, double, m_heading, 0, &Tool::headingChanged)
};

#endif // TOOL_H
