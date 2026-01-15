#include <QCoreApplication>
#include "boundaryinterface.h"

BoundaryInterface *BoundaryInterface::s_instance = nullptr;
QMutex BoundaryInterface::s_mutex;
bool BoundaryInterface::s_cpp_created = false;

BoundaryInterface::BoundaryInterface(QObject *parent)
    : QObject{parent}
{}

BoundaryInterface *BoundaryInterface::instance() {
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new BoundaryInterface();
        s_cpp_created = true;
        // ensure cleanup on app exit
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                         delete s_instance; s_instance = nullptr;
                         });
    }
    return s_instance;
}

BoundaryInterface *BoundaryInterface::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if(!s_instance) {
        s_instance = new BoundaryInterface();
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;
}
