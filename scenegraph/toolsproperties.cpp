#include "toolsproperties.h"

ToolsProperties::ToolsProperties(QObject *parent)
    : QObject{parent}
{}

QQmlListProperty<ToolProperties> ToolsProperties::getTools()
{
    return QQmlListProperty<ToolProperties>(this, nullptr,
                                            &ToolsProperties::appendTool,
                                            &ToolsProperties::toolCount,
                                            &ToolsProperties::toolAt,
                                            &ToolsProperties::clearTools);
}

void ToolsProperties::appendTool(QQmlListProperty<ToolProperties> *list, ToolProperties *tool)
{
    auto *self = static_cast<ToolsProperties*>(list->object);
    tool->setParent(self);  // Take ownership
    self->connect(tool, &ToolProperties::toolChanged, self, &ToolsProperties::toolsChanged);
    self->m_tools.append(tool);
    emit self->toolsChanged();
}

qsizetype ToolsProperties::toolCount(QQmlListProperty<ToolProperties> *list)
{
    auto *self = static_cast<ToolsProperties*>(list->object);
    return self->m_tools.count();
}

ToolProperties *ToolsProperties::toolAt(QQmlListProperty<ToolProperties> *list, qsizetype index)
{
    auto *self = static_cast<ToolsProperties*>(list->object);
    return self->m_tools.at(index);
}

void ToolsProperties::clearTools(QQmlListProperty<ToolProperties> *list)
{
    auto *self = static_cast<ToolsProperties*>(list->object);
    self->m_tools.clear();
    emit self->toolsChanged();
}
