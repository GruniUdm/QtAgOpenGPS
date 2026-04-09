#include "steerconfig.h"
#include <QCoreApplication>
#include <QLoggingCategory>
#include "glm.h"
#include "cvehicle.h"
#include "backend.h"
#include "mainwindowstate.h"
#include "settingsmanager.h"
#include "modulecomm.h"

Q_LOGGING_CATEGORY (steerconfig_log, "steerconfig.qtagopengps")
#define QDEBUG qDebug(steerconfig_log)

SteerConfig *SteerConfig::s_instance = nullptr;
QMutex SteerConfig::s_mutex;
bool SteerConfig::s_cpp_created = false;

SteerConfig::SteerConfig(QObject *parent)
    : QObject{parent}
{
    connect(&timer, &QTimer::timeout, this, &SteerConfig::on_timer);
    timer.setSingleShot(false);
    timer.start(250);
}

SteerConfig *SteerConfig::instance() {
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new SteerConfig();
        qDebug(steerconfig_log) << "SteerConfig singleton created by C++ code.";
        s_cpp_created = true;
        // ensure cleanup on app exit
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                             delete s_instance; s_instance = nullptr;
                         });
    }
    return s_instance;
}

SteerConfig *SteerConfig::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if(!s_instance) {
        s_instance = new SteerConfig();
        qDebug(steerconfig_log) << "SteerConfig singleton created by QML engine.";
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}

void SteerConfig::startSA() {
    qDebug(steerconfig_log) << "Starting SA";

    m_isSA = true;
    startFix = CVehicle::instance()->pivotAxlePos;
    dist = 0;
    m_diameter = 0;
    cntr = 0;
    m_haveSteerAngle = false; //probably not needed

    /*
    //lblDiameter.Text = "0";
    setLblCalcSteerAngleInner("Drive Steady");
    // DEAD CODE from C# original - lblCalcSteerAngleOuter never displayed (FormSteer.cs:848 commented)
    lblCalcSteerAngleOuter = "Consistent Steering Angle!!";
    */
}

void SteerConfig::stopSA(){
        m_isSA = false;
}

void SteerConfig::startSALeft() {
    qDebug(steerconfig_log) << "Starting SA Left (Ackermann)";

    m_isSALeft = true;
    startFixLeft = CVehicle::instance()->pivotAxlePos;
    distLeft = 0;
    m_diameterLeft = 0;
    cntrLeft = 0;
}

void SteerConfig::stopSALeft(){
        m_isSALeft = false;
}

void SteerConfig::startAutoTune() {
    qDebug(steerconfig_log) << "Starting Auto Tune";

    auto *settings = SettingsManager::instance();

    m_isAutoTuning = true;
    set_currentTestKp(1);
    set_kuValue(0);
    set_oscillationDetected(false);
    prevError = 0;
    oscillationCount = 0;
    accumulatedError = 0;
    errorSampleCount = 0;

    settings->setAs_Kp(1);
}

void SteerConfig::stopAutoTune() {
    auto *settings = SettingsManager::instance();

    m_isAutoTuning = false;
    int finalKp = settings->as_Kp();
    set_currentTestKp(finalKp);
    qDebug(steerconfig_log) << "Auto Tune stopped, Kp set to:" << finalKp;
}

void SteerConfig::startPwmAutoTune() {
    qDebug(steerconfig_log) << "Starting PWM Auto Tune";

    m_isPwmAutoTuning = true;
    set_pwmTestValue(0);
    set_pwmDirection(1);
    set_pwmTunePhase(1);
    set_pwmMinAngle(0);
    set_pwmMaxAngle(0);
    set_foundMinPwmLeft(0);
    set_foundMinPwmRight(0);
    set_foundMaxPwmLeft(0);
    set_foundMaxPwmRight(0);
    set_maxAngleLeft(0);
    set_maxAngleRight(0);

    prevPwmAngle = 0;
    prevPwmAcceleration = 0;
    pwmSamplesTaken = 0;
    minPwmFound = false;
    maxPwmFound = false;
    leftMaxAngle = 0;
    rightMaxAngle = 0;
    currentMaxAngle = 0;
}

