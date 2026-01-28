#ifndef PLOUGHCONTROL_H
#define PLOUGHCONTROL_H

#include <QObject>
#include <QMutex>
#include <QVariantMap>
#include <QTimer>
#include <QSettings>
#include "ploughmodel.h"
#include "pgnparser.h"
#include "settingsmanager.h"


class PloughControl : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    QMutex mutex;

protected:
    explicit PloughControl(QObject *parent = nullptr);
    ~PloughControl() override = default;

    // Prevent copying
    PloughControl(const PloughControl &) = delete;
    PloughControl &operator=(const PloughControl &) = delete;

    static PloughControl *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    static PloughControl *instance();
    static PloughControl *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    // Модель данных плуга
    Q_PROPERTY(PloughModel *ploughModel READ ploughModel CONSTANT)
    PloughModel *ploughModel() const { return m_ploughModel; }
    PloughModel *m_ploughModel;

    // Быстрые свойства для удобства (для плуга с индексом 0)
    Q_PROPERTY(int currentWidth READ currentWidth NOTIFY currentWidthChanged)
    Q_PROPERTY(int targetWidth READ targetWidth WRITE setTargetWidth NOTIFY targetWidthChanged)
    Q_PROPERTY(int ploughMode READ ploughMode NOTIFY ploughModeChanged)
    Q_PROPERTY(QString ploughModeName READ ploughModeName NOTIFY ploughModeChanged)
    Q_PROPERTY(bool ploughEnabled READ ploughEnabled WRITE setPloughEnabled NOTIFY ploughEnabledChanged)
    Q_PROPERTY(bool autoWidthEnabled READ autoWidthEnabled WRITE setAutoWidthEnabled NOTIFY autoWidthEnabledChanged)
    Q_PROPERTY(bool autoLiftEnabled READ autoLiftEnabled WRITE setAutoLiftEnabled NOTIFY autoLiftEnabledChanged)
    Q_PROPERTY(int calibrationValue READ calibrationValue WRITE setCalibrationValue NOTIFY calibrationValueChanged)
    Q_PROPERTY(int deadBand READ deadBand WRITE setDeadBand NOTIFY deadBandChanged)
    Q_PROPERTY(int minWidth READ minWidth NOTIFY minWidthChanged)
    Q_PROPERTY(int maxWidth READ maxWidth NOTIFY maxWidthChanged)
    Q_PROPERTY(bool isCalibrated READ isCalibrated NOTIFY calibrationChanged)
    Q_PROPERTY(bool sectionOn READ sectionOn NOTIFY sectionOnChanged)
    Q_PROPERTY(int lineDistance READ lineDistance NOTIFY lineDistanceChanged)
    Q_PROPERTY(int error READ error NOTIFY errorChanged)

    // Q_INVOKABLE методы управления
    Q_INVOKABLE void calibrateMin();
    Q_INVOKABLE void calibrateMax();
    Q_INVOKABLE void saveSettings();
    Q_INVOKABLE void loadSettings();
    Q_INVOKABLE void resetCalibration();
    Q_INVOKABLE void setTargetWidth(int width);
    Q_INVOKABLE void setPloughEnabled(bool enabled);
    Q_INVOKABLE void setAutoWidthEnabled(bool enabled);
    Q_INVOKABLE void setAutoLiftEnabled(bool enabled);
    Q_INVOKABLE void setDeadBand(int deadBand);
    Q_INVOKABLE void setCalibrationValue(int value);
    Q_INVOKABLE void setSectionOn(bool sectionOn);

    // Методы для обновления данных из протокола
    Q_INVOKABLE void updatePloughData(int width, int mode);
    Q_INVOKABLE void updateLineDistance(int distance);
    Q_INVOKABLE void updateSectionState(bool sectionOn);

    // Геттеры свойств (для плуга с индексом 0)
    int currentWidth() const;
    int targetWidth() const;
    int ploughMode() const;
    QString ploughModeName() const;
    bool ploughEnabled() const;
    bool autoWidthEnabled() const;
    bool autoLiftEnabled() const;
    int calibrationValue() const;
    int deadBand() const;
    int minWidth() const;
    int maxWidth() const;
    bool isCalibrated() const;
    bool sectionOn() const;
    int lineDistance() const;
    int error() const;

signals:
    // Сигналы свойств
    void currentWidthChanged(int width);
    void targetWidthChanged(int targetWidth);
    void ploughModeChanged(int mode);
    void ploughEnabledChanged(bool enabled);
    void autoWidthEnabledChanged(bool enabled);
    void autoLiftEnabledChanged(bool enabled);
    void calibrationValueChanged(int value);
    void deadBandChanged(int deadBand);
    void minWidthChanged(int minWidth);
    void maxWidthChanged(int maxWidth);
    void calibrationChanged();
    void sectionOnChanged(bool sectionOn);
    void lineDistanceChanged(int distance);
    void errorChanged(int error);

    // Сигналы для протокола связи
    void sendCalibrationCommand(int command, int value);
    void sendSettingsCommand(const QByteArray &settings);
    void sendPloughCommand(int command, int data);

    // Сигнал об обновлении всех данных
    void ploughDataUpdated();

public slots:
    // Слот для обработки входящих данных по протоколу
    void onPloughDataReady(const PGNParser::ParsedData& data);
    void onMachineSettingsReady(const PGNParser::ParsedData& data);

private:
    // Вспомогательные методы
    void initializeSettings();
    void updatePloughMode();
    QString modeToString(int mode) const;
    void sendCurrentSettings();
    void updateModelFromSettings();
    void updateError();

    // Данные плуга (дублируются в модели для быстрого доступа)
    int m_currentWidth = 0;
    int m_targetWidth = 200;
    int m_ploughMode = 0;
    bool m_ploughEnabled = false;
    bool m_autoWidthEnabled = false;
    bool m_autoLiftEnabled = false;
    int m_calibrationValue = 0;
    int m_deadBand = 10;
    int m_minWidth = 0;
    int m_maxWidth = 400;
    bool m_isCalibrated = false;
    bool m_sectionOn = false;
    int m_lineDistance = 0;
    int m_error = 0;

    // Настройки
    QSettings m_settings;

    // Таймеры
    QTimer m_updateTimer;
    QTimer m_settingsSaveTimer;

    // Флаги состояния
    bool m_settingsDirty = false;
    bool m_calibrationInProgress = false;
};

#endif // PLOUGHCONTROL_H
