// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
#include "agsharedownloader.h"
#include "formgps.h"
#include "qmlutil.h"
#include "newsettings.h"
#include "cboundarylist.h"
#include "cnmea.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QDateTime>
#include <QTextStream>
#include <QDebug>

CAgShareDownloader::CAgShareDownloader(QObject *parent)
    : QObject(parent)
    , m_client(new AgShareClient(this))
    , m_networkManager(new QNetworkAccessManager(this))
    , m_isDownloading(false)
    , m_currentDownloadIndex(0)
    , m_forceOverwrite(false)
{
    // Connect AgShareClient signals
    connect(m_client, &AgShareClient::downloadFinished,
            this, &CAgShareDownloader::onDownloadFinished);
    connect(m_client, &AgShareClient::ownFieldsReceived,
            this, &CAgShareDownloader::onOwnFieldsReceived);
    connect(m_client, &AgShareClient::downloadProgress,
            this, &CAgShareDownloader::onDownloadProgress);
    connect(m_client, &AgShareClient::networkError,
            this, &CAgShareDownloader::onNetworkError);
    connect(m_client, &AgShareClient::jsonError,
            this, &CAgShareDownloader::onJsonError);
}

CAgShareDownloader::~CAgShareDownloader()
{
    // Все запросы будут удалены автоматически через родительство
}

void CAgShareDownloader::downloadField(const QString& fieldId)
{
    if (m_isDownloading) {
        setLastError("Download already in progress");
        emit downloadFinished(false, "Download already in progress");
        return;
    }

    QUuid id = QUuid(fieldId);
    if (id.isNull()) {
        setLastError("Invalid field ID");
        emit downloadFinished(false, "Invalid field ID");
        return;
    }

    setDownloading(true);
    setLastError("");
    setStatusMessage(QString("Downloading field %1...").arg(fieldId));

    emit downloadStarted();

    // Очищаем список полей для загрузки
    m_fieldsToDownload.clear();
    m_currentDownloadIndex = 0;

    // Добавляем поле в список для загрузки
    QJsonObject fieldObj;
    fieldObj["id"] = fieldId;
    m_fieldsToDownload.append(fieldObj);

    // Начинаем загрузку
    downloadNextField();
}

void CAgShareDownloader::downloadAllFields(bool forceOverwrite)
{
    if (m_isDownloading) {
        setLastError("Download already in progress");
        emit downloadFinished(false, "Download already in progress");
        return;
    }

    setDownloading(true);
    setLastError("");
    setStatusMessage("Fetching field list...");
    m_forceOverwrite = forceOverwrite;

    emit downloadStarted();

    // Сначала получаем список полей
    m_client->getOwnFields();
}

void CAgShareDownloader::refreshFieldList()
{
    if (m_isDownloading) {
        setLastError("Operation already in progress");
        emit fieldListUpdated(false, "Operation already in progress");
        return;
    }

    setStatusMessage("Refreshing field list...");
    m_client->getOwnFields();
}

void CAgShareDownloader::cancelDownload()
{
    if (m_isDownloading) {
        setStatusMessage("Download cancelled");
        setDownloading(false);
        emit downloadFinished(false, "Download cancelled by user");
    }
}

bool CAgShareDownloader::importFieldToGPS(const QJsonObject& fieldData, FormGPS* gps)
{
    if (!gps) {
        qWarning() << "AgShareDownloader: GPS is null";
        return false;
    }

    // Обрабатываем скачанные данные
    return processDownloadedField(fieldData, gps);
}

QString CAgShareDownloader::getFieldsDirectory()
{
#ifdef __ANDROID__
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
           + "/" + QCoreApplication::applicationName() + "/Fields/";
#else
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
           + "/" + QCoreApplication::applicationName() + "/Fields/";
#endif
}

