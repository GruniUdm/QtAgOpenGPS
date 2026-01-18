#ifndef RATECONTROL_H
#define RATECONTROL_H

#include <QObject>
#include <QVector>

class ratecontrol : public QObject {
    Q_OBJECT
public:
    int ModID;
    double ManualPWM[4];
    double cUPM;
    double cQuantity[4];
    double MeterCal[4];
    double RateSet;
    double actualRate;
    int ControlType[4];
    double TargetUPM(int ID);
    double RateApplied(int ID);
    double SmoothRate(int ID);
    double CurrentRate(int ID);
    int Command(int ID);
    double MinUPMSpeed(int ID);
    double MinUPM(int ID);
    void dataformodule(QVector<int> set_data, QByteArray pgn_data);
    void aogset(int aBttnState, int mBttnState, double setwidth, double toolwidth, double aogspeed);
    int PWMsetting[4];
    bool SensorReceiving[4];
    int aBtnState;
    int mBtnState;
    double cRateApplied[4];
    double cSmoothRate[4];
    double cCurrentRate[4];
    double cTargetUPM[4];
    double cMinUPMSpeed[4];
    double cMinUPM[4];
    explicit ratecontrol(QObject *parent = nullptr);

private:
    bool ProductOn(int ID);
    int OnScreen[4];
    int kp;
    int ki;
    int kd;
    int minpwm;
    int maxpwm;
    int pidscale[4];
    int rateSensor;
    double TargetRate[4];
    double HectaresPerMinute;
    int CoverageUnits[4];
    int AppMode[4];
    double appRate[4];
    double width; // tool width section control
    double swidth; // tool width fot constant upm
    double speed;
    double minSpeed[4];
    double minUPM[4];
    double ProdDensity[4];
    bool cEnableProdDensity = false;

signals:
public slots:
    void rate_auto(int ID);
    void rate_bump(bool up, int ID);
};

#endif // RATECONTROL_H
