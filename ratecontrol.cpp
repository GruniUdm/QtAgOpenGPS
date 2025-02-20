#include "ratecontrol.h"
//#include "aogproperty.h"
#include <QDebug>
#include "newsettings.h"
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

void ratecontrol::aogset(int toolwidth, double aogspeed)
{
    width = toolwidth;
    speed = aogspeed;
}

double ratecontrol::TargetUPM() // returns units per minute set rate
{
    double Result = 0;
    switch (CoverageUnits)
    {
    case 0:
        // acres
        if (AppMode = 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = width * speed / 600.0;
            Result = TargetRate * HPM * 2.47;
        }
        else
        {
            // section controlled UPM, Document applied or Document target
            Result = TargetRate * HectaresPerMinute * 2.47;
        }
        break;

    case 1:
        // hectares
        if (AppMode = 2)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = width * speed / 600.0;
            Result = TargetRate * HPM;
        }
        else
        {
            // section controlled UPM, Document applied or Document target
            Result = TargetRate * HectaresPerMinute;
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
    //if (cEnableProdDensity && cProdDensity > 0) { Result /= cProdDensity; }

    return Result;
}

double ratecontrol::RateApplied()
{
    double Result = 0;
    switch (CoverageUnits)
    {
    case 0:
        // acres
        if (AppMode == 0 || 2)
        {
            // section controlled UPM or Document applied
            if (HectaresPerMinute > 0) Result = appRate / (HectaresPerMinute * 2.47);
        }
        else if (AppMode == 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = width * speed / 600.0;
            if (HPM > 0) Result = appRate / (HPM * 2.47);
        }
        else
        {
            // Document target rate
            Result = TargetRate;
        }
        break;

    case 1:
        // hectares
        if (AppMode == 0 || 2)
        {
            // section controlled UPM or Document applied
            if (HectaresPerMinute > 0) Result = appRate / HectaresPerMinute;
        }
        else if (AppMode == 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = width * speed / 600.0;
            if (HPM > 0) Result = appRate / HPM;
        }
        else
        {
            // Document target rate
            Result = TargetRate;
        }
        break;

    case 2:
        // minutes
        if (AppMode == 3)
        {
            // document target rate
            Result = TargetRate;
        }
        else
        {
            Result = appRate;
        }
        break;

    default:
        // hours
        if (AppMode == 3)
        {
            // document target rate
            Result = TargetRate;
        }
        else
        {
            Result = appRate * 60;
        }
        break;
    }

    return Result;
}

void ratecontrol::getfrommodule (int ID, QByteArray pgn_data)
{
    ModID = pgn_data[5];
    appRate =  (pgn_data[8] << 16 | pgn_data[7] << 8 | pgn_data[6]) / 1000.0;
    cQuantity = (pgn_data[11] << 16 | pgn_data[10] << 8 | pgn_data[9]) / 1000.0;
    cPWMsetting = (qint16)(pgn_data[13] << 8 | pgn_data[12]);  // need to cast to 16 bit integer to preserve the sign bit
    cSensorReceiving = ((pgn_data[14] & 0b00000001) == 0b00000001);

    qDebug() << "Rate ";
    qDebug() << TargetRate;
}

void ratecontrol::getsettings (int ID, QVector<int> set_data)
{
    ModID = set_data[0];
    OnScreen = set_data[2];
    pidscale = set_data[8];
    MeterCal = set_data[9];
    TargetRate = set_data[10];
    AppMode = set_data[11];
    ControlType = set_data[12];
    CoverageUnits = set_data[13];
}
