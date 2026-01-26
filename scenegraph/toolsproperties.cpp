#include "toolsproperties.h"

ToolsProperties::ToolsProperties(QObject *parent)
    : QObject{parent}
{}

QQmlListProperty<Tool> ToolsProperties::getTools()
{
    return QQmlListProperty<Tool>(this, nullptr,
                                            &ToolsProperties::appendTool,
                                            &ToolsProperties::toolCount,
                                            &ToolsProperties::toolAt,
                                            &ToolsProperties::clearTools);
}

void ToolsProperties::appendTool(QQmlListProperty<Tool> *list, Tool *tool)
{
    auto *self = static_cast<ToolsProperties*>(list->object);
    tool->setParent(self);  // Take ownership
    self->connect(tool, &Tool::toolChanged, self, &ToolsProperties::toolsChanged);
    self->m_tools.append(tool);
    emit self->toolsChanged();
}

qsizetype ToolsProperties::toolCount(QQmlListProperty<Tool> *list)
{
    auto *self = static_cast<ToolsProperties*>(list->object);
    return self->m_tools.count();
}

Tool *ToolsProperties::toolAt(QQmlListProperty<Tool> *list, qsizetype index)
{
    auto *self = static_cast<ToolsProperties*>(list->object);
    return self->m_tools.at(index);
}

void ToolsProperties::clearTools(QQmlListProperty<Tool> *list)
{
    auto *self = static_cast<ToolsProperties*>(list->object);
    self->m_tools.clear();
    emit self->toolsChanged();
}