QString CAgShareDownloader::createSafeFileName(const QString& name)
{
    // Удаляем недопустимые символы
    QString safeName = name;
    safeName.replace(QRegularExpression("[\\\\/:*?\"<>|]"), "_");
    safeName = safeName.trimmed();

    if (safeName.isEmpty()) {
        safeName = "AgShare_Field_" + QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    }

    return safeName;
}

bool CAgShareDownloader::fieldExistsLocally(const QString& fieldName, const QString& fieldId)
{
    QString safeName = createSafeFileName(fieldName);
    QString fieldsDir = getFieldsDirectory();
    QString fieldDir = fieldsDir + safeName;
    QString idPath = fieldDir + "/agshare.txt";

    QFile file(idPath);
    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString existingId = stream.readAll().trimmed();
        file.close();

        // Если не указан ID для проверки, просто проверяем существование
        if (fieldId.isEmpty()) {
            return true;
        }

        // Проверяем соответствие ID
        return existingId == fieldId;
    }

    return false;
}

void CAgShareDownloader::downloadNextField()
{
    if (m_currentDownloadIndex >= m_fieldsToDownload.size()) {
        // Все поля загружены
        setDownloading(false);
        setStatusMessage(QString("Downloaded %1 fields").arg(m_fieldsToDownload.size()));
        emit downloadFinished(true, QString("Downloaded %1 fields").arg(m_fieldsToDownload.size()));
        return;
    }

    QJsonObject fieldInfo = m_fieldsToDownload[m_currentDownloadIndex];
    QString fieldId = fieldInfo["id"].toString();
    QString fieldName = fieldInfo["name"].toString(fieldId);

    setStatusMessage(QString("Downloading %1...").arg(fieldName));
    emit downloadProgress(fieldName, m_currentDownloadIndex + 1, m_fieldsToDownload.size());

    // Загружаем поле
    m_client->downloadField(QUuid(fieldId));
}

void CAgShareDownloader::onDownloadFinished(bool success, const QString& message, const QJsonObject& fieldData)
{
    if (!success) {
        handleError(QString("Download failed: %1").arg(message));
        return;
    }

    QString fieldId = fieldData["id"].toString();
    QString fieldName = fieldData["name"].toString();

    setStatusMessage(QString("Processing %1...").arg(fieldName));

    // Проверяем, существует ли поле локально
    if (!m_forceOverwrite && fieldExistsLocally(fieldName, fieldId)) {
        qDebug() << "AgShareDownloader: Field" << fieldName << "already exists, skipping";
        m_currentDownloadIndex++;
        downloadNextField();
        return;
    }

    // Сохраняем поле на диск
    if (saveFieldToDisk(fieldData)) {
        qDebug() << "AgShareDownloader: Field" << fieldName << "saved successfully";
        m_currentDownloadIndex++;
        downloadNextField();
    } else {
        handleError(QString("Failed to save field %1").arg(fieldName));
    }
}

void CAgShareDownloader::onOwnFieldsReceived(bool success, const QString& message, const QJsonArray& fields)
{
    if (!success) {
        handleError(QString("Failed to fetch field list: %1").arg(message));
        emit fieldListUpdated(false, message);
        return;
    }

    updateFieldList(fields);

    if (m_isDownloading) {
        // Продолжаем процесс загрузки
        m_fieldsToDownload.clear();
        m_currentDownloadIndex = 0;

        // Преобразуем QJsonArray в список полей для загрузки
        for (const QJsonValue& fieldVal : fields) {
            QJsonObject fieldObj = fieldVal.toObject();
            m_fieldsToDownload.append(fieldObj);
        }

        if (m_fieldsToDownload.isEmpty()) {
            setDownloading(false);
            setStatusMessage("No fields available to download");
            emit downloadFinished(true, "No fields available to download");
        } else {
            setStatusMessage(QString("Found %1 fields to download").arg(m_fieldsToDownload.size()));
            downloadNextField();
        }
    } else {
        // Просто обновляем список
        setStatusMessage(QString("Found %1 fields").arg(fields.size()));
        emit fieldListUpdated(true, QString("Found %1 fields").arg(fields.size()));
    }
}

void CAgShareDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgressDetailed(bytesReceived, bytesTotal);
}

bool CAgShareDownloader::processDownloadedField(const QJsonObject& json, FormGPS* gps)
{
    try {
        // Парсим DTO из JSON
        auto agShareDto = AgShareDtos::AgShareFieldDto::fromJson(json);

        if (!agShareDto.isValid()) {
            qWarning() << "AgShareDownloader: Invalid field DTO";
            return false;
        }

        // Если передан GPS, используем его CNMEA для конвертации
        CNMEA* nmea = gps ? &gps->pn : nullptr;

        // Конвертируем в локальный снапшот
        auto snapshot = AgShareDtos::DtoFactory::createFromAgShareField(agShareDto, nmea);

        if (!snapshot.isValid()) {
            qWarning() << "AgShareDownloader: Invalid field snapshot";
            return false;
        }

        // Сохраняем поле локально
        return saveFieldToDisk(json, gps);

    } catch (const std::exception& e) {
        qWarning() << "AgShareDownloader: Exception processing field:" << e.what();
        return false;
    }
}

bool CAgShareDownloader::saveFieldToDisk(const QJsonObject& fieldData, FormGPS* gps)
{
    QString fieldName = fieldData["name"].toString();
    QString fieldId = fieldData["id"].toString();

    if (fieldName.isEmpty() || fieldId.isEmpty()) {
        qWarning() << "AgShareDownloader: Invalid field data (missing name or ID)";
        return false;
    }

    // Создаем безопасное имя директории
    QString safeName = createSafeFileName(fieldName);
    QString fieldsDir = getFieldsDirectory();
    QString fieldDir = fieldsDir + safeName;

    // Создаем директорию
    if (!createFieldDirectory(fieldDir)) {
        qWarning() << "AgShareDownloader: Failed to create directory:" << fieldDir;
        return false;
    }

    // Сохраняем все файлы
    bool success = true;

    // 1. agshare.txt с ID поля
    success = success && writeAgShareId(fieldDir, fieldId);

    // 2. Field.txt с основной информацией
    QJsonObject origin = fieldData["origin"].toObject();
    success = success && writeFieldTxt(fieldDir, origin, fieldName);

    // 3. Boundary.txt с границами
    QJsonObject boundary = fieldData["boundary"].toObject();
    CNMEA* nmea = gps ? &gps->pn : nullptr;
    success = success && writeBoundaryTxt(fieldDir, boundary, nmea);

    // 4. TrackLines.txt с AB-линиями
    QJsonArray abLines = fieldData["abLines"].toArray();
    success = success && writeTrackLinesTxt(fieldDir, abLines, nmea);

    // 5. Статические файлы
    success = success && writeStaticFiles(fieldDir);

    if (success) {
        qDebug() << "AgShareDownloader: Field" << fieldName << "saved to" << fieldDir;
        emit fieldImported(fieldName, fieldDir);
    } else {
        qWarning() << "AgShareDownloader: Failed to save field" << fieldName;
    }

    return success;
}

bool CAgShareDownloader::createFieldDirectory(const QString& fieldDir)
{
    QDir dir(fieldDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "AgShareDownloader: Failed to create directory:" << fieldDir;
            return false;
        }
    }
    return true;
}

bool CAgShareDownloader::writeAgShareId(const QString& fieldDir, const QString& fieldId)
{
    QFile file(fieldDir + "/agshare.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << fieldId;
        file.close();
        return true;
    }

    qWarning() << "AgShareDownloader: Failed to write agshare.txt";
    return false;
}

