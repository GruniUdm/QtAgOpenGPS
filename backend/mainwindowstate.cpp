#include "mainwindowstate.h"
#include <QCoreApplication>

//problematic if static instance gets deleted and this
//is a dangling pointer
MainWindowState *MainWindowState::s_instance = nullptr;
QMutex MainWindowState::s_mutex;
bool MainWindowState::s_cpp_created = false;

MainWindowState::MainWindowState(QObject *parent)
    : QObject{parent}
{}

MainWindowState *MainWindowState::instance() {
    QMutexLocker locker(&s_mutex);
    if (!s_instance) {
        s_instance = new MainWindowState();
        mainWindow = s_instance;
        s_cpp_created = true;
        // ensure cleanup on app exit
        QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit,
                         s_instance, []() {
                         delete s_instance; mainWindow = nullptr; s_instance = nullptr;
                         });
    }
    return s_instance;
}

MainWindowState *MainWindowState::create(QQmlEngine *qmlEngine, QJSEngine *jsEngine) {
    Q_UNUSED(jsEngine)

    QMutexLocker locker(&s_mutex);

    if(!s_instance) {
        s_instance = new MainWindowState();
        mainWindow = s_instance;
    } else if (s_cpp_created) {
        qmlEngine->setObjectOwnership(s_instance, QQmlEngine::CppOwnership);
    }

    return s_instance;

}
