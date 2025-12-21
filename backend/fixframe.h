// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
#ifndef FIXFRAME_H
#define FIXFRAME_H

#include <QObject>

class FixFrame
{
    Q_GADGET

    Q_PROPERTY(double age MEMBER age)
    Q_PROPERTY(double easting MEMBER easting)
    Q_PROPERTY(double northing MEMBER northing)
    Q_PROPERTY(double latitude MEMBER latitude)
    Q_PROPERTY(double longitude MEMBER longitude)
    Q_PROPERTY(double heading MEMBER heading)
    Q_PROPERTY(int fixQuality MEMBER fixQuality)
    Q_PROPERTY(double hz MEMBER hz)
    Q_PROPERTY(double rawHz MEMBER rawHz)

    Q_PROPERTY(double imuHeading MEMBER imuHeading)
    Q_PROPERTY(double imuRollDegrees MEMBER imuRollDegrees)
    Q_PROPERTY(double frameTime MEMBER frameTime)

    Q_PROPERTY(double speedKph MEMBER speedKph)
    Q_PROPERTY(double fusedHeading MEMBER fusedHeading)
    Q_PROPERTY(double toolEasting MEMBER toolEasting)
    Q_PROPERTY(double toolNorthing MEMBER toolNorthing)
    Q_PROPERTY(double toolHeading MEMBER toolHeading)
    Q_PROPERTY(double avgPivDistance MEMBER avgPivDistance)
    Q_PROPERTY(double offlineDistance MEMBER offlineDistance)

public:
    //GPS information
    double age = 0.0;
    double easting = 0.0;
    double northing = 0.0;
    double latitude = 0.0;
    double longitude = 0.0;
    double heading = 0.0;
    int fixQuality = 0;
    double hz = 0;
    double rawHz = 0;

    //imu information
    double imuHeading = 0;
    double imuRollDegrees = 0;
    double frameTime = 0;

    //vehicle state, maybe put in a different struct
    double speedKph = 0;
    double fusedHeading = 0;
    double toolEasting = 0;
    double toolNorthing = 0;
    double toolHeading = 0;
    double avgPivDistance = 0;
    short int offlineDistance = 0;
};

Q_DECLARE_METATYPE(FixFrame)

#endif // FIXFRAME_H
