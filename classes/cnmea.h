#ifndef CNMEA_H
#define CNMEA_H

#include <QObject>
#include <sstream>
#include <QString>
#include <QDateTime>
#include <QByteArray>
#include <QBuffer>
#include "vec2.h"
#include "glm.h"
#include <QTextStream>

class CNMEA : public QObject
{
    Q_OBJECT
private:
public:
    //WGS84 Lat Long
    double latitude, longitude;

    double prevLatitude, prevLongitude;

    // Phase 6.0.20 Task 24 Step 3.5: mPerDegreeLat moved to FormGPS Q_PROPERTY
    // mPerDegreeLon calculated locally in conversion functions (geodetic precision)

    //our current fix
    //moved to CVehicle
    Vec2 fix = Vec2(0, 0);

    Vec2 prefSpeedFix= Vec2(0, 0);

    //used to offset the antenna position to compensate for drift
    Vec2 fixOffset = Vec2(0, 0);

    // GPS data with safe default values (0.0 = no data yet)
    // Phase 6.0.24 Problem 18: Initialize to avoid garbage values at startup
    double altitude = 0.0;
    double speed = 0.0;
    double newSpeed = 0.0;
    double vtgSpeed = 0.0;  // Was DOUBLE_MAX - caused infinite speed display at startup

    // Heading and quality data - NMEA standard defaults
    // hdop=99.9, age=99.9 = standard NMEA values for "no data available"
    double headingTrueDual = 0.0;
    double headingTrue = 0.0;
    double hdop = 99.9;
    double age = 99.9;
    double headingTrueDualOffset = 0.0;

    int fixQuality, ageAlarm;
    int satellitesTracked;

    QString logNMEASentence;

    //StringBuilder logNMEASentence = new StringBuilder();

    explicit CNMEA(QObject *parent = 0);

    //
    //void AverageTheSpeed(); moved to CVehicle
    void SetLocalMetersPerDegree(QObject* aog);
    void ConvertWGS84ToLocal(double Lat, double Lon, double &outNorthing, double &outEasting, QObject* aog);
    void ConvertLocalToWGS84(double Northing, double Easting, double &outLat, double &outLon, QObject* aog);
    QString GetLocalToWSG84_KML(double Easting, double Northing, QObject* aog);

    void loadSettings(void);

signals:
    //void setAveSpeed(double);
    void checkZoomWorldGrid(double, double);
};

#endif // CNMEA_H
