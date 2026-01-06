// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// main
#include "formgps.h"
#include "backend.h"
#include <QApplication>
#include <QCoreApplication>
#include <QLabel>
#include "classes/settingsmanager.h"  // MASSIVE MIGRATION: New SettingsManager
#include "aogrenderer.h"
#include "classes/agioservice.h"      // For auto-registration + C++ usage
#include "classes/ctrack.h"           // For auto-registration + C++ usage
#include "classes/cvehicle.h"         // For auto-registration + C++ usage
#include "classes/pgnparser.h"        // Phase 6.0.21: For ParsedData metatype registration
#include <QProcess>
#include <QSysInfo>
#include <QTranslator> //for translations
#include <QtQml/QQmlEngine>
#include <QtQml/QJSEngine>
#include <QtQml/qqmlregistration.h>
#include <QLoggingCategory>
#include <QIcon>
#include <QPermissions>

QLabel *grnPixelsWindow;
QLabel *overlapPixelsWindow;

QString findIconPath() {
    QString appDir = QCoreApplication::applicationDirPath();

#ifdef Q_OS_WIN
    return appDir + "/icons/icon.ico";
#else
    return appDir + "/icons/64x64/icon.png";
#endif
}

int main(int argc, char *argv[])
{
    qputenv("QSG_RENDER_LOOP", "threaded");

#ifdef  Q_OS_ANDROID
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([]() {
        QJniObject activity = QNativeInterface::QAndroidApplication::context();
        if (activity.isValid()) {
            QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
            if (window.isValid()) {
                const int FLAG_KEEP_SCREEN_ON = 128;
                window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
            }
        }
    });
#endif

    // PHASE 6.0.23.1: Disable debug logs to prevent performance issues (40Hz PGN spam)
    // Phase 6.0.24: Allow selective debug logging for AgIOService (change agioservice.debug=false to true)
    QLoggingCategory::setFilterRules(QStringLiteral(
        "*.debug=false\n"
        "agioservice.debug=false\n"  // Change to true to enable AgIOService debug logs
        "*.qtagopengps.debug=true\n"
        "formgps_position.qtagopengps=false\n"
        "ctool.qtagopengps=false\n"
        "formgps_opengl.qtagopengps=false\n"
        "qt.scenegraph.general=true\n"
        "*.warning=true\n"
        "*.critical=true\n"
        "*.fatal=true"
    ));

    qSetMessagePattern("%{time hh:mm:ss.zzz} [%{type}] %{function}:%{line} - %{message}");
    QApplication a(argc, argv);

    QString iconPath = findIconPath();
    if (QFile::exists(iconPath)) {
        a.setWindowIcon(QIcon(iconPath));
    }

    QFont f = a.font();
    f.setPointSize(16);
    a.setFont(f);
    QCoreApplication::setOrganizationName("QtAgOpenGPS");
    QCoreApplication::setOrganizationDomain("qtagopengps");
    QCoreApplication::setApplicationName("QtAgOpenGPS");
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts, true);
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat,
                       QSettings::UserScope,
                       QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

    //We're supposed to be compatible with the saved data
    //from this version of AOG:
    QCoreApplication::setApplicationVersion("4.1.0");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#error Requires Qt 6
#endif

    // Phase 6.0.21: Register PGNParser::ParsedData for Qt::QueuedConnection signals
    qRegisterMetaType<PGNParser::ParsedData>("PGNParser::ParsedData");

    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);

    // Explicit QML module registration (ensures static initializer runs)
    // qt_add_qml_module generates this function in qtagopengps_qmltyperegistrations.cpp
    extern void qml_register_types_AOG();
    qml_register_types_AOG();

    // Request location permissions for GPS functionality
    QLocationPermission locationPermission;
    locationPermission.setAccuracy(QLocationPermission::Precise);
    locationPermission.setAvailability(QLocationPermission::WhenInUse);

    switch (a.checkPermission(locationPermission)) {
    case Qt::PermissionStatus::Undetermined:
        qDebug() << "Location permission undetermined, requesting...";
        a.requestPermission(locationPermission, [](const QPermission &permission) {
            if (qApp->checkPermission(permission) == Qt::PermissionStatus::Granted) {
                qDebug() << "Location permission granted";
            } else {
                qWarning() << "Location permission denied - GPS functionality will not work";
            }
        });
        break;
    case Qt::PermissionStatus::Denied:
        qWarning() << "Location permission denied - GPS functionality will not work";
        break;
    case Qt::PermissionStatus::Granted:
        qDebug() << "Location permission already granted";
        break;
    }

    FormGPS w;
    //w.show();
    
    // MASSIVE MIGRATION: Validate SettingsManager accessible
    qDebug() << "SettingsManager instance:" << SettingsManager::instance();
    qDebug() << "SettingsManager initialization: completed";

    if (SettingsManager::instance()->display_showBack()) {
        grnPixelsWindow = new QLabel("Back Buffer");
        grnPixelsWindow->setFixedWidth(500);
        grnPixelsWindow->setFixedHeight(500);
        grnPixelsWindow->show();
        overlapPixelsWindow = new QLabel("overlap buffer");
        //overlapPixelsWindow->setFixedWidth(1300);
        //overlapPixelsWindow->setFixedHeight(900);
        overlapPixelsWindow->show();
    }

    return a.exec();
}
