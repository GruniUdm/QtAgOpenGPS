// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Tool properties for ToolsProperties - individual tool definition

#ifndef TOOLPROPERTIES_H
#define TOOLPROPERTIES_H

#include <QObject>
#include <QProperty>
#include <QBindable>
#include <QColor>
#include <QQmlListProperty>
#include <QList>
#include <QtQml/qqmlregistration.h>

#include "simpleproperty.h"
#include "sectionproperties.h"

class ToolProperties : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QQmlListProperty<SectionProperties> sections READ getSections NOTIFY sectionsChanged)

public:
    explicit ToolProperties(QObject *parent = nullptr);

    QQmlListProperty<SectionProperties> getSections();

    // Direct access to the list from C++
    QList<SectionProperties*>& sections() { return m_sections; }
    const QList<SectionProperties*>& sections() const { return m_sections; }

    SIMPLE_BINDABLE_PROPERTY(bool, trailing)
    SIMPLE_BINDABLE_PROPERTY(bool, isTBTTank)
    SIMPLE_BINDABLE_PROPERTY(double, easting)
    SIMPLE_BINDABLE_PROPERTY(double, northing)
    SIMPLE_BINDABLE_PROPERTY(double, heading)
    SIMPLE_BINDABLE_PROPERTY(double, hitchLength) //negative for behind tractor
    SIMPLE_BINDABLE_PROPERTY(double, pivotToToolLength)
    SIMPLE_BINDABLE_PROPERTY(double, offset)
    SIMPLE_BINDABLE_PROPERTY(QColor, color) //ignored for now

signals:
    void toolChanged();
    void sectionsChanged();

private:
    // QQmlListProperty callbacks
    static void appendSection(QQmlListProperty<SectionProperties> *list, SectionProperties *section);
    static qsizetype sectionCount(QQmlListProperty<SectionProperties> *list);
    static SectionProperties *sectionAt(QQmlListProperty<SectionProperties> *list, qsizetype index);
    static void clearSections(QQmlListProperty<SectionProperties> *list);

    QList<SectionProperties*> m_sections;

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(ToolProperties, bool, m_trailing, true, &ToolProperties::trailingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(ToolProperties, bool, m_isTBTTank, false, &ToolProperties::isTBTTankChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(ToolProperties, double, m_easting, 0.0, &ToolProperties::eastingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(ToolProperties, double, m_northing, 0.0, &ToolProperties::northingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(ToolProperties, double, m_heading, 0.0, &ToolProperties::headingChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(ToolProperties, double, m_hitchLength, 0.0, &ToolProperties::hitchLengthChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(ToolProperties, double, m_pivotToToolLength, 0.0, &ToolProperties::pivotToToolLengthChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(ToolProperties, double, m_offset, 0.0, &ToolProperties::offsetChanged)

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(ToolProperties, QColor, m_color, QColor(1,0,0,1), &ToolProperties::colorChanged)
};

#endif // TOOLPROPERTIES_H
