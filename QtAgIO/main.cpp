#include <QGuiApplication>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QDebug>
#include <QStandardPaths>
#include "formloop.h"
// This main.cpp file will be used only for test if we need to work on QAgIO as à stanalone module app
// In this case cemakefiles need to be adapted also


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QCoreApplication::setOrganizationName("QtAgOpenGPS");
    QCoreApplication::setOrganizationDomain("qtagopengps");
    QCoreApplication::setApplicationName("QtAgIO");
    QCoreApplication::setApplicationVersion("0.0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("IO handler for AgOpenGPS and QtAgOpenGPS");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption verbose("verbose", "Set verbosity level, 0-7. 0=silent, 7=noisy, 2=default", "verbose", "2");
    parser.addOption(verbose);

    parser.process(app);

    // I don't think this does what I think it does.  Can I set verbosity of the default object?
    // Does that do anything or is verbosity only useful for diy streams?
    qDebug().setVerbosity(parser.value(verbose).toInt());

    QSettings::setDefaultFormat(QSettings::IniFormat);
    QSettings::setPath(QSettings::IniFormat,
                       QSettings::UserScope,
                       QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    
    // Create the engine
    QQmlApplicationEngine engine;

    FormLoop *formLoop = FormLoop::instance();
    if (!formLoop) {
        qDebug() << "Error : `FormLoop::instance()` is nullptr!";
        return -1;
    } else {
        qDebug() << " FormLoop::instance() is succesfully instantiated.";
    }

    formLoop->setEngine(&engine);    
    qDebug() << "AgIO: All context properties set, loading QML...";
    engine.addImportPath("qrc:/qtagopengps.org/imports/");
    
    // Now load the QML
    engine.load(QUrl(QStringLiteral("qrc:/qtagopengps.org/imports/QtAgIO/Main.qml")));

       
    if (engine.rootObjects().isEmpty()) {
        qDebug() << "Error: Failed to load QML!";
        return -1;
    } else {
        qDebug() << "QML loaded successfully.";
        formLoop->setupGUI();    }
    
    qDebug() << "AgIO started successfully";
    
    return app.exec();
}

