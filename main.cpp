// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// main
#include "formgps.h"
#include <QApplication>
#include <QCoreApplication>
#include <QLabel>
#include "classes/settingsmanager.h"  // MASSIVE MIGRATION: New SettingsManager
#include "aogrenderer.h"
#include "classes/agioservice.h"      // For auto-registration + C++ usage
#include "classes/ctrack.h"           // For auto-registration + C++ usage
#include "classes/cvehicle.h"         // For auto-registration + C++ usage
#include <QProcess>
#include <QSysInfo>
#ifdef  Q_OS_ANDROID
#include <QtCore/private/qandroidextras_p.h>
#endif
#include <QTranslator> //for translations
#include <QtQml/QQmlEngine>
#include <QtQml/QJSEngine>
#include <QtQml/qqmlregistration.h>

QLabel *grnPixelsWindow;
QLabel *overlapPixelsWindow;
// MASSIVE MIGRATION: Settings *settings; REMOVED - replaced by SettingsManager singleton

#ifndef TESTING
int main(int argc, char *argv[])
{
#ifdef  Q_OS_ANDROID
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([]() {
        QJniObject activity = QtAndroidPrivate::activity();
        if (activity.isValid()) {
            QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
            if (window.isValid()) {
                const int FLAG_KEEP_SCREEN_ON = 128;
                window.callMethod<void>("addFlags", "(I)V", FLAG_KEEP_SCREEN_ON);
            }
        }
    });
#endif

    qputenv("QSG_RENDER_LOOP", "threaded");

    QLoggingCategory::setFilterRules(QStringLiteral("qt.scenegraph.general=true"));
    QApplication a(argc, argv);

    QFont f = a.font();
    f.setPointSize(16);
    a.setFont(f);
    QCoreApplication::setOrganizationName("QtAgOpenGPS");
    QCoreApplication::setOrganizationDomain("qtagopengps");
    QCoreApplication::setApplicationName("QtAgOpenGPS");
    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat,
                       QSettings::UserScope,
                       QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));

    //We're supposed to be compatible with the saved data
    //from this version of AOG:
    QCoreApplication::setApplicationVersion("4.1.0");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qRegisterMetaTypeStreamOperators<QList<int> >("QList<int>");
    qRegisterMetaTypeStreamOperators<QVector<int> >("QVector<int>");
#endif
    QQuickWindow::setGraphicsApi(QSGRendererInterface::OpenGL);
    
    // SettingsManager: Unified Meyer's singleton registration
    qmlRegisterSingletonType<SettingsManager>("AOG", 1, 0, "SettingsManager",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            qDebug() << "🔥🔥🔥 SettingsManager singleton registration CALLED! 🔥🔥🔥";
            return SettingsManager::instance();
        });

    // PHASE 1 COMPLETION: Temporary alias - QML files still reference "Settings"
    // TODO: Phase 1.x - Full QML migration Settings → SettingsManager (805 refs)
    qmlRegisterSingletonType<SettingsManager>("AOG", 1, 0, "Settings",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            qDebug() << "🔥🔥🔥 Settings (alias) singleton registration CALLED! 🔥🔥🔥";
            return SettingsManager::instance();
        });
    
    // AOGRenderer: Component registration (OpenGL renderers must be instantiated in QML)
    qmlRegisterType<AOGRendererInSG>("AOG", 1, 0, "AOGRenderer");
    
    // AgIOService: Unified Meyer's singleton registration (QtAgIO.ini)
    qmlRegisterSingletonType<AgIOService>("AOG", 1, 0, "AgIOService",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            qDebug() << "🔥🔥🔥 AgIOService singleton registration CALLED! 🔥🔥🔥";
            return AgIOService::instance();
        });
    
    // CTrack/TracksInterface: Manual registration (most reliable for complex singletons)
    qmlRegisterSingletonType<CTrack>("AOG", 1, 0, "TracksInterface",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            qDebug() << "🔥🔥🔥 TracksInterface manual registration CALLED! 🔥🔥🔥";
            return CTrack::instance();
        });
    
    qmlRegisterSingletonType<CVehicle>("AOG", 1, 0, "VehicleInterface",
        [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            qDebug() << "🔥🔥🔥 VehicleInterface manual registration CALLED! 🔥🔥🔥";
            return CVehicle::instance();
        });
    
    // MASSIVE MIGRATION: settings = new Settings(); REMOVED
    //AOGProperty::init_defaults();
    SettingsManager::instance()->sync();
    
    qDebug() << "=== MASSIVE MIGRATION PHASE 1.2.5 COMPLETED ===\n"
             << "SettingsManager registered as QML singleton\n"
             << "Settings (old) REMOVED - migration complete\n"
             << "Status: Ready for compilation test";
    FormGPS w;
    //w.show();
    
    // MASSIVE MIGRATION: Validate SettingsManager accessible
    qDebug() << "SettingsManager instance:" << SettingsManager::instance();
    qDebug() << "Settings count:" << SettingsManager::instance()->keys().size();

    if (SettingsManager::instance()->value(SETTINGS_display_showBack).value<bool>()) {
        grnPixelsWindow = new QLabel("Back Buffer");
        grnPixelsWindow->setFixedWidth(500);
        grnPixelsWindow->setFixedHeight(500);
        grnPixelsWindow->show();
        overlapPixelsWindow = new QLabel("overlap buffer");
        overlapPixelsWindow->setFixedWidth(500);
        overlapPixelsWindow->setFixedHeight(500);
        overlapPixelsWindow->show();
    }

