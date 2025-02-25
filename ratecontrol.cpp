#include "ratecontrol.h"
//#include "aogproperty.h"
#include <QDebug>
#include "newsettings.h"
ratecontrol::ratecontrol(QObject *parent)
    : QObject{parent}
{

}
void ratecontrol::rate_bump(bool up, int ID)
{

    if (up) {

        if (ManualPWM < 250 ) ManualPWM += 10;
        else ManualPWM = 255;

    } else {
        if (ManualPWM > -250) ManualPWM -=10;
        else ManualPWM = -255;
    }
}
void ratecontrol::rate_auto(int ID)
{
    ManualPWM = 0;
}

void ratecontrol::aogset(double setwidth, double toolwidth, double aogspeed)
{
    width = toolwidth;
    swidth = setwidth;
    speed = aogspeed;
}

int ratecontrol::Command()
{
    int Result = 0;
    Result |= (1<<(ControlType+1));
    if (aBtnState > 0) Result |= (1<<6);
    if ((mBtnState > 0) || (ManualPWM == 0)) Result |= (1<<4);
    //if (ManualPWM == 0) Result |= (1<<6);
    return Result;

}

bool ratecontrol::ProductOn()
{
    bool Result = false;
    if (ControlType == 4)
    {
        Result = SensorReceiving;
    }
    else
    {
        Result = (SensorReceiving && HectaresPerMinute > 0);
    }
    return Result;
}

double ratecontrol::SmoothRate()
{
    double Result = 0;
    if (ProductOn())
    {
        double Ra = RateApplied();
        if (ProdDensity > 0) Ra *= ProdDensity;

        if (TargetRate > 0)
        {
            double Rt = Ra / TargetRate;

            if (Rt >= .9 && Rt <= 1.1 && aBtnState>0)
            {
                Result = TargetRate;
            }
            else
            {
                Result = Ra;
            }
        }
        else
        {
            Result = Ra;
        }
    }
    return Result;
}
double ratecontrol::CurrentRate()
{
    if (ProductOn())
    {
        double V = RateApplied();
        if (ProdDensity > 0) V *= ProdDensity;
        return V;
    }
    else
    {
        return 0;
    }
}
double ratecontrol::TargetUPM() // returns units per minute set rate
{
    double Result = 0;
    switch (CoverageUnits)
    {
    case 0:
        // acres
        if (AppMode == 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = swidth * speed / 600.0;
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
        if (AppMode == 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = swidth * speed / 600.0;
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
    if (ProdDensity > 0) { Result /= ProdDensity; }
    return Result;

}

double ratecontrol::RateApplied()
{   HectaresPerMinute = width * speed / 600.0;
    double Result = 0;
    switch (CoverageUnits)
    {
    case 0:
        // acres
        if (AppMode == 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = swidth * speed / 600.0;
            if (HPM > 0) Result = appRate / (HPM * 2.47);
        }

        else if (AppMode == 0 || 2)
        {
            // section controlled UPM or Document applied
            if (HectaresPerMinute > 0) Result = appRate / (HectaresPerMinute * 2.47);
        }
        else
        {
            // Document target rate
            Result = TargetRate;
        }
        break;

    case 1:
        // hectares
        if (AppMode == 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = swidth * speed / 600.0;
            if (HPM > 0) Result = appRate / HPM;
        }
        else if (AppMode == 0 || 2)
        {
            // section controlled UPM or Document applied
            if (HectaresPerMinute > 0) Result = appRate / HectaresPerMinute;
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
{   aBtnState = ID;
    ModID = pgn_data[5];
    appRate =   (qint32)((uint8_t(pgn_data[8]) << 16) + (uint8_t(pgn_data[8]) << 8) + uint8_t(pgn_data[6]));
    cQuantity = (pgn_data[11] << 16 | pgn_data[10] << 8 | pgn_data[9]) / 1000.0;
    PWMsetting = (qint16)(pgn_data[13] << 8 | pgn_data[12]);  // need to cast to 16 bit integer to preserve the sign bit
    SensorReceiving = ((pgn_data[14] & 0b00100000) == 0b00100000);
}

void ratecontrol::getsettings (int ID, QVector<int> set_data)
{   mBtnState = ID;
    ModID = set_data[0];
    ProdDensity = set_data[1];
    OnScreen = set_data[2];
    pidscale = set_data[8];
    MeterCal = set_data[9];
    TargetRate = set_data[10];
    AppMode = set_data[11];
    ControlType = set_data[12];
    CoverageUnits = set_data[13];

}
