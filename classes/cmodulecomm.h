#ifndef CMODULECOMM_H
#define CMODULECOMM_H

#include <QObject>
#include <QQmlEngine>
#include <QMutex>
#include <QPropertyBinding>
#include <QtCore>
#include <QString>
#include "cpgn.h"
#include "simpleproperty.h"

class CAHRS;

class CModuleComm: public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ModuleComm)
    QML_SINGLETON

    Q_PROPERTY(CPGN_FE p_254 MEMBER p_254 NOTIFY p_254_changed)
    Q_PROPERTY(CPGN_FC p_252 MEMBER p_252 NOTIFY p_252_changed)
    Q_PROPERTY(CPGN_FB p_251 MEMBER p_251 NOTIFY p_251_changed)
    Q_PROPERTY(CPGN_EF p_239 MEMBER p_239 NOTIFY p_239_changed)
    Q_PROPERTY(CPGN_EE p_238 MEMBER p_238 NOTIFY p_238_changed)
    Q_PROPERTY(CPGN_EC p_236 MEMBER p_236 NOTIFY p_236_changed)
    Q_PROPERTY(CPGN_EB p_235 MEMBER p_235 NOTIFY p_235_changed)
    Q_PROPERTY(CPGN_E5 p_229 MEMBER p_229 NOTIFY p_229_changed)

private:
    explicit CModuleComm(QObject *parent = nullptr);
    ~CModuleComm() override = default;

    // Prevent copying
    CModuleComm(const CModuleComm &) = delete;
    CModuleComm &operator=(const CModuleComm &) = delete;

    static CModuleComm *s_instance;
    static QMutex s_mutex;
    static bool s_cpp_created;

public:
    static CModuleComm *instance();
    static CModuleComm *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);
    // ---- Section control switches to AOG  ---------------------------------------------------------
    //PGN - 32736 - 127.249 0x7FF9
    uchar ss[9];

    uchar ssP[9];

    int
        swHeader = 0,
        swMain = 1,
        swReserve = 2,
        swReserve2 = 3,
        swNumSections = 4,
        swOnGr0 = 5,
        swOffGr0 = 6,
        swOnGr1 = 7,
        swOffGr1 = 8;


    qint64 blockage_lastUpdate;
    double blockageseccount[64];
    int blockageseccount1[16];
    int blockageseccount2[16];
    int blockageseccount3[16];
    int blockageseccount4[16];


    //for the workswitch
    bool isWorkSwitchEnabled,
        isWorkSwitchManualSections, isSteerWorkSwitchManualSections, isSteerWorkSwitchEnabled;

    bool oldWorkSwitchHigh, oldSteerSwitchHigh, oldSteerSwitchRemote;

    void CheckWorkAndSteerSwitch(CAHRS &ahrs, bool isBtnAutoSteerOn);


    Q_INVOKABLE void modulesSend238();
    Q_INVOKABLE void modulesSend251();
    Q_INVOKABLE void modulesSend252();

    /*
     * PGNs *
     */
    CPGN_FE p_254;
    CPGN_FC p_252;
    CPGN_FB p_251;
    CPGN_EF p_239;
    CPGN_EE p_238;
    CPGN_EC p_236;
    CPGN_EB p_235;
    CPGN_E5 p_229;

    SIMPLE_BINDABLE_PROPERTY(double,  actualSteerAngleDegrees)
    SIMPLE_BINDABLE_PROPERTY(bool, steerSwitchHigh)
    SIMPLE_BINDABLE_PROPERTY(bool, workSwitchHigh)
    SIMPLE_BINDABLE_PROPERTY(int, sensorData)
    SIMPLE_BINDABLE_PROPERTY(int, pwmDisplay)

signals:
    void stopAutoSteer(void);
    void turnOffManualSections(void);
    void turnOffAutoSections(void);

    void p_254_changed();
    void p_252_changed();
    void p_251_changed();
    void p_239_changed();
    void p_238_changed();
    void p_236_changed();
    void p_235_changed();
    void p_229_changed();

public slots:
private:
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(CModuleComm, double, m_actualSteerAngleDegrees, 0, &CModuleComm::actualSteerAngleDegreesChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(CModuleComm, bool, m_steerSwitchHigh, 0, &CModuleComm::steerSwitchHighChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(CModuleComm, bool, m_workSwitchHigh, 0, &CModuleComm::workSwitchHighChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(CModuleComm, int, m_sensorData, -1, &CModuleComm::sensorDataChanged)
    Q_OBJECT_BINDABLE_PROPERTY_WITH_ARGS(CModuleComm, int, m_pwmDisplay, 0, &CModuleComm::pwmDisplayChanged)
};

#endif // CMODULECOMM_H
