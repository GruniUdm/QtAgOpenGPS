#include "ratecontrol.h"
#include "aogproperty.h"
#include "ctool.h"
#include "formgps.h"
#include "cpgn.h"

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
            double HPM = width * speed / 600.0;
            Result = TargetRate * HPM * 2.47;
        }
        else
        {
            // section controlled UPM, Document applied or Document target
            Result = TargetRate * cHectaresPerMinute * 2.47;
        }
        break;

    case 1:
        // hectares
        if (cAppMode = property_setRateContType)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = width * speed / 600.0;
            Result = TargetRate * HPM;
        }
        else
        {
            // section controlled UPM, Document applied or Document target
            Result = TargetRate * cHectaresPerMinute;
        }
        break;

    case 2:
        // minutes
        Result = TargetRate;
        break;

    default:
        // hours
        Result = TargetRate / 60;
        break;
    }

    // added this back in to change from lb/min to ft^3/min, Moved from PGN32614.
    if (cEnableProdDensity && cProdDensity > 0) { Result /= cProdDensity; }

    return Result;
}

void ratecontrol::set (int ID, QByteArray pgn_data)
{
    cProductID = ID;
    //TargetRate = rateset;
    TargetRate =  (pgn_data[8] << 16 | pgn_data[7] << 8 | pgn_data[6]) / 1000.0;
    cQuantity = (pgn_data[11] << 16 | pgn_data[10] << 8 | pgn_data[9]) / 1000.0;
    cPWMsetting = (qint16)(pgn_data[13] << 8 | pgn_data[12]);  // need to cast to 16 bit integer to preserve the sign bit
    cSensorReceiving = ((pgn_data[14] & 0b00000001) == 0b00000001);

    qDebug() << "Rate ";
    qDebug() << TargetRate;
}



