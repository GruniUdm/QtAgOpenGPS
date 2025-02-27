#ifndef RATECONTROL_H
#define RATECONTROL_H

#include <QObject>

class ratecontrol : public QObject {
    Q_OBJECT
public:
    int ModID;
    double ManualPWM[4];
    double cUPM;
    double cQuantity;
    double MeterCal;
    double RateSet;
    double actualRate;
    int ControlType;
    double TargetUPM();
    double RateApplied();
    double SmoothRate();
    double CurrentRate();
    int Command(int ID);
    void dataformodule(QVector<int> set_data, QByteArray pgn_data);
    void aogset(int aBttnState, int mBttnState, double setwidth, double toolwidth, double aogspeed);
    int PWMsetting;
    bool SensorReceiving;
    int aBtnState;
    int mBtnState;
    explicit ratecontrol(QObject *parent = nullptr);

private:
    bool ProductOn();
    int OnScreen;
    int kp;
    int ki;
    int kd;
    int minpwm;
    int maxpwm;
    int pidscale;
    int rateSensor;
    double TargetRate;
    double HectaresPerMinute;
    int CoverageUnits;
    int AppMode;
    double appRate;
    double width;
    double swidth;
    double speed;

    double ProdDensity = 0;
    bool cEnableProdDensity = false;
    double cMinUPM;
    double cMinUPMbySpeed;

signals:
public slots:
    void rate_auto(int ID);
    void rate_bump(bool up, int ID);
};

#endif // RATECONTROL_H
