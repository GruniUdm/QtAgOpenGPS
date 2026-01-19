// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Rate control source code used from https://github.com/SK21/AOG_RC
#include "RateControl.h"
#include <QDebug>
#include "agioservice.h"
#include "pgnparser.h"

Q_LOGGING_CATEGORY (rc_log, "backend.qtagopengps")

RateControl *RateControl::s_instance = nullptr;
QMutex RateControl::s_mutex;
bool RateControl::s_cpp_created = false;

// ИСПРАВЛЕНО: Добавлена полная инициализация всех членов класса
RateControl::RateControl(QObject *parent)
    : QObject{parent},
    ModID(0),
    cUPM(0),
    RateSet(0),
    actualRate(0),
    aBtnState(0),
    mBtnState(0),
    HectaresPerMinute(0),
    width(0),
    swidth(0),
    speed(0),
    kp(0),
    ki(0),
    kd(0),
    minpwm(0),
    maxpwm(0),
    rateSensor(0)
{
    //connect us to agio
    connect(AgIOService::instance(), &AgIOService::rateControlDataReady,
            this, &RateControl::onRateControlDataReady, Qt::DirectConnection);
    // Инициализация всех массивов
    for (int i = 0; i < 4; i++) {
        ManualPWM[i] = 0;
        cQuantity[i] = 0;
        MeterCal[i] = 0;
        ControlType[i] = 0;
        PWMsetting[i] = 0;
        SensorReceiving[i] = false;
        cRateApplied[i] = 0;
        cSmoothRate[i] = 0;
        cCurrentRate[i] = 0;
        cTargetUPM[i] = 0;
        cMinUPMSpeed[i] = 0;
        cMinUPM[i] = 0;
        OnScreen[i] = 0;
        pidscale[i] = 0;
        CoverageUnits[i] = 0;
        AppMode[i] = 0;
        appRate[i] = 0;
        minSpeed[i] = 0;
        minUPM[i] = 0;
        ProdDensity[i] = 0;
        TargetRate[i] = 0;
    }
}

RateControl *RateControl::instance() {
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new RateControl();
        qDebug(rc_log) << "RateControl singleton created by C++ code.";
        s_cpp_created = true;
        // ensure cleanup on app exit
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                             delete s_instance; s_instance = nullptr;
                         });
    }
    return s_instance;
}

RateControl *RateControl::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if(!s_instance) {
        s_instance = new RateControl();
        qDebug(rc_log) << "RateControl singleton created by QML engine.";
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}

void RateControl::rate_bump(bool up, int ID)
{
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return;

    if (up) {
        if (ManualPWM[ID] < 250) ManualPWM[ID] += 10;
        else ManualPWM[ID] = 255;
    } else {
        if (ManualPWM[ID] > -250) ManualPWM[ID] -= 10;
        else ManualPWM[ID] = -255;
    }
}

void RateControl::rate_auto(int ID)
{
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return;
    ManualPWM[ID] = 0;
}

void RateControl::aogset(int aBttnState, int mBttnState, double setwidth, double toolwidth, double aogspeed)
{
    aBtnState = aBttnState;
    mBtnState = mBttnState;
    width = toolwidth;
    swidth = setwidth;
    speed = aogspeed;
}

int RateControl::Command(int ID)
{
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return 0;

    int Result = 0;

    // ИСПРАВЛЕНО: Безопасная битовая маска
    if (ControlType[ID] >= 0 && ControlType[ID] < 32) {
        Result |= (1 << (ControlType[ID] + 1));
    }

    if (aBtnState > 0) Result |= (1 << 6);
    if ((mBtnState > 0) || (ManualPWM[ID] == 0)) Result |= (1 << 4);

    return Result;
}

bool RateControl::ProductOn(int ID)
{
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return false;

    bool Result = false;
    if (ControlType[ID] == 4) {
        Result = SensorReceiving[ID];
    } else {
        Result = (SensorReceiving[ID] && HectaresPerMinute > 0);
    }
    return Result;
}

double RateControl::SmoothRate(int ID)
{
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return 0;

    double Result = 0;
    if (ProductOn(ID)) {
        double Ra = RateApplied(ID);
        if (ProdDensity[ID] > 0) Ra *= ProdDensity[ID];

        if (TargetRate[ID] > 0) {
            double Rt = Ra / TargetRate[ID];

            if (Rt >= 0.95 && Rt <= 1.05 && aBtnState > 0) {
                Result = TargetRate[ID];
            } else {
                Result = Ra;
            }
        } else {
            Result = Ra;
        }
    }
    return Result;
}

double RateControl::CurrentRate(int ID)
{
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return 0;

    if (ProductOn(ID)) {
        double V = RateApplied(ID);
        if (ProdDensity[ID] > 0) V *= ProdDensity[ID];
        return V;
    } else {
        return 0;
    }
}

