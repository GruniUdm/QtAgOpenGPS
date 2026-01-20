#include <QCoreApplication>
#include "recordedpath.h"

RecordedPath *RecordedPath::s_instance = nullptr;
QMutex RecordedPath::s_mutex;
bool RecordedPath::s_cpp_created = false;

RecordedPath::RecordedPath(QObject *parent)
    : QObject{parent}
{
    m_model = new RecordedPathModel(this);
}

RecordedPath *RecordedPath::instance() {
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new RecordedPath();
        s_cpp_created = true;
        // Ensure cleanup on app exit
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                         delete s_instance; s_instance = nullptr;
                         });
    }
    return s_instance;
}

RecordedPath *RecordedPath::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if(!s_instance) {
        s_instance = new RecordedPath();
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}
