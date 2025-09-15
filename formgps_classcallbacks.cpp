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

    connect(&CTrack::instance()->curve,SIGNAL(stopAutoSteer()),this,SLOT(onStopAutoSteer()), Qt::QueuedConnection);
    connect(&CTrack::instance()->curve,SIGNAL(TimedMessage(int,QString,QString)),this,SLOT(TimedMessageBox(int,QString,QString)), Qt::QueuedConnection);

    connect(&ct,SIGNAL(TimedMessage(int,QString,QString)),this,SLOT(TimedMessageBox(int,QString,QString)), Qt::QueuedConnection);

    connect(&mc, SIGNAL(stopAutoSteer()),this,SLOT(onStopAutoSteer()), Qt::QueuedConnection);
    connect(&mc, SIGNAL(turnOffAutoSections()),this,SLOT(onSectionMasterAutoOff()), Qt::QueuedConnection);
    connect(&mc, SIGNAL(turnOffManulSections()),this,SLOT(onSectionMasterManualOff()), Qt::QueuedConnection);

    connect(&pn, SIGNAL(checkZoomWorldGrid(double,double)),&worldGrid,SLOT(checkZoomWorldGrid(double,double)), Qt::QueuedConnection);

    connect(&recPath, SIGNAL(setSimStepDistance(double)),&sim,SLOT(setSimStepDistance(double)), Qt::QueuedConnection);
    connect(&recPath,SIGNAL(turnOffSectionMasterAuto()),this,SLOT(onSectionMasterAutoOff()), Qt::QueuedConnection);
    connect(&recPath, SIGNAL(stoppedDriving()),this,SLOT(onStoppedDriving()), Qt::QueuedConnection);

    connect(&bnd,SIGNAL(TimedMessage(int,QString,QString)),this,SLOT(TimedMessageBox(int,QString,QString)), Qt::QueuedConnection);
    //connect(&bnd, SIGNAL(soundHydLiftChange(bool)),sounds,SLOT(onHydLiftChange(bool)));

    connect(&yt, SIGNAL(outOfBounds()),&mc,SLOT(setOutOfBounds()), Qt::QueuedConnection);
    //TODO: connect(&yt,SIGNAL(turnOffBoundAlarm()),&sounds,SLOT(onTurnOffBoundAlarm()));

    //connect(settings, &AOGSettings::updateFromSettings, this, &FormGPS::loadSettings);

    connect(CTrack::instance(), SIGNAL(resetCreatedYouTurn()),&yt,SLOT(ResetCreatedYouTurn()), Qt::QueuedConnection);
    connect(CTrack::instance(), SIGNAL(saveTracks()),this,SLOT(FileSaveTracks()), Qt::QueuedConnection);
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
