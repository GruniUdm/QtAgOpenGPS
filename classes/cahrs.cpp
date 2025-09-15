#include "cahrs.h"
#include "classes/settingsmanager.h"
#include <QDebug>

CAHRS::CAHRS(QObject *parent) : QObject(parent)
{
    loadSettings();
}

void CAHRS::loadSettings()
{
    rollZero = SettingsManager::instance()->value(SETTINGS_imu_rollZero).value<double>();
    rollFilter = SettingsManager::instance()->value(SETTINGS_imu_rollFilter).value<double>();

    //is the auto steer in auto turn on mode or not
    isAutoSteerAuto = true; //SettingsManager::instance()->value(SETTINGS_as_isAutoSteerAutoOn).value<bool>();
    isRollInvert = SettingsManager::instance()->value(SETTINGS_imu_invertRoll).value<bool>();
    isDualAsIMU = SettingsManager::instance()->value(SETTINGS_imu_isDualAsIMU).value<bool>();
    isReverseOn = SettingsManager::instance()->value(SETTINGS_imu_isReverseOn).value<bool>();

    //the factor for fusion of GPS and IMU
    forwardComp = SettingsManager::instance()->value(SETTINGS_gps_forwardComp).value<double>();
    reverseComp = SettingsManager::instance()->value(SETTINGS_gps_reverseComp).value<double>();
    fusionWeight = SettingsManager::instance()->value(SETTINGS_imu_fusionWeight2).value<double>();

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
