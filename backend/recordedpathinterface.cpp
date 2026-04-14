#include "recordedpathinterface.h"
#include "recordedpath.h"
#include "cvehicle.h"
#include "cyouturn.h"
#include "siminterface.h"
#include "backend.h"
#include "glm.h"

#include <QCoreApplication>

RecordedPathInterface *RecordedPathInterface::s_instance = nullptr;
QMutex RecordedPathInterface::s_mutex;
bool RecordedPathInterface::s_cpp_created = false;

RecordedPathInterface::RecordedPathInterface(QObject *parent)
    : QObject{parent}
{
}

RecordedPathInterface *RecordedPathInterface::instance() {
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new RecordedPathInterface();
        s_cpp_created = true;
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                         delete s_instance; s_instance = nullptr;
                         });
    }
    return s_instance;
}

RecordedPathInterface *RecordedPathInterface::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if (!s_instance) {
        s_instance = new RecordedPathInterface();
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}

void RecordedPathInterface::recPathLoad()
{
    RecordedPath::instance()->updateInterface();
}

void RecordedPathInterface::recPathClear()
{
    RecordedPath::instance()->recList.clear();
    RecordedPath::instance()->updateInterface();
}

void RecordedPathInterface::recPathFollowStop()
{
    if (RecordedPath::instance()->isDrivingRecordedPath())
    {
        RecordedPath::instance()->StopDrivingRecordedPath();
    }
    else
    {
        CYouTurn *yt = qobject_cast<CYouTurn*>(Backend::instance()->yt());
        RecordedPath::instance()->StartDrivingRecordedPath(*CVehicle::instance(), *yt);
    }
}

void RecordedPathInterface::recPathRecordStop()
{
    if (RecordedPath::instance()->isRecordOn)
    {
        RecordedPath::instance()->isRecordOn = false;
        emit recordStateChanged(false);
        emit showPathNewDialog();
    }
    else if (Backend::instance()->isJobStarted())
    {
        RecordedPath::instance()->recList.clear();
        RecordedPath::instance()->isRecordOn = true;
        emit recordStateChanged(true);
    }
}

void RecordedPathInterface::recPathResumeStyle()
{
    RecordedPath::instance()->resumeState++;
    if (RecordedPath::instance()->resumeState > 2)
        RecordedPath::instance()->resumeState = 0;
}

void RecordedPathInterface::recPathSwapAB()
{
    int cnt = RecordedPath::instance()->recList.size();
    QVector<CRecPathPt> _recList;
    _recList.reserve(cnt);

    for (int i = cnt - 1; i > -1; i--)
    {
        CRecPathPt pt = RecordedPath::instance()->recList[i];
        pt.heading += M_PI;
        if (pt.heading < -glm::twoPI) pt.heading += glm::twoPI;
        _recList.append(pt);
    }
    RecordedPath::instance()->recList.clear();
    RecordedPath::instance()->recList = _recList;
    RecordedPath::instance()->updateInterface();
}

void RecordedPathInterface::recPathPick()
{
    RecordedPath::instance()->resumeState = 0;
    RecordedPath::instance()->currentPositonIndex = 0;
    emit RecordedPath::instance()->open("");
}