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

        if (ManualPWM[ID] < 250 ) ManualPWM[ID] += 10;
        else ManualPWM[ID] = 255;

    } else {
        if (ManualPWM[ID] > -250) ManualPWM[ID] -=10;
        else ManualPWM[ID] = -255;
    }
}
void ratecontrol::rate_auto(int ID)
{
    ManualPWM[ID] = 0;
}

void ratecontrol::aogset(int aBttnState, int mBttnState, double setwidth, double toolwidth, double aogspeed)
{   aBtnState = aBttnState;
    mBtnState = mBttnState;
    width = toolwidth;
    swidth = setwidth;
    speed = aogspeed;
}

int ratecontrol::Command(int ID)
{
    int Result = 0;
    Result |= (1<<(ControlType[ID]+1));
    if (aBtnState > 0) Result |= (1<<6);
    if ((mBtnState > 0) || (ManualPWM[ID] == 0)) Result |= (1<<4);
    //if (ManualPWM == 0) Result |= (1<<6);
    return Result;

}

bool ratecontrol::ProductOn(int ID)
{
    bool Result = false;
    if (ControlType[ID] == 4)
    {
        Result = SensorReceiving[ID];
    }
    else
    {
        Result = (SensorReceiving[ID] && HectaresPerMinute > 0);
    }
    return Result;
}

double ratecontrol::SmoothRate(int ID)
{
    double Result = 0;
    if (ProductOn(ID))
    {
        double Ra = RateApplied(ID);
        if (ProdDensity[ID] > 0) Ra *= ProdDensity[ID];

        if (TargetRate[ID] > 0)
        {
            double Rt = Ra / TargetRate[ID];

            if (Rt >= .9 && Rt <= 1.1 && aBtnState>0)
            {
                Result = TargetRate[ID];
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
double ratecontrol::CurrentRate(int ID)
{
    if (ProductOn(ID))
    {
        double V = RateApplied(ID);
        if (ProdDensity[ID] > 0) V *= ProdDensity[ID];
        return V;
    }
    else
    {
        return 0;
    }
}
double ratecontrol::TargetUPM(int ID) // returns units per minute set rate
{
    double Result = 0;
    switch (CoverageUnits[ID])
    {
    case 0:
        // acres
        if (AppMode[ID] == 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = swidth * speed / 600.0;
            Result = TargetRate[ID] * HPM * 2.47;
        }
        else
        {
            // section controlled UPM, Document applied or Document target
            Result = TargetRate[ID] * HectaresPerMinute * 2.47;
        }
        break;

    case 1:
        // hectares
        if (AppMode[ModID] == 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = swidth * speed / 600.0;
            Result = TargetRate[ID] * HPM;
        }
        else
        {
            // section controlled UPM, Document applied or Document target
            Result = TargetRate[ID] * HectaresPerMinute;
        }
        break;

    case 2:
        // minutes
        Result = TargetRate[ID];
        break;

    default:
        // hours
        Result = TargetRate[ID] / 60;
        break;
    }

    // added this back in to change from lb/min to ft^3/min, Moved from PGN32614.
    if (ProdDensity[ID] > 0) { Result /= ProdDensity[ID]; }
    return Result;

}

double ratecontrol::RateApplied(int ID)
{   HectaresPerMinute = width * speed / 600.0;
    double Result = 0;
    switch (CoverageUnits[ID])
    {
    case 0:
        // acres
        if (AppMode[ID] == 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = swidth * speed / 600.0;
            if (HPM > 0) Result = appRate[ID] / (HPM * 2.47);
        }

        else if (AppMode[ID] == 0 || 2)
        {
            // section controlled UPM or Document applied
            if (HectaresPerMinute > 0) Result = appRate[ID] / (HectaresPerMinute * 2.47);
        }
        else
        {
            // Document target rate
            Result = TargetRate[ID];
        }
        break;

    case 1:
        // hectares
        if (AppMode[ID] == 1)
        {
            // Constant UPM
            // same upm no matter how many sections are on
            double HPM = swidth * speed / 600.0;
            if (HPM > 0) Result = appRate[ID] / HPM;
        }
        else if (AppMode[ID] == 0 || 2)
        {
            // section controlled UPM or Document applied
            if (HectaresPerMinute > 0) Result = appRate[ID] / HectaresPerMinute;
        }
        else
        {
            // Document target rate
            Result = TargetRate[ID];
        }
        break;

    case 2:
        // minutes
        if (AppMode[ID] == 3)
        {
            // document target rate
            Result = TargetRate[ID];
        }
        else
        {
            Result = appRate[ID];
        }
        break;

    default:
        // hours
        if (AppMode[ModID] == 3)
        {
            // document target rate
            Result = TargetRate[ID];
        }
        else
        {
            Result = appRate[ID] * 60;
        }
        break;
    }
    return Result;

}

void ratecontrol::dataformodule (QVector<int> set_data, QByteArray pgn_data)
{

    int ID = set_data[0];
    ProdDensity[ID] = set_data[1];
    OnScreen[ID] = set_data[2];
    pidscale[ID] = set_data[8];
    MeterCal[ID] = set_data[9];
    TargetRate[ID] = set_data[10];
    AppMode[ID] = set_data[11];
    ControlType[ID] = set_data[12];
    CoverageUnits[ID] = set_data[13];
    //ModID = pgn_data[5];
    appRate[ModID] =   (qint32)((uint8_t(pgn_data[8]) << 16) + (uint8_t(pgn_data[7]) << 8) + uint8_t(pgn_data[6]));
    cQuantity[ModID] = (pgn_data[11] << 16 | pgn_data[10] << 8 | pgn_data[9]) / 1000.0;
    PWMsetting[ModID] = (qint16)(pgn_data[13] << 8 | pgn_data[12]);  // need to cast to 16 bit integer to preserve the sign bit
    SensorReceiving[ModID] = ((pgn_data[14] & 0b00100000) == 0b00100000);
    qDebug() << "appRate[ModID]";
    qDebug() << appRate[ModID];
}