void SteerConfig::stopPwmAutoTune() {
    qDebug(steerconfig_log) << "PWM Auto Tune stopped";

    m_isPwmAutoTuning = false;
}

void SteerConfig::on_timer() {
    auto *vehicle = CVehicle::instance();
    auto *settings = SettingsManager::instance();

    if (m_isSA)
    {
        dist = glm::Distance(startFix, vehicle->pivotAxlePos);
        cntr++;
        if (dist > m_diameter)
        {
            m_diameter = dist;
            cntr = 0;
        }
        if (cntr > 9)
        {
            double steerAngleRight = atan(vehicle->wheelbase / ((m_diameter - vehicle->trackWidth * 0.5) / 2));
            m_calcSteerAngleInner = glm::toDegrees(steerAngleRight);

            // Calculate CPD: actualAngle / calculatedAngle * currentCPD * 0.9
            double actualAngle = ModuleComm::instance()->actualSteerAngleDegrees();
            if (steerAngleRight > 0.001) {
                double cpd = (actualAngle / m_calcSteerAngleInner) * settings->as_countsPerDegree() * 0.9;
                int newCPD = static_cast<int>(std::round(cpd));
                newCPD = std::clamp(newCPD, 1, 255);
                settings->setAs_countsPerDegree(newCPD);
                qDebug(steerconfig_log) << "CPD calculated:" << newCPD;
            }

            m_isSA = false;
        }
    }

    // Ackermann measurement (drive left circles)
    if (m_isSALeft)
    {
        distLeft = glm::Distance(startFixLeft, vehicle->pivotAxlePos);
        cntrLeft++;
        if (distLeft > m_diameterLeft)
        {
            m_diameterLeft = distLeft;
            cntrLeft = 0;
        }
        if (cntrLeft > 9)
        {
            double steerAngleLeft = atan(vehicle->wheelbase / ((m_diameterLeft - vehicle->trackWidth * 0.5) / 2));
            m_calcSteerAngleLeft = glm::toDegrees(steerAngleLeft);

            // Calculate Ackermann: (calculatedAngle / |startAngle|) * 100
            // startAngleLeft is stored when we started the measurement
            double startAngleLeft = ModuleComm::instance()->actualSteerAngleDegrees();
            if (std::abs(startAngleLeft) > 0.1) {
                int ackerman = static_cast<int>((m_calcSteerAngleLeft / std::abs(startAngleLeft)) * 100);
                ackerman = std::clamp(ackerman, 1, 200);
                settings->setAs_ackerman(ackerman);
                qDebug(steerconfig_log) << "Ackermann calculated:" << ackerman;
            }

            m_isSALeft = false;
        }
    }

    // Auto-tune Ziegler-Nichols method
    if (m_isAutoTuning) {
        double setAngle = vehicle->driveFreeSteerAngle();
        double actualAngle = ModuleComm::instance()->actualSteerAngleDegrees();
        double error = std::abs(setAngle - actualAngle);

        set_currentError(error);
        errorSampleCount++;

        // Use local variable for minError since m_minError() doesn't work
        static double localMinError = 999.0;
        if (error < localMinError) {
            localMinError = error;
        }

        // Check for oscillation (sign change in error derivative)
        if (prevError != 0 && error > localMinError * 1.5) {
            oscillationCount++;
        }
        prevError = error;

        // Every 50 samples, adjust Kp
        if (errorSampleCount >= 50) {
            double avgError = accumulatedError / errorSampleCount;

            // Use local variables for Ku and test Kp
            static int localTestKp = 1;
            static double localKu = 0;

            if (oscillationCount >= 3) {
                // Oscillation detected - this is Ku
                localKu = localTestKp;
                set_kuValue(localKu);
                set_oscillationDetected(true);

                // Calculate optimal Kp = 0.5 * Ku
                int optimalKp = static_cast<int>(std::round(localKu * 0.5));
                optimalKp = std::clamp(optimalKp, 1, 200);
                settings->setAs_Kp(optimalKp);

                qDebug(steerconfig_log) << "Oscillation detected! Ku =" << localKu << ", Optimal Kp =" << optimalKp;
                m_isAutoTuning = false;
                localMinError = 999.0;
            } else if (localTestKp >= 200) {
                // Max Kp reached, use last value
                qDebug(steerconfig_log) << "Max Kp reached, stopping auto-tune";
                m_isAutoTuning = false;
                localMinError = 999.0;
            } else {
                // Increase Kp for next test
                localTestKp += 5;
                set_currentTestKp(localTestKp);
                settings->setAs_Kp(localTestKp);
                oscillationCount = 0;
                localMinError = 999.0;
            }

            accumulatedError = 0;
            errorSampleCount = 0;
        } else {
            accumulatedError += error;
        }
    }

    // PWM Auto-tune
    if (m_isPwmAutoTuning) {
        auto *modComm = ModuleComm::instance();
        double currentAngle = modComm->actualSteerAngleDegrees();
        int currentPwm = modComm->pwmDisplay();

        // Use local variables since m_* properties don't work with ()
        static int localPwmDirection = 1;
        static int localPwmTunePhase = 1;
        static int localMinPwmLeft = 0, localMinPwmRight = 0;
        static int localMaxPwmLeft = 0, localMaxPwmRight = 0;
        static double localMaxAngleLeft = 0, localMaxAngleRight = 0;
        static bool localMinFound = false;
        static bool localMaxFound = false;

        int phase = localPwmTunePhase;

        // Phase 1: Find minPwm (turning starts)
        if (phase == 1) {
            set_pwmTestValue(currentPwm);

            // Track max angle for current direction
            if (localPwmDirection > 0 && currentAngle > currentMaxAngle) {
                currentMaxAngle = currentAngle;
            } else if (localPwmDirection < 0 && currentAngle < currentMaxAngle) {
                currentMaxAngle = currentAngle;
            }

            // Detect when wheel starts turning (angle > 0.5 degrees)
            if (std::abs(currentAngle) > 0.5 && !localMinFound) {
                if (localPwmDirection > 0) {
                    localMinPwmRight = currentPwm;
                    set_foundMinPwmRight(currentPwm);
                } else {
                    localMinPwmLeft = std::abs(currentPwm);
                    set_foundMinPwmLeft(std::abs(currentPwm));
                }
                localMinFound = true;

                // Switch to finding maxPwm
                localPwmDirection = -localPwmDirection;
                set_pwmDirection(localPwmDirection);
                localPwmTunePhase = 2;
                set_pwmTunePhase(2);
                pwmSamplesTaken = 0;
                currentMaxAngle = 0;
                qDebug(steerconfig_log) << "Min PWM found:" << currentPwm << ", switching to max";
            }
        }
        // Phase 2: Find maxPwm (acceleration stops increasing)
        else if (phase == 2) {
            // Track max angle for current direction
            if (localPwmDirection > 0 && currentAngle > currentMaxAngle) {
                currentMaxAngle = currentAngle;
            } else if (localPwmDirection < 0 && currentAngle < currentMaxAngle) {
                currentMaxAngle = currentAngle;
            }

            // Calculate acceleration (change in angle)
            double acceleration = 0;
            if (pwmSamplesTaken > 0) {
                acceleration = (currentAngle - prevPwmAngle);
            }
            prevPwmAngle = currentAngle;
            pwmSamplesTaken++;

            // Check for diminishing returns (acceleration dropping)
            if (pwmSamplesTaken > 10 && std::abs(acceleration) < std::abs(prevPwmAcceleration) * 0.5) {
                if (localPwmDirection > 0) {
                    localMaxPwmRight = currentPwm;
                    localMaxAngleRight = std::abs(currentMaxAngle);
                    set_foundMaxPwmRight(currentPwm);
                    set_maxAngleRight(localMaxAngleRight);
                } else {
                    localMaxPwmLeft = std::abs(currentPwm);
                    localMaxAngleLeft = std::abs(currentMaxAngle);
                    set_foundMaxPwmLeft(std::abs(currentPwm));
                    set_maxAngleLeft(localMaxAngleLeft);
                }
                localMaxFound = true;

                qDebug(steerconfig_log) << "Max PWM found:" << currentPwm << ", angle:" << currentMaxAngle;

                // Check if both directions done
                if (localPwmDirection < 0 && localMaxFound) {
                    // Done - calculate final values
                    int minPwm = std::max(localMinPwmLeft, localMinPwmRight);
                    int maxPwm = std::min(localMaxPwmLeft, localMaxPwmRight);
                    double maxAngle = std::min(localMaxAngleLeft, localMaxAngleRight);

                    settings->setAs_minSteerPWM(minPwm);
                    settings->setAs_highSteerPWM(maxPwm);

                    // Calculate max steer angle from PWM values
                    double currentMaxSteer = settings->vehicle_maxSteerAngle();
                    if (maxAngle > 0 && maxAngle < currentMaxSteer) {
                        settings->setVehicle_maxSteerAngle(static_cast<int>(std::round(maxAngle)));
                    }

                    qDebug(steerconfig_log) << "PWM Auto-tune complete: minPwm=" << minPwm << ", maxPwm=" << maxPwm << ", maxAngle=" << maxAngle;
                    m_isPwmAutoTuning = false;

                    // Reset local variables
                    localPwmTunePhase = 1;
                    localPwmDirection = 1;
                    localMinFound = false;
                    localMaxFound = false;
                } else if (localPwmDirection > 0 && localMinFound) {
                    // Switch to left direction
                    localPwmDirection = -1;
                    set_pwmDirection(localPwmDirection);
                    pwmSamplesTaken = 0;
                    currentMaxAngle = 0;
                    localMinFound = false;
                    localMaxFound = false;
                }
            }
            prevPwmAcceleration = acceleration;
        }

        set_pwmMinAngle(currentAngle);
        set_pwmMaxAngle(currentMaxAngle);
    }
}

