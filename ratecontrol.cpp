#include "ratecontrol.h"
#include "aogproperty.h"

ratecontrol::ratecontrol(QObject *parent)
    : QObject{parent}
{

}
void ratecontrol::rate_bump(bool up)
{

    if (up) {

        if (ManualPWM < 250 ) ManualPWM += 10;
        else ManualPWM = 255;

    } else {
        if (ManualPWM > -250) ManualPWM -=10;
        else ManualPWM = -255;
    }
    qDebug() << "rate_bump";
    qDebug() << ManualPWM;


}
void ratecontrol::rate_auto()
{
    ManualPWM = 0;
    qDebug() << "Rate auto";
    qDebug() << ManualPWM;
}
