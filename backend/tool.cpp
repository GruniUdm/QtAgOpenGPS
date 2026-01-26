// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#include "tool.h"
#include "sectionproperties.h"

Tool::Tool(QObject *parent)
    : QObject(parent)
    , m_sectionButtons(new SectionButtonsModel(this))
{
    // Report any geometry changes via toolChanged signal
    connect(this, &Tool::trailingChanged, this, &Tool::toolChanged);
    connect(this, &Tool::isTBTTankChanged, this, &Tool::toolChanged);
    connect(this, &Tool::hitchLengthChanged, this, &Tool::toolChanged);
    connect(this, &Tool::pivotToToolLengthChanged, this, &Tool::toolChanged);
    connect(this, &Tool::offsetChanged, this, &Tool::toolChanged);
    connect(this, &Tool::sectionsChanged, this, &Tool::toolChanged);
}

void Tool::setSectionButtonState(int sectionButtonNo, SectionButtonsModel::State new_state)
{
    m_sectionButtons->setState(sectionButtonNo, new_state);
    emit sectionButtonStateChanged(sectionButtonNo, new_state);
}

void Tool::setAllSectionButtonsToState(SectionButtonsModel::State new_state)
{
    m_sectionButtons->setAllState(new_state);
    for (int i = 0; i < m_sectionButtons->count(); i++) {
        emit sectionButtonStateChanged(i, new_state);
    }
}

// ============================================================================
// QQmlListProperty implementation for sections
// ============================================================================

QQmlListProperty<SectionProperties> Tool::getSections()
{
    return QQmlListProperty<SectionProperties>(this, nullptr,
                                               &Tool::appendSection,
                                               &Tool::sectionCount,
                                               &Tool::sectionAt,
                                               &Tool::clearSections);
}

void Tool::appendSection(QQmlListProperty<SectionProperties> *list, SectionProperties *section)
{
    auto *self = static_cast<Tool*>(list->object);
    section->setParent(self);  // Take ownership
    self->connect(section, &SectionProperties::leftPositionChanged, self, &Tool::toolChanged);
    self->connect(section, &SectionProperties::rightPositionChanged, self, &Tool::toolChanged);
    self->m_sections.append(section);
    emit self->sectionsChanged();
    emit self->toolChanged();
}

qsizetype Tool::sectionCount(QQmlListProperty<SectionProperties> *list)
{
    auto *self = static_cast<Tool*>(list->object);
    return self->m_sections.count();
}

SectionProperties *Tool::sectionAt(QQmlListProperty<SectionProperties> *list, qsizetype index)
{
    auto *self = static_cast<Tool*>(list->object);
    return self->m_sections.at(index);
}

void Tool::clearSections(QQmlListProperty<SectionProperties> *list)
{
    auto *self = static_cast<Tool*>(list->object);
    self->m_sections.clear();
    emit self->sectionsChanged();
    emit self->toolChanged();
}