// Начало сбора данных
void SteerConfig::startDataCollection()
{
    isCollectingData = true;
    lastCollectionTime = QDateTime::currentDateTime();
    QDEBUG<< "StartDataCollection";
}

// Завершение сбора данных
void SteerConfig::stopDataCollection()
{
    isCollectingData = false;
    QDEBUG<<"StopDataCollection";
}

// Полностью сбрасываем историю и аналитику
void SteerConfig::resetData()
{
    steerAngleHistory.clear();
    m_sampleCount = 0;
    recommendedWASZero = 0;
    m_confidenceLevel = 0;
    m_hasValidRecommendation = false;
    mean = 0;
    standardDeviation = 0;
    median = 0;
}

// Применяем смещение к историческим данным
void SteerConfig::applyOffsetToCollectedData(double appliedOffsetDegrees)
{
    if (steerAngleHistory.empty()) return;

    for (size_t i = 0; i < steerAngleHistory.size(); ++i)
    {
        steerAngleHistory[i] += appliedOffsetDegrees;
    }

    if (m_sampleCount >= MIN_SAMPLES_FOR_ANALYSIS)
    {
        PerformStatisticalAnalysis();
    }

    QDEBUG << "Smart WAS: Applied " << appliedOffsetDegrees << "° offset to "
             << steerAngleHistory.size() << " collected samples.";
}

