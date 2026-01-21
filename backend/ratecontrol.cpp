// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// Rate control source code used from https://github.com/SK21/AOG_RC
#include "ratecontrol.h"
#include <QDebug>
#include "agioservice.h"
#include "pgnparser.h"
#include "cpgn.h"
#include "modulecomm.h"

Q_LOGGING_CATEGORY (rc_log, "backend.qtagopengps")

RateControl *RateControl::s_instance = nullptr;
QMutex RateControl::s_mutex;
bool RateControl::s_cpp_created = false;

// ИСПРАВЛЕНО: Добавлена полная инициализация всех членов класса
RateControl::RateControl(QObject *parent)
    : QObject{parent},
    m_currentProductId(0),  // Инициализируем явно
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
    // Сначала создаем модель
    m_rcModel = new RCModel(this);

    // Инициализация всех массивов
    for (int i = 0; i < 4; i++) {
        ManualPWM[i] = 0;
        Quantity[i] = 0;
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

    // Создаем начальные продукты в модели
    for (int i = 0; i < 4; i++) {
        RCModel::Product product;
        product.id = i;
        product.name = QString("Product %1").arg(i + 1);
        product.setRate = 0;
        product.smoothRate = 0;
        product.actualRate = 0;
        product.isActive = false;
        m_rcModel->addProduct(product);
    }

    // Подключаем к AgIOService
    connect(AgIOService::instance(), &AgIOService::rateControlDataReady,
            this, &RateControl::onRateControlDataReady, Qt::DirectConnection);
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

void RateControl::loadSettings(int ID)
{
    if (ID < 0 || ID >= 4) return;

    QVector<int> rateSettings;

    switch (ID) {
    case 0:
        rateSettings = SettingsManager::instance()->rate_confProduct0();
        break; // Добавить break!
    case 1:
        rateSettings = SettingsManager::instance()->rate_confProduct1();
        break;
    case 2:
        rateSettings = SettingsManager::instance()->rate_confProduct2();
        break;
    case 3:
        rateSettings = SettingsManager::instance()->rate_confProduct3();
        break;
    }

    ProdDensity[ID] = rateSettings[1];
    OnScreen[ID] = rateSettings[2];
    pidscale[ID] = rateSettings[8];
    MeterCal[ID] = rateSettings[9];
    TargetRate[ID] = rateSettings[10];
    AppMode[ID] = rateSettings[11];
    ControlType[ID] = rateSettings[12];
    CoverageUnits[ID] = rateSettings[13];
    minSpeed[ID] = rateSettings[14];

}
void RateControl::onRateControlDataReady(const PGNParser::ParsedData &data)
{
    // Update data from Blockage modules

    if (!data.isValid) return;

    // PGN 240: RC Data
    if (data.pgnNumber == 240) {

        ModID = data.rateControlInData[0]; // ID из data[ ]

        // Проверяем, что ID в допустимом диапазоне (0-3)
        if (ModID < 4) {
            appRate[ModID] = data.rateControlInData[1];
            Quantity[ModID] = data.rateControlInData[2];
            PWMsetting[ModID] = data.rateControlInData[3];
            SensorReceiving[ModID] = data.rateControlInData[4];
        }
        loadSettings(ModID);
        modulesSend241(ModID);
        updateModel(ModID);
    }
}

void RateControl::modulesSend241(int ID)
{
    CPGN_F1 &p_241 = ModuleComm::instance()->p_241;
    p_241.pgn[CPGN_F1::ID] = ID;
    p_241.pgn[CPGN_F1::RateSetLo] = (char)((int)cMinUPM[ID]); // target rate
    p_241.pgn[CPGN_F1::RateSetHI] = (char)((int)cMinUPM[ID] >> 8);
    p_241.pgn[CPGN_F1::FlowCalLO] = (char)((int)MeterCal[ID]);
    p_241.pgn[CPGN_F1::FlowCalHI] = (char)((int)MeterCal[ID] >> 8);
    p_241.pgn[CPGN_F1::Command] = Command(ID);
    p_241.pgn[CPGN_F1::ManualPWM] = (char)((int)ManualPWM[ID]);

    emit ModuleComm::instance()->p_241_changed();
}
void RateControl::updateModel(int id)
{
    if (id < 0 || id >= 4) return;

    // Обновляем модель из массивов
    if (m_rcModel->productExists(id)) {
        // Получаем данные из Settings для имени
        QString productName = "";
        switch (id) {
        case 0: productName = SettingsManager::instance()->rate_productName0(); break;
        case 1: productName = SettingsManager::instance()->rate_productName1(); break;
        case 2: productName = SettingsManager::instance()->rate_productName2(); break;
        case 3: productName = SettingsManager::instance()->rate_productName3(); break;
        }

        // Обновляем модель
        m_rcModel->updateName(id, productName);
        m_rcModel->updateSetRate(id, TargetRate[id]);
        m_rcModel->updateSmoothRate(id, cSmoothRate[id]);
        m_rcModel->updateActualRate(id, cCurrentRate[id]);
        bool isActive = ProductOn(id) && OnScreen[id];
        m_rcModel->updateIsActive(id, isActive);
    }
}
void RateControl::setCurrentProductId(int id)
{
    if (id >= 0 && id < 4 && m_currentProductId != id) {
        m_currentProductId = id;
        emit currentProductIdChanged(id);
        updateModel(id);
    }
}

QVariantMap RateControl::getProductData(int id) const
{
    if (!m_rcModel || id < 0 || id >= 4)
        return QVariantMap();

    return m_rcModel->getProductById(id);
}
void RateControl::increaseSetRate(int id, double step)
{
    if (m_rcModel && id >= 0 && id < 4) {
        TargetRate[id] = TargetRate[id] + step;

        m_rcModel->updateSetRate(id, TargetRate[id]);
        // Обновляем данные после изменения
        updateModel(id+1);
    }
}

void RateControl::decreaseSetRate(int id, double step)
{
    if (m_rcModel && id >= 0 && id < 4) {
        TargetRate[id] = TargetRate[id] - step;
        m_rcModel->updateSetRate(id, TargetRate[id]);
        // Обновляем данные после изменения
        updateModel(id+1);
    }
}