bool CAgShareDownloader::writeFieldTxt(const QString& fieldDir, const QJsonObject& origin, const QString& fieldName)
{
    QFile file(fieldDir + "/Field.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "AgShareDownloader: Failed to open Field.txt";
        return false;
    }

    QTextStream stream(&file);
    stream << QDateTime::currentDateTime().toString("yyyy-MMM-dd hh:mm:ss AP") << "\n";
    stream << "$FieldDir\n";
    stream << "AgShare Downloaded\n";
    stream << "$Offsets\n";
    stream << "0,0\n";
    stream << "Convergence\n";
    stream << "0\n";
    stream << "StartFix\n";

    double lat = origin["latitude"].toDouble();
    double lon = origin["longitude"].toDouble();
    stream << QString("%1,%2").arg(lat, 0, 'f', 9).arg(lon, 0, 'f', 9) << "\n";

    file.close();
    return true;
}

QList<Vec3> CAgShareDownloader::convertBoundaryFromWgs84(const QJsonArray& wgs84Boundary, CNMEA* nmea)
{
    QList<Vec3> localPoints;

    if (!nmea) {
        // Если нет CNMEA, возвращаем пустой список
        return localPoints;
    }

    for (const QJsonValue& coordVal : wgs84Boundary) {
        QJsonObject coord = coordVal.toObject();
        double lat = coord["latitude"].toDouble();
        double lon = coord["longitude"].toDouble();

        double northing, easting;
        nmea->ConvertWGS84ToLocal(lat, lon, northing, easting);

        localPoints.append(Vec3(easting, northing, 0));
    }

    return localPoints;
}

bool CAgShareDownloader::writeBoundaryTxt(const QString& fieldDir, const QJsonObject& boundary, CNMEA* nmea)
{
    QJsonArray outer = boundary["outer"].toArray();
    QJsonArray holes = boundary["holes"].toArray();

    if (outer.isEmpty()) {
        // Нет границ - это допустимо
        return true;
    }

    QFile file(fieldDir + "/Boundary.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "AgShareDownloader: Failed to open Boundary.txt";
        return false;
    }

    QTextStream stream(&file);
    stream << "$Boundary\n";

    // Обрабатываем все кольца (outer + holes)
    QList<QJsonArray> rings;
    rings.append(outer);
    for (const QJsonValue& hole : holes) {
        rings.append(hole.toArray());
    }

    for (int i = 0; i < rings.size(); i++) {
        bool isHole = (i > 0);
        QJsonArray ring = rings[i];

        if (ring.size() < 3) {
            qWarning() << "AgShareDownloader: Ring has less than 3 points, skipping";
            continue;
        }

        stream << (isHole ? "True\n" : "False\n");

        // Конвертируем координаты
        QList<Vec3> localPoints = convertBoundaryFromWgs84(ring, nmea);

        if (localPoints.isEmpty()) {
            // Если конвертация не удалась, просто используем количество точек
            stream << ring.size() << "\n";

            for (const QJsonValue& coord : ring) {
                QJsonObject point = coord.toObject();
                double easting = 0, northing = 0;

                // Попытка получить easting/northing если они есть
                if (point.contains("easting")) {
                    easting = point["easting"].toDouble();
                }
                if (point.contains("northing")) {
                    northing = point["northing"].toDouble();
                }

                // Для совместимости с AOG добавляем heading (0 для точек из AgShare)
                stream << QString("%1,%2,0.0\n")
                              .arg(easting, 0, 'f', 3)
                              .arg(northing, 0, 'f', 3);
            }
        } else {
            // Используем сконвертированные точки
            stream << localPoints.size() << "\n";

            for (const Vec3& point : localPoints) {
                stream << QString("%1,%2,0.0\n")
                .arg(point.easting, 0, 'f', 3)
                    .arg(point.northing, 0, 'f', 3);
            }
        }
    }

    file.close();
    return true;
}