double RateControl::TargetUPM(int ID)
{
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return 0;

    double Result = 0;
    switch (CoverageUnits[ID]) {
    case 0: // acres
        if (AppMode[ID] == 1) {
            // Constant UPM
            double HPM = swidth * cMinUPMSpeed[ID] / 600.0;
            // ИСПРАВЛЕНО: Проверка деления на ноль
            if (HPM > 0.0001) {
                Result = TargetRate[ID] * HPM * 2.47;
            }
        } else {
            // section controlled UPM
            Result = TargetRate[ID] * HectaresPerMinute * 2.47;
        }
        break;

    case 1: // hectares
        if (AppMode[ID] == 1) {
            // Constant UPM
            double HPM = swidth * cMinUPMSpeed[ID] / 600.0;
            if (HPM > 0.0001) {
                Result = TargetRate[ID] * HPM;
            }
        } else {
            // section controlled UPM
            Result = TargetRate[ID] * HectaresPerMinute;
        }
        break;

    case 2: // minutes
        Result = TargetRate[ID];
        break;

    default: // hours
        Result = TargetRate[ID] / 60;
        break;
    }

    if (ProdDensity[ID] > 0) {
        Result /= ProdDensity[ID];
    }
    return Result;
}

double RateControl::RateApplied(int ID)
{
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return 0;

    HectaresPerMinute = width * cMinUPMSpeed[ID] / 600.0;
    double Result = 0;

    switch (CoverageUnits[ID]) {
    case 0: // acres
        if (AppMode[ID] == 1) {
            // Constant UPM
            double HPM = swidth * cMinUPMSpeed[ID] / 600.0;
            // ИСПРАВЛЕНО: Правильное условие и проверка деления на ноль
            if (HPM > 0.0001) {
                Result = appRate[ID] / (HPM * 2.47);
            }
        } else if (AppMode[ID] == 0 || AppMode[ID] == 2) {
            // ИСПРАВЛЕНО: Правильное условие
            if (HectaresPerMinute > 0.0001) {
                Result = appRate[ID] / (HectaresPerMinute * 2.47);
            }
        } else {
            // Document target rate
            Result = TargetRate[ID];
        }
        break;

    case 1: // hectares
        if (AppMode[ID] == 1) {
            double HPM = swidth * cMinUPMSpeed[ID] / 600.0;
            if (HPM > 0.0001) {
                Result = appRate[ID] / HPM;
            }
        } else if (AppMode[ID] == 0 || AppMode[ID] == 2) {
            if (HectaresPerMinute > 0.0001) {
                Result = appRate[ID] / HectaresPerMinute;
            }
        } else {
            Result = TargetRate[ID];
        }
        break;

    case 2: // minutes
        if (AppMode[ID] == 3) {
            Result = TargetRate[ID];
        } else {
            Result = appRate[ID];
        }
        break;

    default: // hours
        if (AppMode[ID] == 3) {
            // ИСПРАВЛЕНО: Используем ID вместо ModID
            Result = TargetRate[ID];
        } else {
            Result = appRate[ID] * 60;
        }
        break;
    }
    return Result;
}

double RateControl::MinUPMSpeed(int ID)
{
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return 0;

    if (speed < minSpeed[ID])
        return 0;
    else
        return speed;
}

double RateControl::MinUPM(int ID)
{
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return 0;

    if (cTargetUPM[ID] != 0 && cTargetUPM[ID] < minUPM[ID])
        return minUPM[ID];
    else
        return cTargetUPM[ID];
}

void RateControl::dataformodule(QVector<int> set_data, QByteArray pgn_data)
{
    if (set_data.isEmpty() || pgn_data.size() < 15) return;

    int ID = set_data[0];
    // ИСПРАВЛЕНО: Проверка границ ID
    if (ID < 0 || ID >= 4) return;

    ProdDensity[ID] = set_data[1];
    OnScreen[ID] = set_data[2];
    pidscale[ID] = set_data[8];
    MeterCal[ID] = set_data[9];
    TargetRate[ID] = set_data[10];
    AppMode[ID] = set_data[11];
    ControlType[ID] = set_data[12];
    CoverageUnits[ID] = set_data[13];
    minSpeed[ID] = set_data[14];

    // ИСПРАВЛЕНО: Безопасное извлечение ModID
    ModID = (pgn_data.size() > 5) ? static_cast<uint8_t>(pgn_data[5]) : 0;
    if (ModID < 0 || ModID >= 4) ModID = 0;

    // ИСПРАВЛЕНО: Безопасное чтение данных
    if (pgn_data.size() >= 14) {
        appRate[ModID] = ((static_cast<qint32>(static_cast<uint8_t>(pgn_data[8]) << 16) +
                           (static_cast<uint8_t>(pgn_data[7]) << 8) +
                           static_cast<uint8_t>(pgn_data[6]))) / 1000.0;

        cQuantity[ModID] = (static_cast<uint8_t>(pgn_data[11]) << 16 |
                            static_cast<uint8_t>(pgn_data[10]) << 8 |
                            static_cast<uint8_t>(pgn_data[9])) / 1000.0;

        PWMsetting[ModID] = static_cast<qint16>(static_cast<uint8_t>(pgn_data[13]) << 8 |
                                                static_cast<uint8_t>(pgn_data[12]));

        SensorReceiving[ModID] = ((static_cast<uint8_t>(pgn_data[14]) & 0b00100000) == 0b00100000);
    }

    qDebug() << "appRate[ModID]:" << cRateApplied[ModID];
    qDebug() << "TargetRate[ModID]:" << TargetRate[ModID];
    qDebug() << "SmoothRate:" << cSmoothRate[ModID];
}
void RateControl::onRateControlDataReady(const PGNParser::ParsedData &data)
{

}
