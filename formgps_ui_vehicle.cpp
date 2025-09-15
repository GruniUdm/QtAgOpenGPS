// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// GUI to backend vehicle interface
#include "formgps.h"
#include "qmlutil.h"
#include "classes/settingsmanager.h"

QString caseInsensitiveFilename(QString directory, QString filename);

void FormGPS::vehicle_saveas(QString vehicle_name) {
#ifdef __ANDROID__
    QString directoryName = androidDirectory + QCoreApplication::applicationName() + "/Vehicles";
#else
    QString directoryName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                            + "/" + QCoreApplication::applicationName() + "/Vehicles";
#endif

    QDir saveDir(directoryName);
    if (!saveDir.exists()) {
        bool ok = saveDir.mkpath(directoryName);
        if (!ok) {
            qWarning() << "Couldn't create path " << directoryName;
            return;
        }
    }

    QString filename = directoryName + "/" + caseInsensitiveFilename(directoryName, vehicle_name);

    // CRITICAL: Save current vehicle AND tool settings to SettingsManager BEFORE saving JSON
    qDebug() << "Before CVehicle::saveSettings()";
    CVehicle::instance()->saveSettings();
    qDebug() << "CVehicle::saveSettings() completed";

    qDebug() << "Before tool.saveSettings()";
    this->tool.saveSettings();
    qDebug() << "tool.saveSettings() completed";

    // ASYNC SOLUTION: Defer saveJson to avoid mutex deadlock (same as field_close fix)
    qDebug() << "Scheduling async saveJson:" << filename;
    QTimer::singleShot(50, this, [this, filename]() {
        qDebug() << "Executing async saveJson:" << filename;
        SettingsManager::instance()->saveJson(filename);
        qDebug() << "Async saveJson completed:" << filename;
        // Update vehicle list after save is complete for real-time UI refresh
        this->vehicle_update_list();
        qDebug() << "Vehicle list updated after save";
    });

}

void FormGPS::vehicle_load(QString vehicle_name) {
#ifdef __ANDROID__
    QString directoryName = androidDirectory + QCoreApplication::applicationName() + "/Vehicles";
#else
    QString directoryName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                            + "/" + QCoreApplication::applicationName() + "/Vehicles";
#endif

    QDir loadDir(directoryName);
    if (!loadDir.exists()) {
        bool ok = loadDir.mkpath(directoryName);
        if (!ok) {
            qWarning() << "Couldn't create path " << directoryName;
            return;
        }
    }

    if (!loadDir.exists(caseInsensitiveFilename(directoryName, vehicle_name)))
        qWarning() << vehicle_name << " may not exist but will try to load it anyway.";

    QString filename = directoryName + "/" + caseInsensitiveFilename(directoryName, vehicle_name);

    SettingsManager::instance()->loadJson(filename);
}

void FormGPS::vehicle_delete(QString vehicle_name) {
#ifdef __ANDROID__
    QString directoryName = androidDirectory + QCoreApplication::applicationName() + "/Vehicles";
#else
    QString directoryName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                            + "/" + QCoreApplication::applicationName() + "/Vehicles";
#endif

    QDir vehicleDir(directoryName);
    if (vehicleDir.exists()) {
        if (! vehicleDir.remove(caseInsensitiveFilename(directoryName, vehicle_name)))
            qWarning() << "Could not delete vehicle " << vehicle_name;
    }
}

void FormGPS::vehicle_update_list() {
#ifdef __ANDROID__
    QString directoryName = androidDirectory + QCoreApplication::applicationName() + "/Vehicles";
#else
    QString directoryName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                            + "/" + QCoreApplication::applicationName() + "/Vehicles";
#endif

    QDir vehicleDirectory(directoryName);
    if(!vehicleDirectory.exists()) {
        vehicleDirectory.mkpath(directoryName);
    }

    vehicleDirectory.setFilter(QDir::Files);

    QFileInfoList filesList = vehicleDirectory.entryInfoList();

    QList<QVariant> vehicleList;
    QMap<QString, QVariant>vehicle;
    int index = 0;

    for (QFileInfo &file : filesList) {
        vehicle.clear();
        vehicle["index"] = index;
        vehicle["name"] = file.fileName();
        vehicleList.append(vehicle);
        index++;
    }

    CVehicle::instance()->setProperty("vehicle_list", vehicleList);
    emit CVehicle::instance()->vehicle_listChanged();
}

