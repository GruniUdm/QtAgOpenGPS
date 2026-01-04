#include "cmodulecomm.h"
#include "cahrs.h"
#include "settingsmanager.h"

CModuleComm *CModuleComm::s_instance = nullptr;
QMutex CModuleComm::s_mutex;
bool CModuleComm::s_cpp_created = false;

CModuleComm::CModuleComm(QObject *parent) : QObject(parent)
{
    //does a low, grounded out, mean on
    isWorkSwitchActiveLow = true;
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
    //AutoSteerAuto button enable - Ray Bear inspired code - Thx Ray!
    if (ahrs.isAutoSteerAuto && steerSwitchHigh != oldSteerSwitchRemote)
    {
        oldSteerSwitchRemote = steerSwitchHigh;
        //steerSwith is active low
        if (steerSwitchHigh == isBtnAutoSteerOn)
            emit stopAutoSteer();
            //mf.btnAutoSteer.PerformClick();
    }

    if (SettingsManager::instance()->f_isRemoteWorkSystemOn())
    {
        if (isWorkSwitchEnabled && (oldWorkSwitchHigh != workSwitchHigh))
        {
            oldWorkSwitchHigh = workSwitchHigh;

            if (workSwitchHigh != isWorkSwitchActiveLow)
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

        if (isSteerWorkSwitchEnabled && (oldSteerSwitchHigh != steerSwitchHigh))
        {
            oldSteerSwitchHigh = steerSwitchHigh;

            if ((isBtnAutoSteerOn && ahrs.isAutoSteerAuto)
                || (!ahrs.isAutoSteerAuto && !steerSwitchHigh))
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
