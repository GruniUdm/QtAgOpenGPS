// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
//
#include "formgps.h"

/***************************************
 * callbacks for classes to connect to *
 ***************************************/

//TimedMessageBox defined in ??????, not here

void FormGPS::connect_classes()
{
    simConnectSlots();

    // Phase 6.0.33: GPS timer for real GPS mode (50 Hz fixed rate)
    // 50 Hz = 20ms interval for smooth rendering and PGN 254 AutoSteer commands
    connect(&timerGPS, &QTimer::timeout, this, &FormGPS::onGPSTimerTimeout, Qt::UniqueConnection);
    // Timer will be started when GPS data starts arriving (not in simulation mode)
    timerGPS.start(100);  // 100ms = 10 Hz (synchronized with NMEA data rate)

    connect(&track.curve, &CABCurve::stopAutoSteer, this, &FormGPS::onStopAutoSteer, Qt::QueuedConnection);
    connect(&track.curve, &CABCurve::TimedMessage, this, &FormGPS::TimedMessageBox, Qt::QueuedConnection);

    connect(&ct, &CContour::TimedMessage, this, &FormGPS::TimedMessageBox, Qt::QueuedConnection);

    connect(&mc, &CModuleComm::stopAutoSteer, this, &FormGPS::onStopAutoSteer, Qt::QueuedConnection);
    connect(&mc, &CModuleComm::turnOffAutoSections, this, &FormGPS::onSectionMasterAutoOff, Qt::QueuedConnection);
    connect(&mc, &CModuleComm::turnOffManulSections, this, &FormGPS::onSectionMasterManualOff, Qt::QueuedConnection);

    connect(&pn, &CNMEA::checkZoomWorldGrid, &worldGrid, &CWorldGrid::checkZoomWorldGrid, Qt::QueuedConnection);

    connect(&recPath, &CRecordedPath::turnOffSectionMasterAuto, this, &FormGPS::onSectionMasterAutoOff, Qt::QueuedConnection);
    connect(&recPath, &CRecordedPath::stoppedDriving, this, &FormGPS::onStoppedDriving, Qt::QueuedConnection);

    connect(&bnd, &CBoundary::TimedMessage, this, &FormGPS::TimedMessageBox, Qt::QueuedConnection);
    //connect(&bnd, SIGNAL(soundHydLiftChange(bool)),sounds,SLOT(onHydLiftChange(bool)));
    connect(&bnd, &CBoundary::saveBoundaryRequested, this, &FormGPS::FileSaveBoundary, Qt::DirectConnection);

    //connect(settings, &AOGSettings::updateFromSettings, this, &FormGPS::loadSettings);

    connect(&track, &CTrack::resetCreatedYouTurn, &yt, &CYouTurn::ResetCreatedYouTurn, Qt::QueuedConnection);
    connect(&track, &CTrack::saveTracks, this, &FormGPS::FileSaveTracks, Qt::QueuedConnection);
}

void FormGPS::onStopAutoSteer()
{
    //TODO
    //turn off auto steer
    //reset gui buttons
}

void FormGPS::onSectionMasterAutoOff()
{
    //turn off sections
    //reset GUI buttons
}

void FormGPS::onSectionMasterManualOff()
{
    //turn off sections
    //reset GUI buttons
}

void FormGPS::onStoppedDriving()
{
    //reset btnPathGoStop
    //reset btnPathRecordStop
    //reset btnPickPath
    //reset btnResumePath
}