// //auto start AgIO
// #ifndef __ANDROID__
//     QProcess process;
//     if(SettingsManager::instance()->value(SETTINGS_feature_isAgIOOn).value<bool>()){
//         QObject::connect(&process, &QProcess::errorOccurred, [&](QProcess::ProcessError error) {
//             if (error == QProcess::Crashed) {
//                 qDebug() << "AgIO Crashed! Continuing QtAgOpenGPS like normal";
//             }
//         });

// //start the application
// #ifdef __WIN32
//         process.start("./QtAgIO.exe");

// #else //assume linux
//         process.start("./QtAgIO/QtAgIO");

// #endif

//         // Ensure process starts successfully
//         if (!process.waitForStarted()) {
//             qWarning() << "AgIO failed to start. Continuing QtAgOpenGPS like normal";
//         }
//     }
// #endif




    /*
    CDubinsTurningRadius = 5.25;

    CDubins c;
    Vec3 start(0,0,0);
    Vec3 goal (8,0,0);
    QVector<Vec3> pathlist;
    pathlist = c.GenerateDubins(start, goal);

    foreach(Vec3 goal: pathlist) {
        qDebug() << goal.easting<< ", "<<goal.northing;
    }
    return 0;
    */

    //Test file I/O
    //w.fileSaveCurveLines();
    //w.fileSaveBoundary();
    //w.fileSaveABLines();
    //w.fileSaveContour();
    //w.fileSaveVehicle("/tmp/TestVehicle.txt");
    //w.fileOpenField("49111 1 1 2020.Mar.21 09_58");
    //w.ABLine.isBtnABLineOn = true;
    //w.hd.isOn = true;
    //w.yt.isYouTurnBtnOn = true;

    //w.ABLine.isBtnABLineOn = true;
    //w.fileOpenTool("/tmp/TestTool1.txt");
    //w.fileOpenVehicle("/tmp/TestVehicle2.txt");
    //w.fileSaveTool("/tmp/TestTool.TXT");
    /*
    //testing to see how locale affects numbers in the stream writer
    QFileInfo testit("/tmp/noexistant/file.txt");
    qDebug() << testit.baseName();
    qDebug() << testit.suffix();
    QFile testFile("/tmp/test.txt");
    testFile.open(QIODevice::WriteOnly);
    QTextStream writer(&testFile);
    writer << "Testing" << Qt::endl;
    writer << qSetFieldWidth(3) << (double)3.1415926535 << Qt::endl;
    testFile.close();
    */

    return a.exec();
}
#endif
