// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Section properties for ToolProperties - individual section definition

#ifndef SECTIONPROPERTIES_H
#define SECTIONPROPERTIES_H

#include <QObject>
#include <QProperty>
#include <QBindable>
#include <QtQml/qqmlregistration.h>

#include "simpleproperty.h"

class SectionProperties : public QObject
{
    Q_OBJECT
    QML_ELEMENT
public:
    explicit SectionProperties(QObject *parent = nullptr);

    SIMPLE_BINDABLE_PROPERTY(double, leftPosition)
    SIMPLE_BINDABLE_PROPERTY(double, rightPosition)

signals:

private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SectionProperties, double, m_leftPosition, 0.0, &SectionProperties::leftPositionChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SectionProperties, double, m_rightPosition, 0.0, &SectionProperties::rightPositionChanged)
};

#endif // SECTIONPROPERTIES_H
