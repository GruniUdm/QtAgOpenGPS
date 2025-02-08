#ifndef RATECONTROL_H
#define RATECONTROL_H

#include <QObject>

class ratecontrol : public QObject
{
    Q_OBJECT
public:
    explicit ratecontrol(QObject *parent = nullptr);
double TargetUPM();

signals:
};

#endif // RATECONTROL_H
