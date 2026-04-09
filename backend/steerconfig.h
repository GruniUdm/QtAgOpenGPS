#ifndef STEERCONFIG_H
#define STEERCONFIG_H

#include <QObject>
#include <QBindable>
#include <QQmlEngine>
#include <QMutex>
#include <QTimer>
#include <QElapsedTimer>

#include "simpleproperty.h"
#include "vec3.h"


class SteerConfig : public QObject
{
    Q_OBJECT
    QML_SINGLETON
    QML_ELEMENT
private:
    explicit SteerConfig(QObject *parent = nullptr);
    ~SteerConfig() override=default;

    //prevent copying
    SteerConfig(const SteerConfig &) = delete;
    SteerConfig &operator=(const SteerConfig &) = delete;

    static SteerConfig *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    static SteerConfig *instance();
    static SteerConfig *create (QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    SIMPLE_BINDABLE_PROPERTY(bool, isSA)
    SIMPLE_BINDABLE_PROPERTY(bool, isSALeft)
    SIMPLE_BINDABLE_PROPERTY(double, calcSteerAngleInner)
    SIMPLE_BINDABLE_PROPERTY(double, calcSteerAngleLeft)
    SIMPLE_BINDABLE_PROPERTY(double, diameter)
    SIMPLE_BINDABLE_PROPERTY(double, diameterLeft)
    SIMPLE_BINDABLE_PROPERTY(bool, haveSteerAngle)
    SIMPLE_BINDABLE_PROPERTY(bool, hasValidRecommendation)
    SIMPLE_BINDABLE_PROPERTY(int, sampleCount)
    SIMPLE_BINDABLE_PROPERTY(double, confidenceLevel)

    // Auto-tune properties
    SIMPLE_BINDABLE_PROPERTY(bool, isAutoTuning)
    SIMPLE_BINDABLE_PROPERTY(int, currentTestKp)
    SIMPLE_BINDABLE_PROPERTY(double, minError)
    SIMPLE_BINDABLE_PROPERTY(double, currentError)
    SIMPLE_BINDABLE_PROPERTY(double, kuValue)
    SIMPLE_BINDABLE_PROPERTY(bool, oscillationDetected)

    // PWM auto-tune properties
    SIMPLE_BINDABLE_PROPERTY(bool, isPwmAutoTuning)
    SIMPLE_BINDABLE_PROPERTY(int, pwmTestValue)
    SIMPLE_BINDABLE_PROPERTY(double, pwmMinAngle)
    SIMPLE_BINDABLE_PROPERTY(double, pwmMaxAngle)
    SIMPLE_BINDABLE_PROPERTY(int, pwmDirection)
    SIMPLE_BINDABLE_PROPERTY(int, foundMinPwmLeft)
    SIMPLE_BINDABLE_PROPERTY(int, foundMinPwmRight)
    SIMPLE_BINDABLE_PROPERTY(int, foundMaxPwmLeft)
    SIMPLE_BINDABLE_PROPERTY(int, foundMaxPwmRight)
    SIMPLE_BINDABLE_PROPERTY(double, maxAngleLeft)
    SIMPLE_BINDABLE_PROPERTY(double, maxAngleRight)
    SIMPLE_BINDABLE_PROPERTY(int, pwmTunePhase)

    Q_INVOKABLE void startSA();
    Q_INVOKABLE void stopSA();
    Q_INVOKABLE void startSALeft();
    Q_INVOKABLE void stopSALeft();
    Q_INVOKABLE void startAutoTune();
    Q_INVOKABLE void stopAutoTune();
    Q_INVOKABLE void startPwmAutoTune();
    Q_INVOKABLE void stopPwmAutoTune();

     // Методы для работы с данными
    Q_INVOKABLE int getRecommendedWASOffsetAdjustment(int currentCPD);
    Q_INVOKABLE void startDataCollection();
    Q_INVOKABLE void stopDataCollection();
    Q_INVOKABLE void resetData();
    Q_INVOKABLE void applyOffsetToCollectedData(double appliedOffsetDegrees);
    Q_INVOKABLE void smartCalLabelClick();
    Q_INVOKABLE void zeroWAS();
    void AddSteerAngleSample(double guidanceSteerAngle, double currentSpeed);

    bool isCollectingData = false;

public slots:
    void on_timer();

signals:

private:
    QTimer timer;
    QElapsedTimer elapsed;

    Vec3 startFix;
    Vec3 startFixLeft;
    double dist = 0.0;
    double distLeft = 0.0;
    int counter = 0, secondCntr = 0, cntr = 0, cntrLeft = 0;

    // Auto-tune state
    double prevError = 0;
    int oscillationCount = 0;
    double accumulatedError = 0;
    int errorSampleCount = 0;

    // PWM auto-tune state
    double prevPwmAngle = 0;
    double prevPwmAcceleration = 0;
    int pwmSamplesTaken = 0;
    bool minPwmFound = false;
    bool maxPwmFound = false;
    double leftMaxAngle = 0;
    double rightMaxAngle = 0;
    double currentMaxAngle = 0;

    // Публичные свойства was wizard
    double recommendedWASZero = 0;
    QDateTime lastCollectionTime;

    // Средние показатели распределения
    double mean;
    double standardDeviation;
    double median;

    // Вектор для хранения истории углов
    QVector<double> steerAngleHistory;

    // Критерии проверки собираемых данных
    static constexpr int MAX_SAMPLES = 2000;          // Максимальное число хранимых образцов
    static constexpr int MIN_SAMPLES_FOR_ANALYSIS = 200; // Минимальное число образцов для анализа
    static constexpr double MIN_SPEED_THRESHOLD = 2.0;  // км/ч — минимальная скорость для начала записи
    static constexpr double MAX_ANGLE_THRESHOLD = 25.0; // Градусы — максимальный угол для включения записи

    // Личные вспомогательные методы
    bool ShouldCollectSample(double steerAngle, double speed);
    void PerformStatisticalAnalysis();
    double CalculateMedian(QVector<double> sortedData);
    double CalculateStandardDeviation(QVector<double> data, double mean);
    void CalculateConfidenceLevel(QVector<double> sortedData);

    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, bool, m_isSA, false, &SteerConfig::isSAChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, bool, m_isSALeft, false, &SteerConfig::isSALeftChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_calcSteerAngleInner, 0, &SteerConfig::calcSteerAngleInnerChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_calcSteerAngleLeft, 0, &SteerConfig::calcSteerAngleLeftChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_diameter, 0, &SteerConfig::diameterChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_diameterLeft, 0, &SteerConfig::diameterLeftChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, bool, m_haveSteerAngle, false, &SteerConfig::haveSteerAngleChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, bool, m_hasValidRecommendation, false, &SteerConfig::hasValidRecommendationChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, int, m_sampleCount, false, &SteerConfig::sampleCountChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_confidenceLevel, 0, &SteerConfig::confidenceLevelChanged)

