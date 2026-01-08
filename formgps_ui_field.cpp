// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
// GUI to backend field interface
#include "formgps.h"
#include "qmlutil.h"
#include "classes/settingsmanager.h"
#include <QUrl>
#include <QTimer>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>

#include "cboundarylist.h"
#include "fieldinterface.h"
#include "siminterface.h"
#include "ctrack.h"

void FormGPS::field_update_list() {

#ifdef __ANDROID__
    QString directoryName = androidDirectory + QCoreApplication::applicationName() + "/Fields";
#else
    QString directoryName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                            + "/" + QCoreApplication::applicationName() + "/Fields";
#endif

    // fieldInterface is now a class member, initialized in formgps_ui.cpp
    QList<QVariant> fieldList;
    QMap<QString, QVariant> field;
    int index = 0;

    QDirIterator it(directoryName, QStringList() << "Field.txt", QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()){
        field = FileFieldInfo(it.next());
        if(field.contains("latitude")) {
            field["index"] = index;
            fieldList.append(field);
            index++;
        }
    }

    FieldInterface::instance()->set_field_list(fieldList);
}

void FormGPS::field_close() {
    qDebug() << "field_close";

    // Get current field name and set active field profile for saving
    QString currentField = SettingsManager::instance()->f_currentDir();
    if (!currentField.isEmpty() && currentField != "Default") {
        QString jsonFilename;
#ifdef __ANDROID__
        jsonFilename = androidDirectory + QCoreApplication::applicationName() + "/Fields/" + currentField + "_settings.json";
#else
        jsonFilename = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                      + "/" + QCoreApplication::applicationName() + "/Fields/" + currentField + "_settings.json";
#endif

        // Set active field profile so FileSaveEverythingBeforeClosingField() saves to field JSON
        SettingsManager::instance()->setActiveFieldProfile(jsonFilename);
        qDebug() << "field_close: Set active field profile for saving:" << jsonFilename;
    }

    FileSaveEverythingBeforeClosingField(false);  // Don't save vehicle when closing field

    // Clear field profile after saving
    SettingsManager::instance()->clearActiveFieldProfile();
    qDebug() << "field_close: Cleared field profile after saving";
}

void FormGPS::field_open(QString field_name) {
    qDebug() << "field_open";

    // Phase 6.0.4: PropertyWrapper completely removed - using Qt 6.8 Q_PROPERTY native architecture

    qDebug() << "✅ field_open: AOGInterface ready, proceeding with field operations";

    FileSaveEverythingBeforeClosingField(false);  // Don't save vehicle when opening field
    if (! FileOpenField(field_name)) {
        TimedMessageBox(8000, tr("Saved field does not exist."), QString(tr("Cannot find the requested saved field.")) + " " +
                                                                field_name);

        SettingsManager::instance()->setF_currentDir("Default");
    } else {
        // Field opened successfully, try to load JSON profile if it exists
        field_load_json(field_name);
    }
}

void FormGPS::field_new(QString field_name) {
    // Phase 6.0.4: PropertyWrapper completely removed - using Qt 6.8 Q_PROPERTY native architecture

    //assume the GUI will vet the name a little bit
    lock.lockForWrite();

    // CRITICAL DEADLOCK FIX: Save current field AFTER releasing lock (same as field_close fix)
    // FileSaveEverythingBeforeClosingField() needs to acquire mutex but lock is already held
    lock.unlock();
    qDebug() << "Lock released, calling FileSaveEverythingBeforeClosingField(false)";
    FileSaveEverythingBeforeClosingField(false);  // Don't save vehicle to avoid async deadlock
    qDebug() << "FileSaveEverythingBeforeClosingField() completed, no async ops - re-acquiring lock";
    lock.lockForWrite();

    currentFieldDirectory = field_name.trimmed();
    SettingsManager::instance()->setF_currentDir(currentFieldDirectory);
    JobNew();

    // Phase 6.3.1: Use PropertyWrapper for safe property access
    this->setLatStart(pn.latitude);
    // Phase 6.3.1: Use PropertyWrapper for safe property access
    this->setLonStart(pn.longitude);
    // Phase 6.3.1: Use PropertyWrapper for safe QObject access
    pn.SetLocalMetersPerDegree(this);

    FileCreateField();
    FileCreateSections();
    FileCreateRecPath();
    FileCreateContour();
    FileCreateElevation();
    FileSaveFlags();
    FileCreateBoundary();
    FileSaveTram();
    lock.unlock();
}

