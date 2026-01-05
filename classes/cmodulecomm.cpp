#include "cmodulecomm.h"
#include "cahrs.h"
#include "settingsmanager.h"
#include "agioservice.h"

Q_LOGGING_CATEGORY (cmodulecomm_log, "cmodulecomm.qtagopengps")
#define QDEBUG qDebug(cmodulecomm_log)

CModuleComm *CModuleComm::s_instance = nullptr;
QMutex CModuleComm::s_mutex;
bool CModuleComm::s_cpp_created = false;

CModuleComm::CModuleComm(QObject *parent) : QObject(parent)
{
}

CModuleComm *CModuleComm::instance()
{
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new CModuleComm();
        s_cpp_created = true;
        // Ensure cleanup on app exit
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                             delete s_instance;
                             s_instance = nullptr;
                         });
    }
    return s_instance;
}

CModuleComm *CModuleComm::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if (!s_instance) {
        s_instance = new CModuleComm();
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}


void CModuleComm::CheckWorkAndSteerSwitch(CAHRS &ahrs, bool isBtnAutoSteerOn)
{
    //This really doesn't want to be in this class.  It's only used in FormGPS

    //AutoSteerAuto button enable - Ray Bear inspired code - Thx Ray!
    if (ahrs.isAutoSteerAuto && m_steerSwitchHigh != oldSteerSwitchRemote)
    {
        oldSteerSwitchRemote = m_steerSwitchHigh;
        //steerSwith is active low
        set_steerSwitchHigh(isBtnAutoSteerOn);
        if (m_steerSwitchHigh)
            emit stopAutoSteer();
            //mf.btnAutoSteer.PerformClick();
    }

    if (SettingsManager::instance()->f_isRemoteWorkSystemOn())
    {
        if (isWorkSwitchEnabled && (oldWorkSwitchHigh != workSwitchHigh()))
        {
            oldWorkSwitchHigh = workSwitchHigh();

            if (workSwitchHigh() != SettingsManager::instance()->f_isWorkSwitchActiveLow())
            {
                if (isWorkSwitchManualSections)
                {
                    emit turnOffManualSections();
                    //mf.btnSectionMasterManual.PerformClick();
                }
                else
                {
                    emit turnOffAutoSections();
                    //mf.btnSectionMasterAuto.PerformClick();
                }
            }

            else//Checks both on-screen buttons, performs click if button is not off
            {
                emit turnOffAutoSections();
                //mf.btnSectionMasterAuto.PerformClick();
                emit turnOffManualSections();
                //mf.btnSectionMasterManual.PerformClick();
            }
        }

        if (isSteerWorkSwitchEnabled && (oldSteerSwitchHigh != m_steerSwitchHigh))
        {
            oldSteerSwitchHigh = m_steerSwitchHigh;

            if ((isBtnAutoSteerOn && ahrs.isAutoSteerAuto)
                || (!ahrs.isAutoSteerAuto && !m_steerSwitchHigh))
            {
                if (isSteerWorkSwitchManualSections)
                {
                    emit turnOffManualSections();
                    //mf.btnSectionMasterManual.PerformClick();
                }
                else
                {
                    emit turnOffAutoSections();
                    //mf.btnSectionMasterAuto.PerformClick();
                }
            }

            else//Checks both on-screen buttons, performs click if button is not off
            {
                emit turnOffAutoSections();
                //mf.btnSectionMasterAuto.PerformClick();
                emit turnOffManualSections();
                //mf.btnSectionMasterManual.PerformClick();
            }
        }
    }
}

void CModuleComm::modulesSend238() {
    QDEBUG << "Sending 238 message to AgIO";
    p_238.pgn[p_238.set0] = SettingsManager::instance()->ardMac_setting0();
    p_238.pgn[p_238.raiseTime] = SettingsManager::instance()->ardMac_hydRaiseTime();
    p_238.pgn[p_238.lowerTime] = SettingsManager::instance()->ardMac_hydLowerTime();

    p_238.pgn[p_238.user1] = SettingsManager::instance()->ardMac_user1();
    p_238.pgn[p_238.user2] = SettingsManager::instance()->ardMac_user2();
    p_238.pgn[p_238.user3] = SettingsManager::instance()->ardMac_user3();
    p_238.pgn[p_238.user4] = SettingsManager::instance()->ardMac_user4();

    QDEBUG << SettingsManager::instance()->ardMac_user1();

    AgIOService::instance()->sendPgn(p_238.pgn);
}

void CModuleComm::modulesSend251() {
    //QDEBUG << "Sending 251 message to AgIO";
    p_251.pgn[p_251.set0] = SettingsManager::instance()->ardSteer_setting0();
    p_251.pgn[p_251.set1] = SettingsManager::instance()->ardSteer_setting1();
    p_251.pgn[p_251.maxPulse] = SettingsManager::instance()->ardSteer_maxPulseCounts();
    p_251.pgn[p_251.minSpeed] = 5; //0.5 kmh THIS IS CHANGED IN AOG FIXES

    if (SettingsManager::instance()->as_isConstantContourOn())
        p_251.pgn[p_251.angVel] = 1;
    else p_251.pgn[p_251.angVel] = 0;

    QDEBUG << p_251.pgn;

    AgIOService::instance()->sendPgn(p_251.pgn);
}

void CModuleComm::modulesSend252() {
    //QDEBUG << "Sending 252 message to AgIO";
    p_252.pgn[p_252.gainProportional] = SettingsManager::instance()->as_Kp();
    p_252.pgn[p_252.highPWM] = SettingsManager::instance()->as_highSteerPWM();
    p_252.pgn[p_252.lowPWM] = SettingsManager::instance()->as_lowSteerPWM();
    p_252.pgn[p_252.minPWM] = SettingsManager::instance()->as_minSteerPWM();
    p_252.pgn[p_252.countsPerDegree] = SettingsManager::instance()->as_countsPerDegree();
    int wasOffset = (int)SettingsManager::instance()->as_wasOffset();
    p_252.pgn[p_252.wasOffsetHi] = (char)(wasOffset >> 8);
    p_252.pgn[p_252.wasOffsetLo] = (char)wasOffset;
    p_252.pgn[p_252.ackerman] = SettingsManager::instance()->as_ackerman();


    QDEBUG << p_252.pgn;

    AgIOService::instance()->sendPgn(p_252.pgn);
}

