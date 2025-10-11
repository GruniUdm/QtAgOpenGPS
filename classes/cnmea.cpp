#include <QCoreApplication>
#include <math.h>
#include "cnmea.h"
#include "vec2.h"
#include "glm.h"
#include "classes/settingsmanager.h"
#include "formgps.h"  // Phase 6.0.20 Task 24 Step 3.5 - For FormGPS qobject_cast


CNMEA::CNMEA(QObject *parent) : QObject(parent)
{
    // Phase 6.0.24 Problem 18: Initialize GPS position variables to safe defaults
    // Defense in depth - ensures coherent values even if class member initialization forgotten
    latitude = 0.0;
    longitude = 0.0;
    prevLatitude = 0.0;
    prevLongitude = 0.0;

    // GPS quality indicators - 0 = Invalid fix, no satellites
    fixQuality = 0;
    satellitesTracked = 0;

    loadSettings();
}

void CNMEA::loadSettings(void)
{
    ageAlarm  = SettingsManager::instance()->gps_ageAlarm();
}

//moved to CVehicle
/*
void CNMEA::AverageTheSpeed()
{
    mf.avgSpeed = (mf.avgSpeed * 0.75) + (speed * 0.25);
}
*/

void CNMEA::SetLocalMetersPerDegree(QObject* aog)
{
    if (!aog) {
        qWarning() << "⚠️ CNMEA::SetLocalMetersPerDegree - aog is NULL";
        return;
    }

    // Phase 6.0.20 Task 24 Step 3.5: mPerDegreeLat calculated automatically in FormGPS
    // when latStart is set. mPerDegreeLon calculated locally in conversion functions.

    double northing, easting;

    ConvertWGS84ToLocal(latitude, longitude, northing, easting, aog);
    emit checkZoomWorldGrid(northing, easting); //ask main form to call checkZoomWorldGrid

}

void CNMEA::ConvertWGS84ToLocal(double Lat, double Lon, double &outNorthing, double &outEasting, QObject* aog)
{
    // Phase 6.0.20 Task 24 Step 3.5: Cast to FormGPS for direct Q_PROPERTY access
    FormGPS* formGPS = qobject_cast<FormGPS*>(aog);
    if (!formGPS) {
        qWarning() << "⚠️ CNMEA::ConvertWGS84ToLocal - aog is not FormGPS!";
        outNorthing = 0;
        outEasting = 0;
        return;
    }

    // Phase 6.0.20 Task 24 Step 3.5: Direct Q_PROPERTY access (no property() calls)
    double latStart = formGPS->latStart();
    double lonStart = formGPS->lonStart();
    double mPerDegreeLat = formGPS->mPerDegreeLat();

    // Phase 6.0.20 Task 24 Step 3.5: Calculate mPerDegreeLon locally for geodetic precision
    double mPerDegreeLon = 111412.84 * cos(Lat * 0.01745329251994329576923690766743)
                         - 93.5 * cos(3.0 * Lat * 0.01745329251994329576923690766743)
                         + 0.118 * cos(5.0 * Lat * 0.01745329251994329576923690766743);

    outNorthing = (Lat - latStart) * mPerDegreeLat;
    outEasting = (Lon - lonStart) * mPerDegreeLon;

    //Northing += mf.RandomNumber(-0.02, 0.02);
    //Easting += mf.RandomNumber(-0.02, 0.02);
}

void CNMEA::ConvertLocalToWGS84(double Northing, double Easting, double &outLat, double &outLon, QObject* aog)
{
    // Phase 6.0.20 Task 24 Step 3.5: Cast to FormGPS for direct Q_PROPERTY access
    FormGPS* formGPS = qobject_cast<FormGPS*>(aog);
    if (!formGPS) {
        qWarning() << "⚠️ CNMEA::ConvertLocalToWGS84 - aog is not FormGPS!";
        outLat = 0;
        outLon = 0;
        return;
    }

    // Phase 6.0.20 Task 24 Step 3.5: Direct Q_PROPERTY access (no property() calls)
    double latStart = formGPS->latStart();
    double lonStart = formGPS->lonStart();
    double mPerDegreeLat = formGPS->mPerDegreeLat();

    outLat = ((Northing + fixOffset.northing) / mPerDegreeLat) + latStart;

    // Phase 6.0.20 Task 24 Step 3.5: Calculate mPerDegreeLon locally with output latitude
    double mPerDegreeLon = 111412.84 * cos(outLat * 0.01745329251994329576923690766743)
                         - 93.5 * cos(3.0 * outLat * 0.01745329251994329576923690766743)
                         + 0.118 * cos(5.0 * outLat * 0.01745329251994329576923690766743);

    outLon = ((Easting + fixOffset.easting) / mPerDegreeLon) + lonStart;
}

QString CNMEA::GetLocalToWSG84_KML(double Easting, double Northing, QObject* aog)
{
    // Phase 6.0.20 Task 24 Step 3.5: Cast to FormGPS for direct Q_PROPERTY access
    FormGPS* formGPS = qobject_cast<FormGPS*>(aog);
    if (!formGPS) {
        qWarning() << "⚠️ CNMEA::GetLocalToWSG84_KML - aog is not FormGPS!";
        return "0, 0, 0";
    }

    // Phase 6.0.20 Task 24 Step 3.5: Direct Q_PROPERTY access (no property() calls)
    double latStart = formGPS->latStart();
    double lonStart = formGPS->lonStart();
    double mPerDegreeLat = formGPS->mPerDegreeLat();

    double Lat = (Northing / mPerDegreeLat) + latStart;

    // Phase 6.0.20 Task 24 Step 3.5: Calculate mPerDegreeLon locally with calculated latitude
    double mPerDegreeLon = 111412.84 * cos(Lat * 0.01745329251994329576923690766743)
                         - 93.5 * cos(3.0 * Lat * 0.01745329251994329576923690766743)
                         + 0.118 * cos(5.0 * Lat * 0.01745329251994329576923690766743);

    double Lon = (Easting / mPerDegreeLon) + lonStart;

    return QString("%1, %2, 0").arg(Lon,0,'g',7).arg(Lat,0,'g',7); //shouldn't use locale
}