void FormGPS::field_new_from(QString existing, QString field_name, int flags) {
    qDebug() << "field_new_from - REFACTORED: save first, then create with single lock";

    // STEP 1: Save current field WITHOUT any lock (cleaner approach)
    qDebug() << "Saving current field before creating new one";
    FileSaveEverythingBeforeClosingField(false);  // Don't save vehicle to avoid async operations
    qDebug() << "Current field saved, proceeding to create new field";

    // STEP 2: Load existing field BEFORE acquiring lock (FileOpenField has its own locks)
    qDebug() << "Before FileOpenField(" << existing << ")";
    if (! FileOpenField(existing,flags)) { //load whatever is requested from existing field
        TimedMessageBox(8000, tr("Existing field cannot be found"), QString(tr("Cannot find the existing saved field.")) + " " +
                                                                existing);
    }
    qDebug() << "After FileOpenField, acquiring lock for field creation operations";

    // STEP 3: Create new field with lock
    lock.lockForWrite();
    qDebug() << "Lock acquired, changing to new name:" << field_name;

    //change to new name
    currentFieldDirectory = field_name;
    qDebug() << "Before SettingsManager setValue";
    SettingsManager::instance()->setF_currentDir(currentFieldDirectory);
    qDebug() << "After SettingsManager setValue";

    FileCreateField();
    FileCreateSections();
    FileCreateElevation();
    FileSaveFlags();
    FileSaveABLines();
    FileSaveCurveLines();

    contourSaveList.clear();
    contourSaveList.append(ct.ptList);
    FileSaveContour();

    FileSaveRecPath();
    FileSaveTram();

    //some how we have to write the existing patches to the disk.
    //FileSaveSections only write pending triangles

    for(QSharedPointer<PatchTriangleList> &l: tool.triStrip[0].patchList) {
        tool.patchSaveList.append(l);
    }
    FileSaveSections();
    lock.unlock();
    qDebug() << "field_new_from completed successfully";
}

bool parseDouble(const std::string& input, double& output) {
    std::string cleaned = input;

    // Replace comma with dot for decimal point compatibility
    std::replace(cleaned.begin(), cleaned.end(), ',', '.');

    std::istringstream iss(cleaned);
    iss.imbue(std::locale::classic()); // Ensures '.' is the decimal point

    iss >> output;

    // Check for parsing success and no extra characters
    return !iss.fail() && iss.eof();
}

void FormGPS::FindLatLon(QString filename)
{
    qDebug() << "Finding average Lat/Lon from KML file:" << filename;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Error opening file:" << file.errorString();
        return;
    }

    QTextStream stream(&file);
    QString line;
    QString coordinates;

    while (!stream.atEnd()) {
        line = stream.readLine().trimmed();

        int startIndex = line.indexOf(QLatin1String("<coordinates>"));
        if (startIndex != -1) {
            // Found <coordinates> tag — extract content
            while (true) {
                int endIndex = line.indexOf(QLatin1String("</coordinates>"));
                if (endIndex == -1) {
                    // No closing tag in this line
                    if (startIndex == -1) {
                        coordinates += line;
                    } else {
                        coordinates += QStringView(line).mid(startIndex + 13); // Skip "<coordinates>"
                    }
                } else {
                    // Closing tag found
                    if (startIndex == -1) {
                        coordinates += QStringView(line).left(endIndex);
                    } else {
                        coordinates += QStringView(line).mid(startIndex + 13, endIndex - (startIndex + 13));
                    }
                    break;
                }

                if (stream.atEnd()) break;
                line = stream.readLine().trimmed();
                startIndex = -1; // reset for continuation lines
            }

            // Split coordinates by whitespace (spaces, tabs, newlines)
            QStringList coordList = coordinates.split(QRegularExpression(QStringLiteral("\\s+")),
                                                      Qt::SkipEmptyParts);

            if (coordList.size() <= 2) {
                qWarning() << "Error reading KML: Too few coordinate points.";
                file.close();
                return;
            }

            double totalLat = 0.0;
            double totalLon = 0.0;
            int validCount = 0;

            for (const QString& coord : std::as_const(coordList)) {
                if (coord.length() < 3) continue;

                int comma1 = coord.indexOf(QLatin1Char(','));
                int comma2 = coord.indexOf(QLatin1Char(','), comma1 + 1);

                if (comma1 == -1 || comma2 == -1) continue;

                QString lonStr = coord.left(comma1);
                QString latStr = coord.mid(comma1 + 1, comma2 - comma1 - 1);

                bool okLon = false, okLat = false;
                double lon = lonStr.toDouble(&okLon);
                double lat = latStr.toDouble(&okLat);

                if (okLon && okLat) {
                    totalLon += lon;
                    totalLat += lat;
                    ++validCount;
                }
            }

            if (validCount > 0) {
                lonK = totalLon / validCount;
                latK = totalLat / validCount;
                qDebug() << "Average Lat:" << latK << "Lon:" << lonK;
            } else {
                qWarning() << "No valid coordinates found in KML.";
            }

            break; // Process only the first <coordinates> block
        }
    }

    file.close();
}

