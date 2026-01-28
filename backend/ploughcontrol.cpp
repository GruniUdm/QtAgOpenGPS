#include "ploughcontrol.h"
#include <QTimer>
#include <QDebug>
#include <QQmlEngine>
#include <QJSEngine>
#include <QMutexLocker>

// Статические члены
PloughControl *PloughControl::s_instance = nullptr;
QMutex PloughControl::s_mutex;
bool PloughControl::s_cpp_created = false;

PloughControl::PloughControl(QObject *parent)
    : QObject(parent)
    , m_ploughModel(new PloughModel(this))
    , m_settings("YourCompany", "AgOpenGPS_Plough")
{
    // Загружаем настройки
    loadSettings();

    // Таймер для периодических операций (симуляция получения данных)
    m_updateTimer.setInterval(1000);
    connect(&m_updateTimer, &QTimer::timeout, this, [this]() {
        // В реальной системе здесь будет запрос данных по протоколу
        if (m_ploughEnabled && m_autoWidthEnabled) {
            int error = m_currentWidth - m_targetWidth;
            if (abs(error) > m_deadBand) {
                m_currentWidth += (error > 0) ? -5 : 5;
                m_currentWidth = qBound(m_minWidth, m_currentWidth, m_maxWidth);

                // Обновляем модель с индексом 0
                m_ploughModel->updateCurrentWidth(0, m_currentWidth);

                // Обновляем ошибку
                m_error = m_currentWidth - m_targetWidth;
                m_ploughModel->updateError(0, m_error);

                emit currentWidthChanged(m_currentWidth);
                emit errorChanged(m_error);
                updatePloughMode();
            }
        }
    });
    m_updateTimer.start();

    // Таймер для автосохранения настроек
    m_settingsSaveTimer.setInterval(5000); // Каждые 5 секунд
    connect(&m_settingsSaveTimer, &QTimer::timeout, this, [this]() {
        if (m_settingsDirty) {
            saveSettings();
            m_settingsDirty = false;
        }
    });
    m_settingsSaveTimer.start();
}

PloughControl *PloughControl::instance()
{
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new PloughControl();
        s_cpp_created = true;
    }
    return s_instance;
}

PloughControl *PloughControl::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new PloughControl();
        s_cpp_created = true;
    }

    // Передаем владение QML, если создано из QML
    if (!s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}

// Геттеры свойств
int PloughControl::currentWidth() const { return m_currentWidth; }
int PloughControl::targetWidth() const { return m_targetWidth; }
int PloughControl::ploughMode() const { return m_ploughMode; }
bool PloughControl::ploughEnabled() const { return m_ploughEnabled; }
bool PloughControl::autoWidthEnabled() const { return m_autoWidthEnabled; }
bool PloughControl::autoLiftEnabled() const { return m_autoLiftEnabled; }
int PloughControl::calibrationValue() const { return m_calibrationValue; }
int PloughControl::deadBand() const { return m_deadBand; }
int PloughControl::minWidth() const { return m_minWidth; }
int PloughControl::maxWidth() const { return m_maxWidth; }
bool PloughControl::isCalibrated() const { return m_isCalibrated; }
bool PloughControl::sectionOn() const { return m_sectionOn; }
int PloughControl::lineDistance() const { return m_lineDistance; }
int PloughControl::error() const { return m_error; }

QString PloughControl::ploughModeName() const
{
    return modeToString(m_ploughMode);
}

QString PloughControl::modeToString(int mode) const
{
    switch (mode) {
    case 0: return tr("Секция Выкл");
    case 1: return tr("Автоконфигурация Выкл");
    case 2: return tr("Автопереключение Выкл");
    case 3: return tr("Фиксированный");
    case 4: return tr("Расширение");
    case 5: return tr("Максимум");
    case 6: return tr("Сужение");
    case 7: return tr("Минимум");
    case 8: return tr("Нет линии");
    default: return tr("Неизвестно");
    }
}

// Q_INVOKABLE методы управления
void PloughControl::calibrateMin()
{
    if (m_calibrationValue > 0 && !m_calibrationInProgress) {
        m_calibrationInProgress = true;

        // Отправляем команду калибровки
        emit sendCalibrationCommand(111, m_calibrationValue);

        // Сохраняем калибровку
        m_minWidth = m_calibrationValue;
        m_isCalibrated = (m_minWidth > 0 && m_maxWidth > m_minWidth);

        // Обновляем модель с индексом 0
        m_ploughModel->updateCalibration(0, m_isCalibrated, m_minWidth, m_maxWidth);

        // Помечаем настройки как измененные
        m_settingsDirty = true;

        // Сигнализируем об изменениях
        emit minWidthChanged(m_minWidth);
        emit calibrationChanged();

        m_calibrationInProgress = false;
    }
}

