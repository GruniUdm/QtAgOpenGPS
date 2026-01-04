#include "cmodulecomm.h"
#include "cahrs.h"
#include "settingsmanager.h"

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
        if (isWorkSwitchEnabled && (oldWorkSwitchHigh != workSwitchHigh))
        {
            oldWorkSwitchHigh = workSwitchHigh;

            if (workSwitchHigh != SettingsManager::instance()->f_isWorkSwitchActiveLow())
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
