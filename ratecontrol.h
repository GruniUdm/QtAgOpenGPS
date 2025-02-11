#ifndef RATECONTROL_H
#define RATECONTROL_H

#include <QObject>

class ratecontrol : public QObject
{
    Q_OBJECT
public:
    double width;
    //PGN32400 RateSensor;
    //PGN32500 ModuleRateSettings;
    int ManualPWM = 0;
    int ControlType = 0;
    uint8_t CoverageUnits = 0;
    double cUPM;
    double cQuantity;
    double MeterCal;
    double RateSet;
    double TargetRate();
    int cPWMsetting;
    int cSensorReceiving;
    explicit ratecontrol(QObject *parent = nullptr);
    double TargetUPM();
private:

    double AccumulatedLast = 0;
    double cAppMode = 0;
    bool cBumpButtons;
    bool cCalRun;
    bool cCalSetMeter;
    bool cCalUseBaseRate;
    double cControlType = 0;
    int cCountsRev;
    bool cEnabled = true;
    bool cEnableProdDensity = false;
    bool cFanOn;
    double cHectaresPerMinute;
    double cHours1;
    double cHours2;
    int cManualPWM;
    double cMeterCal = 0;
    double cMinUPM;
    double cMinUPMbySpeed;
    int cModID;
    uint8_t cOffRateSetting;
    bool cOnScreen;
    double Coverage = 0;
    double Coverage2 = 0;
    double cProdDensity = 0;
    int cProductID;
    QString cProductName = "";
    QString cQuantityDescription = "Lbs";
    double cRateAlt = 100;
    double cRateSet = 0;
    int cSenID;
    int cSerialPort;
    int cShiftRange = 4;
    double cTankStart = 0;
    double cUnitsApplied = 0;
    double cUnitsApplied2 = 0;
    double CurrentMinutes;
    double CurrentWorkedArea_Hc = 0;
    bool cUseAltRate = false;
    bool cUseMinUPMbySpeed = false;
    bool cUseOffRateAlarm;
    bool cUseVR;
    uint8_t cVRID = 0;
    double cVRmax;
    double cVRmin;
    bool PauseWork = false;
    double TankSize = 0;
signals:
public slots:
    void rate_auto();
    void rate_bump(bool up);
};

#endif // RATECONTROL_H