void FormGPS::LoadKMLBoundary(QString filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Error opening file:" << file.errorString();
        return;
    }

    QTextStream stream(&file);
    QString line;
    QString coordinates;

    while (!stream.atEnd()) {
        line = stream.readLine().trimmed();

        int startIndex = line.indexOf(QLatin1String("<coordinates>"));
        if (startIndex != -1) {
            // Found opening tag
            while (true) {
                int endIndex = line.indexOf(QLatin1String("</coordinates>"));
                if (endIndex == -1) {
                    if (startIndex == -1) {
                        coordinates += line;
                    } else {
                        coordinates += QStringView(line).mid(startIndex + 13); // Skip "<coordinates>"
                    }
                } else {
                    if (startIndex == -1) {
                        coordinates += QStringView(line).left(endIndex);
                    } else {
                        coordinates += QStringView(line).mid(startIndex + 13, endIndex - (startIndex + 13));
                    }
                    break;
                }

                if (stream.atEnd()) break;
                line = stream.readLine().trimmed();
                startIndex = -1; // reset for subsequent lines
            }

            // Split coordinates by whitespace
            QStringList numberSets = coordinates.split(QRegularExpression(QStringLiteral("\\s+")),
                                                       Qt::SkipEmptyParts);

            if (numberSets.size() > 2) {
                double latK = 0.0, lonK = 0.0;
                CBoundaryList New;

                for (const QString& coord : std::as_const(numberSets)) {
                    if (coord.length() < 3) continue;

                    qDebug() << coord;

                    int comma1 = coord.indexOf(QLatin1Char(','));
                    int comma2 = coord.indexOf(QLatin1Char(','), comma1 + 1);

                    if (comma1 == -1 || comma2 == -1) continue;

                    QString lonStr = coord.left(comma1);
                    QString latStr = coord.mid(comma1 + 1, comma2 - comma1 - 1);

                    bool ok1 = false, ok2 = false;
                    double lonVal = lonStr.toDouble(&ok1);
                    double latVal = latStr.toDouble(&ok2);

                    if (!ok1 || !ok2) continue;

                    latK = latVal;
                    lonK = lonVal;

                    double easting = 0.0, northing = 0.0;
                    pn.ConvertWGS84ToLocal(latK, lonK, northing, easting, this);
                    Vec3 temp(easting, northing, 0);
                    New.fenceLine.append(temp);
                }

                // Build the boundary: clockwise for outer, counter-clockwise for inner
                New.CalculateFenceArea(bnd.bndList.count());
                New.FixFenceLine(bnd.bndList.count());
                bnd.bndList.append(New);

            } else {
                qWarning() << "Error reading KML: Too few coordinate points.";
                file.close();
                return;
            }

            break; // Process only the first <coordinates> block
        }
    }

    file.close();
}