void PloughControl::calibrateMax()
{
    if (m_calibrationValue > 0 && !m_calibrationInProgress) {
        m_calibrationInProgress = true;

        // Отправляем команду калибровки
        emit sendCalibrationCommand(222, m_calibrationValue);

        // Сохраняем калибровку
        m_maxWidth = m_calibrationValue;
        m_isCalibrated = (m_minWidth > 0 && m_maxWidth > m_minWidth);

        // Обновляем модель с индексом 0
        m_ploughModel->updateCalibration(0, m_isCalibrated, m_minWidth, m_maxWidth);

        // Помечаем настройки как измененные
        m_settingsDirty = true;

        // Сигнализируем об изменениях
        emit maxWidthChanged(m_maxWidth);
        emit calibrationChanged();

        m_calibrationInProgress = false;
    }
}

void PloughControl::saveSettings()
{
    m_settings.beginGroup("Plough");
    m_settings.setValue("targetWidth", m_targetWidth);
    m_settings.setValue("ploughEnabled", m_ploughEnabled);
    m_settings.setValue("autoWidthEnabled", m_autoWidthEnabled);
    m_settings.setValue("autoLiftEnabled", m_autoLiftEnabled);
    m_settings.setValue("deadBand", m_deadBand);
    m_settings.setValue("minWidth", m_minWidth);
    m_settings.setValue("maxWidth", m_maxWidth);
    m_settings.setValue("isCalibrated", m_isCalibrated);
    m_settings.endGroup();

    m_settings.sync();

    // Отправляем настройки на устройство
    sendCurrentSettings();
}

void PloughControl::loadSettings()
{
    m_settings.beginGroup("Plough");
    m_targetWidth = m_settings.value("targetWidth", 200).toInt();
    m_ploughEnabled = m_settings.value("ploughEnabled", false).toBool();
    m_autoWidthEnabled = m_settings.value("autoWidthEnabled", false).toBool();
    m_autoLiftEnabled = m_settings.value("autoLiftEnabled", false).toBool();
    m_deadBand = m_settings.value("deadBand", 10).toInt();
    m_minWidth = m_settings.value("minWidth", 0).toInt();
    m_maxWidth = m_settings.value("maxWidth", 400).toInt();
    m_isCalibrated = m_settings.value("isCalibrated", false).toBool();
    m_settings.endGroup();

    // Обновляем модель из загруженных настроек
    updateModelFromSettings();
}

void PloughControl::resetCalibration()
{
    m_minWidth = 0;
    m_maxWidth = 400;
    m_isCalibrated = false;

    // Обновляем модель с индексом 0
    m_ploughModel->updateCalibration(0, false, m_minWidth, m_maxWidth);

    emit minWidthChanged(m_minWidth);
    emit maxWidthChanged(m_maxWidth);
    emit calibrationChanged();

    m_settingsDirty = true;
}

void PloughControl::setTargetWidth(int width)
{
    if (width >= m_minWidth && width <= m_maxWidth && m_targetWidth != width) {
        m_targetWidth = width;

        // Обновляем модель с индексом 0
        m_ploughModel->updateTargetWidth(0, width);

        // Обновляем ошибку
        m_error = m_currentWidth - m_targetWidth;
        m_ploughModel->updateError(0, m_error);

        emit targetWidthChanged(width);
        emit errorChanged(m_error);
        updatePloughMode();

        m_settingsDirty = true;
    }
}

void PloughControl::setPloughEnabled(bool enabled)
{
    if (m_ploughEnabled != enabled) {
        m_ploughEnabled = enabled;

        // Обновляем модель с индексом 0
        m_ploughModel->updatePloughEnabled(0, enabled);

        emit ploughEnabledChanged(enabled);
        updatePloughMode();

        m_settingsDirty = true;
    }
}

void PloughControl::setAutoWidthEnabled(bool enabled)
{
    if (m_autoWidthEnabled != enabled) {
        m_autoWidthEnabled = enabled;

        // Обновляем модель с индексом 0
        m_ploughModel->updateAutoWidthEnabled(0, enabled);

        emit autoWidthEnabledChanged(enabled);
        updatePloughMode();

        m_settingsDirty = true;
    }
}

void PloughControl::setAutoLiftEnabled(bool enabled)
{
    if (m_autoLiftEnabled != enabled) {
        m_autoLiftEnabled = enabled;

        // Обновляем модель с индексом 0
        m_ploughModel->updateAutoLiftEnabled(0, enabled);

        emit autoLiftEnabledChanged(enabled);
        updatePloughMode();

        m_settingsDirty = true;
    }
}

void PloughControl::setDeadBand(int deadBand)
{
    if (deadBand >= 0 && m_deadBand != deadBand) {
        m_deadBand = deadBand;

        // Обновляем модель с индексом 0
        m_ploughModel->updateDeadBand(0, deadBand);

        emit deadBandChanged(deadBand);
        updatePloughMode();

        m_settingsDirty = true;
    }
}

void PloughControl::setCalibrationValue(int value)
{
    if (value >= 0 && m_calibrationValue != value) {
        m_calibrationValue = value;
        emit calibrationValueChanged(value);
    }
}

void PloughControl::setSectionOn(bool sectionOn)
{
    if (m_sectionOn != sectionOn) {
        m_sectionOn = sectionOn;

        // Обновляем модель с индексом 0
        m_ploughModel->updateSectionState(0, sectionOn);

        emit sectionOnChanged(sectionOn);
        updatePloughMode();
    }
}

