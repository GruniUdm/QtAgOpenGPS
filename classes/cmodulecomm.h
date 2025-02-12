#ifndef CMODULECOMM_H
#define CMODULECOMM_H

#include <QObject>
#include <QtCore>
#include <QString>
#include "interfaceproperty.h"

class CAHRS;

class CModuleComm: public QObject
{
    Q_OBJECT

public:
    bool isOutOfBounds = true;

    // ---- Section control switches to AOG  ---------------------------------------------------------
    //PGN - 32736 - 127.249 0x7FF9
    uchar ss[9];

    uchar ssP[9];

    uchar blockagemoduleid[16];
    uchar blockagesecnum[16];
    int blockageseccount[64];
    int blockageseccount1[16];
    int blockageseccount2[16];
    int blockageseccount3[16];
    int blockageseccount4[16];
    int
            swHeader = 0,
            swMain = 1,
            swReserve = 2,
            swReserve2 = 3,
            swNumSections = 4,
            swOnGr0 = 5,
            swOffGr0 = 6,
            swOnGr1 = 7,
            swOffGr1 = 8;

    int pwmDisplay = 0;
    double actualSteerAngleDegrees = 0;
    int actualSteerAngleChart = 0;

    InterfaceProperty<AOGInterface, int> sensorData = InterfaceProperty<AOGInterface, int>("sensorData");

    //for the workswitch
    bool isWorkSwitchActiveLow, isRemoteWorkSystemOn, isWorkSwitchEnabled,
        isWorkSwitchManualSections, isSteerWorkSwitchManualSections, isSteerWorkSwitchEnabled;

    bool workSwitchHigh, oldWorkSwitchHigh, steerSwitchHigh, oldSteerSwitchHigh, oldSteerSwitchRemote;

    explicit CModuleComm(QObject *parent = 0);
    void CheckWorkAndSteerSwitch(CAHRS &ahrs, bool isBtnAutoSteerOn);
signals:
    void stopAutoSteer(void);
    void turnOffManulSections(void);
    void turnOffAutoSections(void);

public slots:
    void set_actualSteerAngleDegrees(double newSteerAngleDegrees) {
        actualSteerAngleDegrees = newSteerAngleDegrees;
    }

    void setOutOfBounds() {
        isOutOfBounds = true;
    }
};

#endif // CMODULECOMM_H