    // Auto-tune Q_OBJECT_BINDABLE properties
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, bool, m_isAutoTuning, false, &SteerConfig::isAutoTuningChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, int, m_currentTestKp, 1, &SteerConfig::currentTestKpChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_minError, 999.0, &SteerConfig::minErrorChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_currentError, 0, &SteerConfig::currentErrorChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_kuValue, 0, &SteerConfig::kuValueChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, bool, m_oscillationDetected, false, &SteerConfig::oscillationDetectedChanged)

    // PWM auto-tune Q_OBJECT_BINDABLE properties
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, bool, m_isPwmAutoTuning, false, &SteerConfig::isPwmAutoTuningChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, int, m_pwmTestValue, 0, &SteerConfig::pwmTestValueChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_pwmMinAngle, 0, &SteerConfig::pwmMinAngleChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_pwmMaxAngle, 0, &SteerConfig::pwmMaxAngleChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, int, m_pwmDirection, 0, &SteerConfig::pwmDirectionChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, int, m_foundMinPwmLeft, 0, &SteerConfig::foundMinPwmLeftChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, int, m_foundMinPwmRight, 0, &SteerConfig::foundMinPwmRightChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, int, m_foundMaxPwmLeft, 0, &SteerConfig::foundMaxPwmLeftChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, int, m_foundMaxPwmRight, 0, &SteerConfig::foundMaxPwmRightChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_maxAngleLeft, 0, &SteerConfig::maxAngleLeftChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, double, m_maxAngleRight, 0, &SteerConfig::maxAngleRightChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(SteerConfig, int, m_pwmTunePhase, 0, &SteerConfig::pwmTunePhaseChanged)
};

#endif // STEERCONFIG_H
