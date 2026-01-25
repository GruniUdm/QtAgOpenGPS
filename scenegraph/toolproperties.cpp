#include "toolproperties.h"

ToolProperties::ToolProperties(QObject *parent)
    : QObject{parent}
{
    //report any geometry changes
    connect(this, &ToolProperties::trailingChanged, this, &ToolProperties::toolChanged);
    connect(this, &ToolProperties::isTBTTankChanged, this, &ToolProperties::toolChanged);
    connect(this, &ToolProperties::hitchLengthChanged, this, &ToolProperties::toolChanged);
    connect(this, &ToolProperties::pivotToToolLengthChanged, this, &ToolProperties::toolChanged);
    connect(this, &ToolProperties::offsetChanged, this, &ToolProperties::toolChanged);
    connect(this, &ToolProperties::sectionChanged, this, &ToolProperties::toolChanged);
}

QQmlListProperty<SectionProperties> ToolProperties::getSections()
{
    return QQmlListProperty<SectionProperties>(this, nullptr,
                                               &ToolProperties::appendSection,
                                               &ToolProperties::sectionCount,
                                               &ToolProperties::sectionAt,
                                               &ToolProperties::clearSections);
}

void ToolProperties::appendSection(QQmlListProperty<SectionProperties> *list, SectionProperties *section)
{
    auto *self = static_cast<ToolProperties*>(list->object);
    section->setParent(self);  // Take ownership
    self->connect(section, &SectionProperties::leftPositionChanged, self, &ToolProperties::toolChanged);
    self->connect(section, &SectionProperties::rightPositionChanged, self, &ToolProperties::toolChanged);
    self->m_sections.append(section);
    emit self->toolChanged();
}

qsizetype ToolProperties::sectionCount(QQmlListProperty<SectionProperties> *list)
{
    auto *self = static_cast<ToolProperties*>(list->object);
    return self->m_sections.count();
}

SectionProperties *ToolProperties::sectionAt(QQmlListProperty<SectionProperties> *list, qsizetype index)
{
    auto *self = static_cast<ToolProperties*>(list->object);
    return self->m_sections.at(index);
}

void ToolProperties::clearSections(QQmlListProperty<SectionProperties> *list)
{
    auto *self = static_cast<ToolProperties*>(list->object);
    self->m_sections.clear();
    emit self->toolChanged();
}