void FormGPS::field_new_from_KML(QString field_name, QString file_name) {
    qDebug() << field_name << " " << file_name;

    // Phase 6.0.4: PropertyWrapper completely removed - using Qt 6.8 Q_PROPERTY native architecture

    //assume the GUI will vet the name a little bit
    field_close();
    lock.lockForWrite();
    FileSaveEverythingBeforeClosingField(false);  // Don't save vehicle when creating field from KML
    currentFieldDirectory = field_name.trimmed();
    SettingsManager::instance()->setF_currentDir(currentFieldDirectory);
    JobNew();
    // Convert QML file URL to local path using QUrl for robustness
    QUrl fileUrl(file_name);
    QString localPath = fileUrl.toLocalFile();
    if (localPath.isEmpty()) {
        // Fallback for manual parsing if QUrl fails
        file_name.remove("file:///");
        if (file_name.startsWith("/") && file_name.length() > 3 && file_name[2] == ':') {
            file_name.remove(0, 1);
        }
        localPath = file_name;
    }
    file_name = localPath;
    FindLatLon(file_name);

    // Phase 6.3.1: Use PropertyWrapper for safe property access
    this->setLatStart(latK);
    // Phase 6.3.1: Use PropertyWrapper for safe property access
    this->setLonStart(lonK);
    if (SimInterface::instance()->isRunning())
        {
            pn.latitude = this->latStart();
            pn.longitude = this->lonStart();

            SettingsManager::instance()->setGps_simLatitude(this->latStart());
            SettingsManager::instance()->setGps_simLongitude(this->lonStart());
            SimInterface::instance()->reset();
        }
    // Phase 6.3.1: Use PropertyWrapper for safe QObject access
    pn.SetLocalMetersPerDegree(this);


    FileCreateField();
    FileCreateSections();
    FileCreateRecPath();
    FileCreateContour();
    FileCreateElevation();
    FileSaveFlags();
    FileSaveABLines();     // Create empty AB lines files
    FileSaveCurveLines();  // Create empty curve lines files
    FileCreateBoundary();
    FileSaveTram();
    FileSaveHeadland();    // Create empty Headland.txt to prevent load errors

    LoadKMLBoundary(file_name);
    lock.unlock();
}

void FormGPS::field_delete(QString field_name) {
#ifdef __ANDROID__
    QString directoryName = androidDirectory + QCoreApplication::applicationName() + "/Fields/" + field_name;
#else
    QString directoryName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                            + "/" + QCoreApplication::applicationName() + "/Fields/" + field_name;
#endif

    QDir fieldDir(directoryName);

    if(! fieldDir.exists()) {
        TimedMessageBox(8000,tr("Cannot find saved field"),QString(tr("Cannot find saved field to delete.")) + " " + field_name);
        return;
    }
    if(!QFile::moveToTrash(directoryName)){
        fieldDir.removeRecursively();
    }
    field_update_list();
}

void FormGPS::field_saveas(QString field_name) {
#ifdef __ANDROID__
    QString directoryName = androidDirectory + QCoreApplication::applicationName() + "/Fields";
#else
    QString directoryName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                            + "/" + QCoreApplication::applicationName() + "/Fields";
#endif

    QDir saveDir(directoryName);
    if (!saveDir.exists()) {
        bool ok = saveDir.mkpath(directoryName);
        if (!ok) {
            qWarning() << "Couldn't create path " << directoryName;
            return;
        }
    }

    QString jsonFilename = directoryName + "/" + field_name + "_settings.json";

    // Save current field settings to JSON for auto-sync
    qDebug() << "Field saveas: Scheduling async saveJson:" << jsonFilename;
    QTimer::singleShot(50, this, [this, jsonFilename, field_name]() {
        qDebug() << "Field saveas: Executing async saveJson:" << jsonFilename;
        SettingsManager::instance()->saveJson(jsonFilename);

        // Set as active field profile for future auto-saving
        SettingsManager::instance()->setActiveFieldProfile(jsonFilename);
        qDebug() << "Field saveas: JSON saved and set as active profile:" << jsonFilename;

        // Also save traditional .txt files (existing system)
        // This keeps compatibility with existing Field.txt, Boundary.txt etc.
        this->field_update_list();
        qDebug() << "Field saveas: Field list updated";
    });
}