// Методы для обновления данных из протокола
void PloughControl::updatePloughData(int width, int mode)
{
    if (m_currentWidth != width) {
        m_currentWidth = width;

        // Обновляем модель с индексом 0
        m_ploughModel->updateCurrentWidth(0, width);

        // Обновляем ошибку
        m_error = m_currentWidth - m_targetWidth;
        m_ploughModel->updateError(0, m_error);

        emit currentWidthChanged(width);
        emit errorChanged(m_error);
    }

    if (m_ploughMode != mode) {
        m_ploughMode = mode;

        // Обновляем модель с индексом 0
        m_ploughModel->updatePloughMode(0, mode);

        emit ploughModeChanged(mode);
    }

    updatePloughMode();
    emit ploughDataUpdated();
}

void PloughControl::updateLineDistance(int distance)
{
    m_lineDistance = distance;

    // Обновляем модель с индексом 0
    m_ploughModel->updateLineDistance(0, distance);

    emit lineDistanceChanged(distance);
    updatePloughMode();
}

void PloughControl::updateSectionState(bool sectionOn)
{
    if (m_sectionOn != sectionOn) {
        m_sectionOn = sectionOn;

        // Обновляем модель с индексом 0
        m_ploughModel->updateSectionState(0, sectionOn);

        emit sectionOnChanged(sectionOn);
        updatePloughMode();
    }
}

// Обработка входящих данных по протоколу
void PloughControl::onPloughDataReady(const PGNParser::ParsedData& data)
{
    QMutexLocker locker(&mutex);

    // // Обработка данных плуга (PGN 0xED из Arduino скетча)
    // if (data.pgn == 0xED) {
    //     int width = (data.data[6] << 8) + data.data[5];
    //     int mode = data.data[7];

    //     updatePloughData(width, mode);
    // }
}

void PloughControl::onMachineSettingsReady(const PGNParser::ParsedData& data)
{
    QMutexLocker locker(&mutex);

    // Обработка настроек машины (PGN 0xEE из Arduino скетча)
    // if (data.pgn == 0xEE) {
    //     // Здесь можно обработать подтверждение калибровки или других настроек
    //     // Например, если устройство подтвердило получение калибровки
    // }
}

// Вспомогательные методы
void PloughControl::initializeSettings()
{
    // Инициализация настроек по умолчанию
    if (!m_settings.contains("Plough/targetWidth")) {
        saveSettings();
    }
}

void PloughControl::updatePloughMode()
{
    int newMode = 0; // ModeOff

    if (!m_sectionOn) {
        newMode = 0; // ModeOff
    }
    else if (!m_autoLiftEnabled) {
        newMode = 1; // ModeAutoConfigOff
    }
    else if (!m_autoWidthEnabled) {
        newMode = 2; // ModeAutoSwitchOff
    }
    else if (m_lineDistance > 30000) {
        newMode = 8; // ModeNoLine
    }
    else {
        int error = m_currentWidth - m_targetWidth;

        if (error < -m_deadBand) {
            newMode = (m_currentWidth >= m_maxWidth) ? 5 : 4; // ModeMax : ModeWider
        }
        else if (error > m_deadBand) {
            newMode = (m_currentWidth <= m_minWidth) ? 7 : 6; // ModeMin : ModeNarrower
        }
        else {
            newMode = 3; // ModeFixed
        }
    }

    if (m_ploughMode != newMode) {
        m_ploughMode = newMode;

        // Обновляем модель с индексом 0
        m_ploughModel->updatePloughMode(0, newMode);

        emit ploughModeChanged(newMode);
    }
}

void PloughControl::sendCurrentSettings()
{
    // Формируем и отправляем настройки на устройство
    // В реальной системе здесь будет формирование PGN пакета
    QByteArray settings;
    // ... заполнение settings ...
    emit sendSettingsCommand(settings);
}

void PloughControl::updateModelFromSettings()
{
    // Обновляем все поля модели с индексом 0
    m_ploughModel->updateTargetWidth(0, m_targetWidth);
    m_ploughModel->updatePloughEnabled(0, m_ploughEnabled);
    m_ploughModel->updateAutoWidthEnabled(0, m_autoWidthEnabled);
    m_ploughModel->updateAutoLiftEnabled(0, m_autoLiftEnabled);
    m_ploughModel->updateDeadBand(0, m_deadBand);
    m_ploughModel->updateCalibration(0, m_isCalibrated, m_minWidth, m_maxWidth);

    // Обновляем текущий режим
    m_ploughModel->updatePloughMode(0, m_ploughMode);

    // Обновляем текущую ширину и ошибку
    m_ploughModel->updateCurrentWidth(0, m_currentWidth);
    m_ploughModel->updateError(0, m_currentWidth - m_targetWidth);

    // Обновляем состояние секции
    m_ploughModel->updateSectionState(0, m_sectionOn);

    // Обновляем расстояние до линии
    m_ploughModel->updateLineDistance(0, m_lineDistance);
}
