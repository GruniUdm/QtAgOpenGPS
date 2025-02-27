#ifndef RATECONTROL_H
#define RATECONTROL_H

#include <QObject>

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
    double TargetUPM();
    double RateApplied();
    double SmoothRate();
    double CurrentRate();
    int Command(int ID);
    void dataformodule(QVector<int> set_data, QByteArray pgn_data);
    void aogset(int aBttnState, int mBttnState, double setwidth, double toolwidth, double aogspeed);
    int PWMsetting[4];
    bool SensorReceiving[4];
    int aBtnState;
    int mBtnState;
    explicit ratecontrol(QObject *parent = nullptr);

private:
    bool ProductOn();
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
    double width;
    double swidth;
    double speed;

    double ProdDensity[4];
    bool cEnableProdDensity = false;
    double cMinUPM;
    double cMinUPMbySpeed;

signals:
public slots:
    void rate_auto(int ID);
    void rate_bump(bool up, int ID);
};

#endif // RATECONTROL_H