void FormGPS::field_load_json(QString field_name) {
#ifdef __ANDROID__
    QString directoryName = androidDirectory + QCoreApplication::applicationName() + "/Fields";
#else
    QString directoryName = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                            + "/" + QCoreApplication::applicationName() + "/Fields";
#endif

    QString jsonFilename = directoryName + "/" + field_name + "_settings.json";

    // Check if JSON profile exists
    if (QFile::exists(jsonFilename)) {
        qDebug() << "Field load JSON starting:" << jsonFilename;
        SettingsManager::instance()->loadJson(jsonFilename);

        // Set as active field profile for auto-saving
        SettingsManager::instance()->setActiveFieldProfile(jsonFilename);
        qDebug() << "Field JSON loaded and set as active profile:" << jsonFilename;
    } else {
        qDebug() << "Field JSON profile not found, using traditional .txt system:" << jsonFilename;
    }
}
void FormGPS::FindLatLonGeoJSON(QString filename)
{
    qDebug() << "Finding average Lat/Lon from GeoJSON file:" << filename;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Error opening GeoJSON file:" << file.errorString();
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull()) {
        qWarning() << "Invalid GeoJSON file";
        return;
    }

    QJsonObject root = doc.object();

    // Check if it's a FeatureCollection or single Feature
    QJsonArray features;
    if (root.contains("type") && root["type"].toString() == "FeatureCollection") {
        features = root["features"].toArray();
    } else if (root.contains("type") && root["type"].toString() == "Feature") {
        features = QJsonArray{root};
    } else {
        // Assume it's a Geometry object
        features = QJsonArray{root};
    }

    double totalLat = 0.0;
    double totalLon = 0.0;
    int validCount = 0;

    // Recursive function to extract coordinates from geometry
    std::function<void(const QJsonValue&)> extractCoordinates;
    extractCoordinates = [&](const QJsonValue& geometryValue) {
        if (geometryValue.isObject()) {
            QJsonObject geomObj = geometryValue.toObject();
            QString type = geomObj["type"].toString();

            if (type == "Point") {
                QJsonArray coords = geomObj["coordinates"].toArray();
                if (coords.size() >= 2) {
                    double lon = coords[0].toDouble();
                    double lat = coords[1].toDouble();
                    totalLon += lon;
                    totalLat += lat;
                    validCount++;
                }
            }
            else if (type == "LineString" || type == "MultiPoint") {
                QJsonArray coords = geomObj["coordinates"].toArray();
                for (const QJsonValue& coord : coords) {
                    QJsonArray point = coord.toArray();
                    if (point.size() >= 2) {
                        double lon = point[0].toDouble();
                        double lat = point[1].toDouble();
                        totalLon += lon;
                        totalLat += lat;
                        validCount++;
                    }
                }
            }
            else if (type == "Polygon" || type == "MultiPolygon") {
                QJsonArray polygons;
                if (type == "Polygon") {
                    polygons = QJsonArray{geomObj["coordinates"].toArray()};
                } else {
                    polygons = geomObj["coordinates"].toArray();
                }

                for (const QJsonValue& polygon : polygons) {
                    QJsonArray rings = polygon.toArray();
                    if (!rings.isEmpty()) {
                        QJsonArray outerRing = rings[0].toArray(); // First ring is outer boundary
                        for (const QJsonValue& coord : outerRing) {
                            QJsonArray point = coord.toArray();
                            if (point.size() >= 2) {
                                double lon = point[0].toDouble();
                                double lat = point[1].toDouble();
                                totalLon += lon;
                                totalLat += lat;
                                validCount++;
                            }
                        }
                    }
                }
            }
            else if (type == "MultiLineString") {
                QJsonArray lines = geomObj["coordinates"].toArray();
                for (const QJsonValue& line : lines) {
                    QJsonArray coords = line.toArray();
                    for (const QJsonValue& coord : coords) {
                        QJsonArray point = coord.toArray();
                        if (point.size() >= 2) {
                            double lon = point[0].toDouble();
                            double lat = point[1].toDouble();
                            totalLon += lon;
                            totalLat += lat;
                            validCount++;
                        }
                    }
                }
            }
            else if (type == "GeometryCollection") {
                QJsonArray geometries = geomObj["geometries"].toArray();
                for (const QJsonValue& geom : geometries) {
                    extractCoordinates(geom);
                }
            }
        }
    };

    // Process all features
    for (const QJsonValue& featureValue : features) {
        QJsonObject feature = featureValue.toObject();

        if (feature.contains("geometry") && !feature["geometry"].isNull()) {
            extractCoordinates(feature["geometry"]);
        } else if (feature.contains("coordinates")) {
            // Direct geometry
            extractCoordinates(feature);
        }
    }

    if (validCount > 0) {
        lonK = totalLon / validCount;
        latK = totalLat / validCount;
        qDebug() << "Average Lat:" << static_cast<double>(latK) << "Lon:" << static_cast<double>(lonK) << "from" << validCount << "points";
    } else {
        qWarning() << "No valid coordinates found in GeoJSON.";
        latK = static_cast<double>(pn.latitude);
        lonK = static_cast<double>(pn.longitude);
    }
}

