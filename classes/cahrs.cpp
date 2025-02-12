#include "cahrs.h"
#include "newsettings.h"
#include <QDebug>

CAHRS::CAHRS(QObject *parent) : QObject(parent)
{
    loadSettings();
}

void CAHRS::loadSettings()
{
    rollZero = settings->value("imu/rollZero").value<double>();
    rollFilter = settings->value("imu/rollFilter").value<double>();

    //is the auto steer in auto turn on mode or not
    isAutoSteerAuto = settings->value("as/isAutoSteerAutoOn").value<bool>();
    isRollInvert = settings->value("imu/invertRoll").value<bool>();
    isDualAsIMU = settings->value("imu/isDualAsIMU").value<bool>();
    isReverseOn = settings->value("imu/isReverseOn").value<bool>();

    //the factor for fusion of GPS and IMU
    forwardComp = settings->value("gps/forwardComp").value<double>();
    reverseComp = settings->value("gps/reverseComp").value<double>();
    fusionWeight = settings->value("imu/fusionWeight2").value<double>();

}

void CAHRS::changeImuHeading(double newImuHeading) {
    imuHeading = newImuHeading;
    qDebug() << "changed imuHeading to " << newImuHeading;
}

void CAHRS::changeImuRoll(double newImuRoll) {
    //new roll number
    imuRoll = newImuRoll;
    qDebug() << "changed imuRoll to " << newImuRoll;
}