// Добавляем новую запись угла направления
void SteerConfig::AddSteerAngleSample(double guidanceSteerAngle, double currentSpeed)
{   //QDEBUG<<"AddSteerAngleSample";
    if (!isCollectingData || !ShouldCollectSample(guidanceSteerAngle, currentSpeed))
        return;

    steerAngleHistory.push_back(guidanceSteerAngle);
    lastCollectionTime = QDateTime::currentDateTime();

    if (steerAngleHistory.size() > MAX_SAMPLES)
    {
        steerAngleHistory.pop_front();  // удаляем самый старый элемент
    }

    m_sampleCount = steerAngleHistory.size();

    if (m_sampleCount >= MIN_SAMPLES_FOR_ANALYSIS)
    {
        PerformStatisticalAnalysis();
    }

}

// Возвращаем поправочный коэффициент на основе текущих данных
int SteerConfig::getRecommendedWASOffsetAdjustment(int currentCPD)
{
    if (!m_hasValidRecommendation) return 0;

    return static_cast<int>(std::round(recommendedWASZero * currentCPD));
}

// Проверяем подходит ли данный образец для сбора
bool SteerConfig::ShouldCollectSample(double steerAngle, double speed)
{
    if (speed < MIN_SPEED_THRESHOLD) return false;
    if (std::abs(steerAngle) > MAX_ANGLE_THRESHOLD) return false;
    if (!MainWindowState::instance()->isBtnAutoSteerOn()) return false;
    if (std::abs(CVehicle::instance()->guidanceLineDistanceOff()) > 15000) return false;

    return true;
}