QVector3D FormGPS::getColorByRate(double rate) { // need UI
    if (rate < 0.1) {
        return QVector3D(1.0f, 0.0f, 0.0f); // Красный
    } else if (rate < 0.2) {
        return QVector3D(1.0f, 0.5f, 0.0f); // Оранжевый
    } else if (rate < 0.35) {
        return QVector3D(1.0f, 1.0f, 0.0f); // Желтый
    } else if (rate < 0.5) {
        return QVector3D(0.1f, 0.8f, 0.1f); // Светло-зеленый
    } else {
        return QVector3D(0.0f, 0.5f, 0.0f); // Темно-зеленый
    }
}

void FormGPS::LoadGeoJSONBoundary(QString filename) {
    qDebug() << "Opening GeoJSON file:" << filename;

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Error opening GeoJSON file:" << file.errorString();
        return;
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (doc.isNull()) {
        qWarning() << "Invalid GeoJSON file";
        return;
    }

    QJsonObject root = doc.object();

    // Check if it's a FeatureCollection or single Feature
    QJsonArray features;
    if (root.contains("type") && root["type"].toString() == "FeatureCollection") {
        features = root["features"].toArray();
    } else if (root.contains("type") && root["type"].toString() == "Feature") {
        features = QJsonArray{root};
    } else {
        features = QJsonArray{root};
    }

    // Clear existing patches
    tool.triStrip[0].patchList.clear();
    tool.patchSaveList.clear();

    // Process all features
    for (const QJsonValue& featureValue : features) {
        QJsonObject feature = featureValue.toObject();
        QJsonValue geometryValue;

        if (feature.contains("geometry") && !feature["geometry"].isNull()) {
            geometryValue = feature["geometry"];
        } else if (feature.contains("coordinates")) {
            geometryValue = feature;
        } else {
            continue;
        }

        if (geometryValue.isObject()) {
            QJsonObject geometry = geometryValue.toObject();
            QString type = geometry["type"].toString();

            // Handle MultiPolygon as boundary
            if (type == "MultiPolygon") {
                QJsonArray polygons = geometry["coordinates"].toArray();

                // Process each polygon in MultiPolygon as separate boundary
                for (const QJsonValue& polygonValue : polygons) {
                    CBoundaryList New;
                    QJsonArray rings = polygonValue.toArray();

                    // Take only the outer ring (first ring) for boundary
                    if (!rings.isEmpty()) {
                        QJsonArray outerRing = rings[0].toArray();
                        for (const QJsonValue& coord : outerRing) {
                            QJsonArray point = coord.toArray();
                            if (point.size() >= 2) {
                                double lon = point[0].toDouble();
                                double lat = point[1].toDouble();

                                double easting = 0.0, northing = 0.0;
                                pn.ConvertWGS84ToLocal(lat, lon, northing, easting, this);
                                Vec3 temp(easting, northing, 0);
                                New.fenceLine.append(temp);
                            }
                        }

                        if (!New.fenceLine.isEmpty()) {
                            // Build the boundary
                            New.CalculateFenceArea(bnd.bndList.count());
                            New.FixFenceLine(bnd.bndList.count());
                            bnd.bndList.append(New);
                            qDebug() << "Added boundary from GeoJSON MultiPolygon with" << New.fenceLine.count() << "points";
                        }
                    }
                }
            }
            // Handle Polygon as colored patches based on RATE
            else if (type == "Polygon") {
                // Extract RATE from feature properties
                double rate = 0.0;
                if (feature.contains("properties")) {
                    QJsonObject properties = feature["properties"].toObject();
                    if (properties.contains("RATE")) {
                        rate = properties["RATE"].toDouble();
                        qDebug() << "Found RATE:" << rate;
                    }
                }

                // Get color based on RATE
                QVector3D color = getColorByRate(rate);

                QJsonArray rings = geometry["coordinates"].toArray();
                if (rings.isEmpty()) continue;

                QJsonArray outerRing = rings[0].toArray();

                // Convert all points to local coordinates
                QVector<QVector3D> points;
                for (const QJsonValue& coord : outerRing) {
                    QJsonArray point = coord.toArray();
                    if (point.size() >= 2) {
                        double lon = point[0].toDouble();
                        double lat = point[1].toDouble();

                        double easting = 0.0, northing = 0.0;
                        pn.ConvertWGS84ToLocal(lat, lon, northing, easting, this);
                        points.append(QVector3D(easting, northing, 0.0f));
                    }
                }

                // Skip if not enough points
                if (points.size() < 3) {
                    qDebug() << "Skipping polygon with less than 3 points";
                    return;
                }

                // Check if polygon is closed and remove last point if needed
                if (points.size() >= 2) {
                    QVector3D first = points.first();
                    QVector3D last = points.last();
                    double distance = sqrt(pow(first.x() - last.x(), 2) + pow(first.y() - last.y(), 2));
                    if (distance < 0.1) {
                        points.removeLast();
                        qDebug() << "Removed duplicate closing point";
                    }
                }

                if (points.size() < 3) {
                    qDebug() << "Skipping polygon with less than 3 points after closing check";
                    return;
                }

                // Create triangle strip from polygon using fan triangulation
                QVector3D center = points[0]; // Use first point as center for fan

                for (int i = 1; i < points.size() - 1; i++) {
                    // Create a separate patch for each triangle
                    QSharedPointer<PatchTriangleList> singleTriangle = QSharedPointer<PatchTriangleList>(new PatchTriangleList);

                    // Use the color based on RATE
                    singleTriangle->append(color);

                    // Add triangle vertices: center, current point, next point
                    singleTriangle->append(center);
                    singleTriangle->append(points[i]);
                    singleTriangle->append(points[i + 1]);

                    // Add to patch lists
                    tool.triStrip[0].patchList.append(singleTriangle);
                    tool.patchSaveList.append(singleTriangle);
                }

                qDebug() << "Added Polygon with RATE" << rate << "as" << (points.size() - 2) << "colored triangle patches";
            }
            // Handle LineString as AB Line
            else if (type == "LineString") {
                qDebug() << "Processing LineString as AB Line";

                QJsonArray coords = geometry["coordinates"].toArray();
                if (coords.size() < 2) {
                    qDebug() << "LineString has less than 2 points, skipping";
                    return;
                }

                // Extract line properties
                QString lineName = "GeoJSON Line";
                bool isVisible = true;
                double nudgeDistance = 0.0;

                if (feature.contains("properties")) {
                    QJsonObject properties = feature["properties"].toObject();
                    if (properties.contains("name")) {
                        lineName = properties["name"].toString();
                    }
                }

                // Convert coordinates to local system
                QVector<Vec3> linePoints;
                for (const QJsonValue& coord : coords) {
                    QJsonArray point = coord.toArray();
                    if (point.size() >= 2) {
                        double lon = point[0].toDouble();
                        double lat = point[1].toDouble();

                        double easting = 0.0, northing = 0.0;
                        pn.ConvertWGS84ToLocal(lat, lon, northing, easting, this);
                        linePoints.append(Vec3(easting, northing, 0));
                    }
                }

                if (linePoints.size() < 2) {
                    qDebug() << "Failed to convert LineString coordinates";
                    return;
                }

                // Create AB Line from first and last points
                CTrk newTrack;
                newTrack.mode = (int)TrackMode::AB;
                newTrack.name = lineName;
                newTrack.isVisible = isVisible;
                newTrack.nudgeDistance = nudgeDistance;
                newTrack.ptA = Vec2(linePoints.first().easting, linePoints.first().northing);
                newTrack.ptB = Vec2(linePoints.last().easting, linePoints.last().northing);

                // Calculate heading from A to B
                newTrack.heading = atan2(newTrack.ptB.easting - newTrack.ptA.easting,
                                         newTrack.ptB.northing - newTrack.ptA.northing);
                if (newTrack.heading < 0) newTrack.heading += glm::twoPI;

                // Calculate end points (extended line)
                newTrack.endPtA.easting = newTrack.ptA.easting - (sin(newTrack.heading) * 1000);
                newTrack.endPtA.northing = newTrack.ptA.northing - (cos(newTrack.heading) * 1000);
                newTrack.endPtB.easting = newTrack.ptB.easting + (sin(newTrack.heading) * 1000);
                newTrack.endPtB.northing = newTrack.ptB.northing + (cos(newTrack.heading) * 1000);

                // Add to tracks
                track.gArr.append(newTrack);
                track.reloadModel();

                qDebug() << "Added AB Line from GeoJSON:" << lineName
                         << "with" << linePoints.size() << "points, heading:"
                         << glm::toDegrees(newTrack.heading) << "degrees";
            }

            // Handle MultiLineString as Curve
            else if (type == "MultiLineString") {
                qDebug() << "Processing MultiLineString as Curve";

                QJsonArray linePoints = geometry["coordinates"].toArray();
                if (linePoints.isEmpty()) {
                    qDebug() << "MultiLineString has no lines, skipping";
                    return;
                }

                // Extract curve properties
                QString curveName = "GeoJSON Curve";
                bool isVisible = true;
                double nudgeDistance = 0.0;

                if (feature.contains("properties")) {
                    QJsonObject properties = feature["properties"].toObject();
                    if (properties.contains("name")) {
                        curveName = properties["name"].toString();
                    }
                }

                // Собираем ВСЕ точки из ВСЕХ линий в один массив
                QVector<Vec3> allCurvePoints;

                // Process all lines in MultiLineString
                for (const QJsonValue& lineValue : linePoints) {
                    QJsonArray coords = lineValue.toArray();
                    if (coords.size() < 2) {
                        qDebug() << "Line in MultiLineString has less than 2 points, skipping this line";
                        return;
                    }

                    // Добавляем все точки из этой линии в общий массив
                    for (const QJsonValue& coord : coords) {
                        QJsonArray point = coord.toArray();
                        if (point.size() >= 2) {
                            double lon = point[0].toDouble();
                            double lat = point[1].toDouble();

                            double easting = 0.0, northing = 0.0;
                            pn.ConvertWGS84ToLocal(lat, lon, northing, easting, this);
                            allCurvePoints.append(Vec3(easting, northing, 0));
                        }
                    }
                }

                if (allCurvePoints.size() > 3) {

                CTrk newTrack;
                newTrack.mode = (int)TrackMode::Curve;
                newTrack.name = curveName;
                newTrack.isVisible = isVisible;
                newTrack.nudgeDistance = nudgeDistance;

                track.curve.MakePointMinimumSpacing(allCurvePoints, 1.6);
                track.curve.CalculateHeadings(allCurvePoints);

                // Set A and B points as first and last points
                newTrack.ptA = Vec2(allCurvePoints.first().easting, allCurvePoints.first().northing);
                newTrack.ptB = Vec2(allCurvePoints.last().easting, allCurvePoints.last().northing);

                // Calculate average heading
                double x = 0, y = 0;
                for (int i = 0; i < allCurvePoints.size() - 1; i++) {
                    double heading = atan2(allCurvePoints[i+1].easting - allCurvePoints[i].easting,
                                           allCurvePoints[i+1].northing - allCurvePoints[i].northing);
                    if (heading < 0) heading += glm::twoPI;
                    x += cos(heading);
                    y += sin(heading);
                }
                x /= (allCurvePoints.size() - 1);
                y /= (allCurvePoints.size() - 1);
                newTrack.heading = atan2(y, x);
                if (newTrack.heading < 0) newTrack.heading += glm::twoPI;

                track.curve.AddFirstLastPoints(allCurvePoints);

                //write out the Curve Points
                for (Vec3 &item : allCurvePoints)
                {
                    newTrack.curvePts.append(item);
                }

                // Add to tracks
                track.gArr.append(newTrack);
                track.reloadModel();
                }
            }
        }
    }

    // Auto-select first track if any were loaded
    if (!track.gArr.isEmpty()) {
        track.select(0);
    }
}

