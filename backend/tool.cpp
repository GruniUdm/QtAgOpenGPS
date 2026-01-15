// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#include "tool.h"

Tool::Tool(QObject *parent)
    : QObject(parent)
    , m_sectionButtons(new SectionButtonsModel(this))
{
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