// Основная процедура статистического анализа
void SteerConfig::PerformStatisticalAnalysis()
{
    if (steerAngleHistory.size() < MIN_SAMPLES_FOR_ANALYSIS) return;

    auto sortedData = steerAngleHistory;
    std::sort(sortedData.begin(), sortedData.end()); // сортируем массив

    mean = std::accumulate(steerAngleHistory.begin(), steerAngleHistory.end(), 0.0) /
           steerAngleHistory.size();

    median = CalculateMedian(sortedData);
    standardDeviation = CalculateStandardDeviation(steerAngleHistory, mean);

    recommendedWASZero = -median; // отрицательная коррекция приближает к центру

    CalculateConfidenceLevel(sortedData);

    m_hasValidRecommendation = m_confidenceLevel > 50.0 &&
                             m_sampleCount >= MIN_SAMPLES_FOR_ANALYSIS;
    //QDEBUG<<"m_hasValidRecommendation"<<m_hasValidRecommendation;
}

// Функция для нахождения медианы
double SteerConfig::CalculateMedian(QVector<double> sortedData)
{
    int count = sortedData.size();
    if (count == 0) return 0;

    if (count % 2 == 0)
    {
        return (sortedData[count / 2 - 1] + sortedData[count / 2]) / 2.0;
    }
    else
    {
        return sortedData[count / 2];
    }
}

// Расчет стандартного отклонения
double SteerConfig::CalculateStandardDeviation(QVector<double> data, double mean)
{
    if (data.size() < 2) return 0;

    double sumOfSquares = 0.0;
    for (double d : data)
    {
        sumOfSquares += std::pow(d - mean, 2);
    }

    return std::sqrt(sumOfSquares / (data.size() - 1));
}

// Подсчет коэффициента уверенности
void SteerConfig::CalculateConfidenceLevel(QVector<double> sortedData)
{
    if (sortedData.size() < MIN_SAMPLES_FOR_ANALYSIS)
    {
        m_confidenceLevel = 0;
        return;
    }

    double oneStdDevRange = standardDeviation;
    double twoStdDevRange = 2 * standardDeviation;

    int withinOneStdDev = 0;
    int withinTwoStdDev = 0;

    for (double angle : sortedData)
    {
        double deviationFromMedian = std::abs(angle - median);
        if (deviationFromMedian <= oneStdDevRange) withinOneStdDev++;
        if (deviationFromMedian <= twoStdDevRange) withinTwoStdDev++;
    }

    double oneStdDevPercentage = static_cast<double>(withinOneStdDev) / sortedData.size();
    double twoStdDevPercentage = static_cast<double>(withinTwoStdDev) / sortedData.size();

    // ожидаемое нормальное распределение данных
    double expectedOneStdDev = 0.68;
    double expectedTwoStdDev = 0.95;

    // считаем баллы для каждой метрики
    double oneStdDevScore = std::max(0.0, 1 - std::abs(oneStdDevPercentage - expectedOneStdDev) / expectedOneStdDev);
    double twoStdDevScore = std::max(0.0, 1 - std::abs(twoStdDevPercentage - expectedTwoStdDev) / expectedTwoStdDev);
    double magnitudeScore = std::max(0.0, 1 - std::abs(recommendedWASZero) / 10.0); // штрафуем большие поправки
    double sampleSizeFactor = std::min(1.0, static_cast<double>(sortedData.size()) / (MIN_SAMPLES_FOR_ANALYSIS * 3)); // размер выборки влияет положительно

    // объединяем факторы
    m_confidenceLevel = ((oneStdDevScore * 0.3 + twoStdDevScore * 0.3 + magnitudeScore * 0.2 + sampleSizeFactor * 0.2) * 100);
    m_confidenceLevel = std::clamp(confidenceLevel(), 0.0, 100.0);
}

