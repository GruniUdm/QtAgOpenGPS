// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
#include "formgps.h"
#include "modulecomm.h"
#include "backend.h"
#include "recordedpathinterface.h"

void FormGPS::onStoppedDriving()
{
    qWarning() << "onStoppedDriving: resetting UI";
    // Only reset isDriving if it was actually set (prevents auto-restart loop)
    if (RecordedPathInterface::instance()->isDriving()) {
        RecordedPathInterface::instance()->set_isDriving(false);
    }
}
