#include "ratecontrol.h"
#include "aogproperty.h"
#include "ctool.h"
#include "formgps.h"

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

double ratecontrol::TargetUPM() // returns units per minute set rate
{
    double Result = 0;
    switch (CoverageUnits)
    {
    case 0:
        // acres
        if (cAppMode = property_setRateContType)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = tool.width * speedData / 600.0;
            Result = TargetRate() * HPM * 2.47;
        }
        else
        {
            // section controlled UPM, Document applied or Document target
            Result = TargetRate() * cHectaresPerMinute * 2.47;
        }
        break;

    case 1:
        // hectares
        if (cAppMode == ApplicationMode.ConstantUPM)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = tool.width * speedData / 600.0;
            Result = TargetRate() * HPM;
        }
        else
        {
            // section controlled UPM, Document applied or Document target
            Result = TargetRate() * cHectaresPerMinute;
        }
        break;

    case 2:
        // minutes
        Result = TargetRate();
        break;

    default:
        // hours
        Result = TargetRate() / 60;
        break;
    }

    // added this back in to change from lb/min to ft^3/min, Moved from PGN32614.
    if (cEnableProdDensity && cProdDensity > 0) { Result /= cProdDensity; }

    return Result;
}
double ratecontrol::TargetRate()
{
    double Result = 0;

    return Result;
}