void SteerConfig::smartCalLabelClick()
{
    // Сброс калибровки Smart WAS при клике на любую статусную метку
    if (isCollectingData)
    {
        resetData();

        // Покажите короткое подтверждение сброса
        Backend::instance()->timedMessage(1500, tr("Reset To Default"), tr("CalibrationDataReset"));
    }
    QDEBUG<<"SmartCalLabelClick";
}

void SteerConfig::zeroWAS()
{
    if (!isCollectingData)
    {   Backend::instance()->timedMessage(2000, "SmartCalibrationErro", "gsSmartWASNotAvailable");
        return;
    }

    if (!m_hasValidRecommendation)
    {
        if (m_sampleCount < 200)
        {
            Backend::instance()->timedMessage(2000, tr("Need at least 200 samples for calibration. Drive on guidance lines to collect more data."), QString(tr("Insufficient Data")) + " " +
                                                                         QString::number(m_sampleCount, 'f', 1));
        }
        else
        {
            Backend::instance()->timedMessage(2000, tr("Calibration confidence is low. Need at least 70% confidence. Drive more consistently on guidance lines."), QString(tr("Low Confidence")) + " " +
                                                                                                                                      QString::number(confidenceLevel(), 'f', 1));
        }
        return;
    }

    // Получаем рекомендацию по смещению
    int recommendedOffsetAdjustment = getRecommendedWASOffsetAdjustment(SettingsManager::instance()->as_countsPerDegree());
    int newOffset = SettingsManager::instance()->as_wasOffset() + recommendedOffsetAdjustment;

    // Проверяем новое значение смещения на допустимый диапазон
    if (std::abs(newOffset) > 3900)
    {
        Backend::instance()->timedMessage(2000, tr("Recommended adjustment {0} exceeds safe range (±50). Please check WAS sensor alignment"), QString(tr("Exceeded Range")) + " " +
                                                                                                                                  QString::number(newOffset, 'f', 1));
        QDEBUG << "Smart Zero превысил диапазон:" << newOffset;
        return;
    }

    // Применяем смещение нуля WAS
    SettingsManager::instance()->setAs_wasOffset(newOffset);

    // Критически важно: применяем смещение к ранее собранным данным
    applyOffsetToCollectedData(recommendedWASZero);

    // Сообщаем об успешной настройке
    Backend::instance()->timedMessage(2000, tr("%1 образцов, %2% уверенности, коррекция %3°")
                                  .arg(sampleCount())
                                                .arg(QString::number(confidenceLevel(), 'f', 1))
                                  .arg(QString::number(recommendedWASZero, 'f', 2)),
    QString(tr("Смещение успешно применено")));


    QDEBUG << "Настройка Smart WAS выполнена -"
             << "Образцы:" << m_sampleCount
             << ", Уверенность:" << QString::number(confidenceLevel(), 'f', 1) << "%,"
             << "Корректировка:" << QString::number(recommendedWASZero, 'f', 2) << "°";
}

