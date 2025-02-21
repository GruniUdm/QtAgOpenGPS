#ifndef RATECONTROL_H
#define RATECONTROL_H

#include <QObject>

class ratecontrol : public QObject
{
    Q_OBJECT
public:
    int ModID;
    int ManualPWM = 0;
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
    void getfrommodule(int ID, QByteArray pgn_data);
    void aogset(int toolwidth, double aogspeed);
    void getsettings(int ID, QVector<int> set_data);
    int PWMsetting;
    int SensorReceiving;
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
    double speed;

    double ProdDensity = 0;
    bool cEnableProdDensity = false;
    double cMinUPM;
    double cMinUPMbySpeed;














signals:
public slots:
    void rate_auto();
    void rate_bump(bool up);

};

#endif // RATECONTROL_H
