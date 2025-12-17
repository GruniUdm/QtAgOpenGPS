#include "backend.h"
#include <QCoreApplication>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY (backend, "backend.qtagopengps")

Backend *Backend::s_instance = nullptr;
QMutex Backend::s_mutex;
bool Backend::s_cpp_created = false;

Backend::Backend(QObject *parent)
    : QObject{parent}{}

Backend *Backend::instance() {
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new Backend();
        qDebug(backend) << "Backend singleton created by C++ code.";
        s_cpp_created = true;
        // ensure cleanup on app exit
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                             delete s_instance; s_instance = nullptr;
                         });
    }
    return s_instance;
}

Backend *Backend::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if(!s_instance) {
        s_instance = new Backend();
        qDebug(backend) << "Backend singleton created by QML engine.";
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}
// ===== USER DATA MANAGEMENT IMPLEMENTATIONS =====
// note: not currently used in QML
/*
void Backend::setDistanceUser(const QString& value) {
    bool ok;
    double distance = value.toDouble(&ok);
    if (ok) {
        set_distanceUser(distance);
    }
}

void Backend::setWorkedAreaTotalUser(const QString& value) {
    bool ok;
    double area = value.toDouble(&ok);
    if (ok) {
        set_workedAreaTotalUser(area);
    }
}
*/