QList<CTrk> CAgShareDownloader::convertAbLinesFromWgs84(const QJsonArray& abLinesArray, CNMEA* nmea)
{
    QList<CTrk> tracks;

    if (!nmea) {
        return tracks;
    }

    for (const QJsonValue& abLineVal : abLinesArray) {
        QJsonObject abLine = abLineVal.toObject();
        QString type = abLine["type"].toString();
        QJsonArray coords = abLine["coords"].toArray();

        if (coords.size() < 2) {
            continue;
        }

        CTrk track;
        track.name = abLine["name"].toString();

        if (type == "AB") {
            track.mode = TrackMode::AB;

            // Точка A
            QJsonObject ptA = coords[0].toObject();
            double latA = ptA["latitude"].toDouble();
            double lonA = ptA["longitude"].toDouble();
            nmea->ConvertWGS84ToLocal(latA, lonA, track.ptA.northing, track.ptA.easting);

            // Точка B
            QJsonObject ptB = coords[1].toObject();
            double latB = ptB["latitude"].toDouble();
            double lonB = ptB["longitude"].toDouble();
            nmea->ConvertWGS84ToLocal(latB, lonB, track.ptB.northing, track.ptB.easting);

        } else if (type == "Curve") {
            track.mode = TrackMode::Curve;

            for (const QJsonValue& coordVal : coords) {
                QJsonObject point = coordVal.toObject();
                double lat = point["latitude"].toDouble();
                double lon = point["longitude"].toDouble();

                double northing, easting;
                nmea->ConvertWGS84ToLocal(lat, lon, northing, easting);
                track.curvePts.append(Vec3(easting, northing, 0));
            }
        }

        tracks.append(track);
    }

    return tracks;
}

bool CAgShareDownloader::writeTrackLinesTxt(const QString& fieldDir, const QJsonArray& abLines, CNMEA* nmea)
{
    if (abLines.isEmpty()) {
        // Нет AB-линий - это допустимо
        return true;
    }

    QFile file(fieldDir + "/TrackLines.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "AgShareDownloader: Failed to open TrackLines.txt";
        return false;
    }

    QTextStream stream(&file);
    stream << "$TrackLines\n";

    // Конвертируем AB-линии
    QList<CTrk> tracks = convertAbLinesFromWgs84(abLines, nmea);

    for (const CTrk& track : tracks) {
        stream << track.name << "\n";

        if (track.mode == TrackMode::AB) {
            // Heading (упрощенно)
            double dx = track.ptB.easting - track.ptA.easting;
            double dy = track.ptB.northing - track.ptA.northing;
            double heading = atan2(dx, dy) * 180.0 / M_PI;
            if (heading < 0) heading += 360.0;

            stream << QString::number(heading, 'f', 3) << "\n";
            stream << QString("%1,%2\n")
                          .arg(track.ptA.easting, 0, 'f', 3)
                          .arg(track.ptA.northing, 0, 'f', 3);
            stream << QString("%1,%2\n")
                          .arg(track.ptB.easting, 0, 'f', 3)
                          .arg(track.ptB.northing, 0, 'f', 3);
            stream << "0\n"; // Nudge
            stream << "2\n"; // AB mode
            stream << "True\n";
            stream << "0\n";

        } else if (track.mode == TrackMode::Curve && !track.curvePts.isEmpty()) {
            // Для кривой используем первую и последнюю точки для heading
            Vec3 ptA = track.curvePts.first();
            Vec3 ptB = track.curvePts.last();
            double dx = ptB.easting - ptA.easting;
            double dy = ptB.northing - ptA.northing;
            double heading = atan2(dx, dy) * 180.0 / M_PI;
            if (heading < 0) heading += 360.0;

            stream << QString::number(heading, 'f', 3) << "\n";
            stream << QString("%1,%2\n")
                          .arg(ptA.easting, 0, 'f', 3)
                          .arg(ptA.northing, 0, 'f', 3);
            stream << QString("%1,%2\n")
                          .arg(ptB.easting, 0, 'f', 3)
                          .arg(ptB.northing, 0, 'f', 3);
            stream << "0\n"; // Nudge
            stream << "4\n"; // Curve mode
            stream << "True\n";
            stream << track.curvePts.size() << "\n";

            for (const Vec3& point : track.curvePts) {
                stream << QString("%1,%2,0.0\n")
                .arg(point.easting, 0, 'f', 3)
                    .arg(point.northing, 0, 'f', 3);
            }
        }
    }

    file.close();
    return true;
}