void FormGPS::field_new_from_GeoJSON(QString field_name, QString file_name) {
    qDebug() << "GeoJSON open " << field_name << " " << file_name;

    lock.lockForWrite();
    FileSaveEverythingBeforeClosingField();
    currentFieldDirectory = field_name.trimmed();
    SettingsManager::instance()->setF_currentDir(currentFieldDirectory);
    JobNew();

    QUrl fileUrl(file_name);
    QString localPath = fileUrl.toLocalFile();

    FindLatLonGeoJSON(localPath);
    // Phase 6.3.1: Use PropertyWrapper for safe property access
    this->setLatStart(latK);
    // Phase 6.3.1: Use PropertyWrapper for safe property access
    this->setLonStart(lonK);

    if (SimInterface::instance()->isRunning()){
        pn.latitude = this->latStart();
        pn.longitude = this->lonStart();

    SettingsManager::instance()->setGps_simLatitude(this->latStart());
    SettingsManager::instance()->setGps_simLongitude(this->lonStart());
    }

    pn.SetLocalMetersPerDegree(this);
    FileCreateField();
    FileCreateSections();
    FileCreateRecPath();
    FileCreateContour();
    FileCreateElevation();
    FileSaveFlags();
    FileCreateBoundary();
    FileSaveTram();
    LoadGeoJSONBoundary(localPath);
   // boundary_stop();
    lock.unlock();
}
