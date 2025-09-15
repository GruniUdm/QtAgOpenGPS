// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Main event sim comms
#include "formgps.h"
#include "classes/csim.h"
#include "qmlutil.h"
#include "classes/settingsmanager.h"

/* Callback for Simulator new position */
void FormGPS::simConnectSlots()
{
    connect(&sim,SIGNAL(setActualSteerAngle(double)),this,SLOT(onSimNewSteerAngle(double)), Qt::QueuedConnection);
    connect(&sim,SIGNAL(newPosition(double,double,double,double,double,double,double)),
            this,SLOT(onSimNewPosition(double,double,double,double,double,double,double)),
            Qt::UniqueConnection);
    connect(&timerSim,SIGNAL(timeout()),this,SLOT(onSimTimerTimeout()),Qt::UniqueConnection);

    if (SettingsManager::instance()->value(SETTINGS_menu_isSimulatorOn).value<bool>()) {
        pn.latitude = sim.latitude;
        pn.longitude = sim.longitude;
        pn.headingTrue = 0;

        timerSim.start(100); //fire simulator every 100 ms.
        gpsHz = 10;
    }
}

void FormGPS::onSimNewPosition(double vtgSpeed,
                     double headingTrue,
                     double latitude,
                     double longitude, double hdop,
                     double altitude,
                     double satellitesTracked)
{

    pn.vtgSpeed = vtgSpeed;

    CVehicle::instance()->AverageTheSpeed(vtgSpeed);

    pn.headingTrue = pn.headingTrueDual = headingTrue;
    //ahrs.imuHeading = pn.headingTrue;
    //if (ahrs.imuHeading > 360) ahrs.imuHeading -= 360;
    ahrs.imuHeading = 99999;

    pn.ConvertWGS84ToLocal(latitude,longitude,pn.fix.northing,pn.fix.easting);
    pn.latitude = latitude;
    pn.longitude = longitude;

    pn.hdop = hdop;
    pn.altitude = altitude;
    pn.satellitesTracked = satellitesTracked;
    sentenceCounter = 0;
    //qWarning() << "Acted on new position.";
    UpdateFixPosition();
}

void FormGPS::onSimNewSteerAngle(double steerAngleAve)
{
    mc.actualSteerAngleDegrees = steerAngleAve;
}

/* iterate the simulator on a timer */
void FormGPS::onSimTimerTimeout()
{
    //qWarning() << "sim tick.";
    QObject *qmlobject;
    //double stepDistance = qmlobject->property("value").toReal() / 10.0 /gpsHz;
    //sim.setSimStepDistance(stepDistance);
    if (recPath.isDrivingRecordedPath || (isBtnAutoSteerOn && (CVehicle::instance()->guidanceLineDistanceOff !=32000)))
    {
        sim.DoSimTick(CVehicle::instance()->guidanceLineSteerAngle * 0.01);
    } else {
        //TODO redirect through AOGInterface
        qmlobject = qmlItem(mainWindow, "simSteer");
        double steerAngle = (qmlobject->property("value").toReal() - 300) * 0.1;
        sim.DoSimTick(steerAngle); //drive straight for now until UI
    }
}
