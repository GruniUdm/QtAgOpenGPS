// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#include "tool.h"

Tool::Tool(QObject *parent)
    : QObject(parent)
    , m_sections(new SectionsModel(this))
{
}
