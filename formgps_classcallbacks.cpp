// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
//
#include "formgps.h"
#include "aogsettings.h"


extern AOGSettings *settings;

/***************************************
 * callbacks for classes to connect to *
 ***************************************/

//TimedMessageBox defined in ??????, not here

void FormGPS::connect_classes()
{
    simConnectSlots();

    connect(&trk.curve,SIGNAL(stopAutoSteer()),this,SLOT(onStopAutoSteer()));
    connect(&trk.curve,SIGNAL(TimedMessage(int,QString,QString)),this,SLOT(TimedMessageBox(int,QString,QString)));

    connect(&ct,SIGNAL(TimedMessage(int,QString,QString)),this,SLOT(TimedMessageBox(int,QString,QString)));

    connect(&mc, SIGNAL(stopAutoSteer()),this,SLOT(onStopAutoSteer()));
    connect(&mc, SIGNAL(turnOffAutoSections()),this,SLOT(onSectionMasterAutoOff()));
    connect(&mc, SIGNAL(turnOffManulSections()),this,SLOT(onSectionMasterManualOff()));

    connect(&pn, SIGNAL(checkZoomWorldGrid(double,double)),&worldGrid,SLOT(checkZoomWorldGrid(double,double)));

    connect(&recPath, SIGNAL(setSimStepDistance(double)),&sim,SLOT(setSimStepDistance(double)));
    connect(&recPath,SIGNAL(turnOffSectionMasterAuto()),this,SLOT(onSectionMasterAutoOff()));
    connect(&recPath, SIGNAL(stoppedDriving()),this,SLOT(onStoppedDriving()));

    connect(&bnd,SIGNAL(TimedMessage(int,QString,QString)),this,SLOT(TimedMessageBox(int,QString,QString)));
    //connect(&bnd, SIGNAL(soundHydLiftChange(bool)),sounds,SLOT(onHydLiftChange(bool)));

    connect(&yt, SIGNAL(outOfBounds()),&mc,SLOT(setOutOfBounds()));
    //TODO: connect(&yt,SIGNAL(turnOffBoundAlarm()),&sounds,SLOT(onTurnOffBoundAlarm()));

    //connect(settings, &AOGSettings::updateFromSettings, this, &FormGPS::loadSettings);

    connect(&trk, SIGNAL(resetCreatedYouTurn()),&yt,SLOT(ResetCreatedYouTurn()));
    connect(&trk, SIGNAL(saveTracks()),this,SLOT(FileSaveTracks()));
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
