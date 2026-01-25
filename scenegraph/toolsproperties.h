// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Field surface properties for FieldViewItem - grouped property for QML

#ifndef TOOLSPROPERTIES_H
#define TOOLSPROPERTIES_H

#include <QObject>
#include <QProperty>
#include <QBindable>
#include <QColor>
#include <QQmlListProperty>
#include <QList>

#include "simpleproperty.h"
#include "toolproperties.h"

//Need an enum for type:
//arrow, tractor 2wd, tractor 4wd, combine, dot

class ToolsProperties : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<ToolProperties> tools READ getTools NOTIFY toolsChanged)

public:
    explicit ToolsProperties(QObject *parent = nullptr);

    QQmlListProperty<ToolProperties> getTools();

    // Direct access to the list from C++
    QList<ToolProperties*>& tools() { return m_tools; }
    const QList<ToolProperties*>& tools() const { return m_tools; }

    SIMPLE_BINDABLE_PROPERTY(bool, visible)
signals:
    void toolsChanged();

private:
    // QQmlListProperty callbacks
    static void appendTool(QQmlListProperty<ToolProperties> *list, ToolProperties *tool);
    static qsizetype toolCount(QQmlListProperty<ToolProperties> *list);
    static ToolProperties *toolAt(QQmlListProperty<ToolProperties> *list, qsizetype index);
    static void clearTools(QQmlListProperty<ToolProperties> *list);

    QList<ToolProperties*> m_tools;

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(ToolsProperties, bool, m_visible, true, &ToolsProperties::visibleChanged)

};

#endif // TOOLSPROPERTIES_H