bool CAgShareDownloader::writeStaticFiles(const QString& fieldDir)
{
    // Flags.txt
    {
        QFile file(fieldDir + "/Flags.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << "$Flags\n0\n";
            file.close();
        } else {
            qWarning() << "AgShareDownloader: Failed to write Flags.txt";
            return false;
        }
    }

    // Headland.txt
    {
        QFile file(fieldDir + "/Headland.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << "$Headland\n0\n";
            file.close();
        } else {
            qWarning() << "AgShareDownloader: Failed to write Headland.txt";
            return false;
        }
    }

    // Contour.txt
    {
        QFile file(fieldDir + "/Contour.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << "$Contour\n0\n";
            file.close();
        } else {
            qWarning() << "AgShareDownloader: Failed to write Contour.txt";
            return false;
        }
    }

    // Sections.txt
    {
        QFile file(fieldDir + "/Sections.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << "Sections\n0\n";
            file.close();
        } else {
            qWarning() << "AgShareDownloader: Failed to write Sections.txt";
            return false;
        }
    }

    return true;
}

void CAgShareDownloader::onNetworkError(const QString& error)
{
    handleError(QString("Network error: %1").arg(error));
}

void CAgShareDownloader::onJsonError(const QString& error)
{
    handleError(QString("JSON error: %1").arg(error));
}

void CAgShareDownloader::handleError(const QString& error)
{
    setDownloading(false);
    setLastError(error);
    setStatusMessage("Download failed");

    qWarning() << "AgShareDownloader:" << error;
    emit downloadFinished(false, error);
}

void CAgShareDownloader::updateStatus(const QString& message)
{
    setStatusMessage(message);
    qDebug() << "AgShareDownloader:" << message;
}

void CAgShareDownloader::updateFieldList(const QJsonArray& fields)
{
    m_fieldList.clear();

    for (const QJsonValue& fieldVal : fields) {
        QJsonObject fieldObj = fieldVal.toObject();
        QVariantMap fieldMap;

        fieldMap["id"] = fieldObj["id"].toString();
        fieldMap["name"] = fieldObj["name"].toString();
        fieldMap["description"] = fieldObj.value("description").toString("");
        fieldMap["isPublic"] = fieldObj.value("isPublic").toBool(false);
        fieldMap["createdAt"] = fieldObj.value("createdAt").toString("");
        fieldMap["areaHa"] = fieldObj.value("areaHa").toDouble(0);

        // Outer boundary если есть
        if (fieldObj.contains("outerBoundary")) {
            QJsonArray boundary = fieldObj["outerBoundary"].toArray();
            QVariantList boundaryList;
            for (const QJsonValue& coordVal : boundary) {
                QJsonObject coord = coordVal.toObject();
                QVariantMap coordMap;
                coordMap["latitude"] = coord["latitude"].toDouble();
                coordMap["longitude"] = coord["longitude"].toDouble();
                boundaryList.append(coordMap);
            }
            fieldMap["outerBoundary"] = boundaryList;
        }

        m_fieldList.append(fieldMap);
    }

    emit fieldListChanged();
    qDebug() << "AgShareDownloader: Field list updated with" << m_fieldList.size() << "fields";
}

void CAgShareDownloader::setDownloading(bool downloading)
{
    if (m_isDownloading != downloading) {
        m_isDownloading = downloading;
        emit isDownloadingChanged(downloading);
    }
}

void CAgShareDownloader::setStatusMessage(const QString& message)
{
    if (m_statusMessage != message) {
        m_statusMessage = message;
        emit statusMessageChanged(message);
    }
}

void CAgShareDownloader::setLastError(const QString& error)
{
    if (m_lastError != error) {
        m_lastError = error;
        emit lastErrorChanged(error);
    }
}
