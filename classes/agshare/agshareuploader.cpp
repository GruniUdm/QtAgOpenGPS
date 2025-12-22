// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
#include "agshareuploader.h"
#include "formgps.h"
#include "qmlutil.h"
#include "newsettings.h"
#include "cnmea.h"

#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTimer>
#include <QTextStream>
#include <QDebug>

AgShareUploader::AgShareUploader(QObject *parent)
    : QObject(parent)
    , m_client(new AgShareClient(this))
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
    , m_isUploading(false)
    , m_uploadProgress(0)
{
    // Connect AgShareClient signals
    connect(m_client, &AgShareClient::uploadFinished,
            this, &AgShareUploader::onUploadFinished);
    connect(m_client, &AgShareClient::uploadProgress,
            this, &AgShareUploader::onUploadProgress);
    connect(m_client, &AgShareClient::networkError,
            this, &AgShareUploader::onNetworkError);
    connect(m_client, &AgShareClient::jsonError,
            this, &AgShareUploader::onJsonError);
}

AgShareUploader::~AgShareUploader()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
}

AgShareDtos::FieldSnapshot AgShareUploader::createSnapshot(FormGPS* gps)
{
    AgShareDtos::FieldSnapshot snapshot;

    if (!gps) {
        qWarning() << "AgShareUploader: GPS object is null";
        return snapshot;
    }

    // Получаем директорию поля
#ifdef __ANDROID__
    QString baseDir = gps->androidDirectory + QCoreApplication::applicationName() + "/Fields/";
#else
    QString baseDir = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                      + "/" + QCoreApplication::applicationName() + "/Fields/";
#endif

    QString dir = baseDir + gps->currentFieldDirectory;

    // Проверяем существование директории
    if (!QDir(dir).exists()) {
        qWarning() << "AgShareUploader: Field directory does not exist:" << dir;
        return snapshot;
    }

    // Пытаемся загрузить существующий ID поля
    snapshot.fieldId = loadFieldId(dir);
    if (snapshot.fieldId.isNull()) {
        snapshot.fieldId = QUuid::createUuid();
        qDebug() << "AgShareUploader: Created new field ID:" << snapshot.fieldId;
    } else {
        qDebug() << "AgShareUploader: Loaded existing field ID:" << snapshot.fieldId;
    }

    // Собираем границы
    snapshot.boundaries.clear();
    for (const auto& boundary : gps->bnd.bndList) {
        if (!boundary.fenceLine.isEmpty()) {
            QList<Vec3> fencePoints;
            for (const auto& point : boundary.fenceLine) {
                fencePoints.append(point);
            }
            snapshot.boundaries.append(fencePoints);
        }
    }

    // Собираем треки
    snapshot.tracks = gps->trk.gArr;

    // Информация о поле
    snapshot.fieldName = gps->displayFieldName;
    snapshot.fieldDirectory = dir;
    snapshot.originLat = gps->pn.latStart;
    snapshot.originLon = gps->pn.lonStart;
    snapshot.convergence = 0; // TODO: Рассчитать convergence если нужно

    // Устанавливаем конвертер
    snapshot.converter = &gps->pn;

    qDebug() << "AgShareUploader: Created snapshot for field" << snapshot.fieldName
             << "with" << snapshot.boundaries.size() << "boundaries and"
             << snapshot.tracks.size() << "tracks";

    return snapshot;
}

void AgShareUploader::uploadCurrentField(FormGPS* gps)
{
    if (!gps) {
        setLastError("GPS object is null");
        emit uploadFinished(false, "GPS object is null");
        return;
    }

    if (m_isUploading) {
        setLastError("Upload already in progress");
        emit uploadFinished(false, "Upload already in progress");
        return;
    }

    // Валидация поля
    if (!validateFieldForUpload(gps)) {
        setLastError("Field is not valid for upload");
        emit uploadFinished(false, "Field is not valid for upload");
        return;
    }

    // Создаем снапшот
    AgShareDtos::FieldSnapshot snapshot = createSnapshot(gps);

    // Валидация снапшота
    if (!validateFieldForUpload(snapshot)) {
        setLastError("Field snapshot is not valid");
        emit uploadFinished(false, "Field snapshot is not valid");
        return;
    }

    uploadFieldFromSnapshot(snapshot);
}

void AgShareUploader::uploadFieldFromSnapshot(const AgShareDtos::FieldSnapshot& snapshot)
{
    if (m_isUploading) {
        setLastError("Upload already in progress");
        emit uploadFinished(false, "Upload already in progress");
        return;
    }

    if (!snapshot.isValid()) {
        setLastError("Invalid field snapshot");
        emit uploadFinished(false, "Invalid field snapshot");
        return;
    }

    setUploading(true);
    setUploadProgress(0);
    setLastError("");
    setStatusMessage("Preparing field data...");

    emit uploadStarted();
    m_currentFieldId = snapshot.fieldId;
    m_currentFieldDir = snapshot.fieldDirectory;

    try {
        // Подготавливаем данные для отправки
        setStatusMessage("Converting coordinates...");
        AgShareDtos::AgShareFieldDto dto = prepareUploadData(snapshot);

        if (!dto.isValid()) {
            handleError("Failed to prepare upload data");
            return;
        }

        // Создаем payload
        setStatusMessage("Creating upload package...");
        QJsonObject payload = AgShareDtos::DtoFactory::createUploadPayload(dto);

        if (payload.isEmpty()) {
            handleError("Failed to create upload payload");
            return;
        }

        // Отправляем через AgShareClient
        setStatusMessage("Uploading to AgShare...");
        m_client->uploadField(dto.id, payload);

        qDebug() << "AgShareUploader: Started upload of field" << dto.id << dto.name;

    } catch (const std::exception& e) {
        handleError(QString("Exception: %1").arg(e.what()));
    } catch (...) {
        handleError("Unknown exception during upload preparation");
    }
}

void AgShareUploader::cancelUpload()
{
    if (m_isUploading) {
        setStatusMessage("Upload cancelled");
        setUploading(false);
        emit uploadFinished(false, "Upload cancelled by user", m_currentFieldId);
    }
}

QList<AgShareDtos::CoordinateDto> AgShareUploader::convertBoundaryToWgs84(const QList<Vec3>& localPoints, CNMEA* nmea)
{
    QList<AgShareDtos::CoordinateDto> coords;

    if (!nmea || localPoints.isEmpty()) {
        return coords;
    }

    for (const Vec3& point : localPoints) {
        double lat, lon;
        nmea->ConvertLocalToWGS84(point.northing, point.easting, lat, lon);
        coords.append(AgShareDtos::CoordinateDto(lat, lon));
    }

    // Замыкаем полигон если нужно
    if (coords.size() > 1) {
        const AgShareDtos::CoordinateDto& first = coords.first();
        const AgShareDtos::CoordinateDto& last = coords.last();

        if (first.latitude != last.latitude || first.longitude != last.longitude) {
            coords.append(first);
        }
    }

    return coords;
}

QList<AgShareDtos::AbLineUploadDto> AgShareUploader::convertTracksToWgs84(const QVector<CTrk>& tracks, CNMEA* nmea)
{
    QList<AgShareDtos::AbLineUploadDto> result;

    if (!nmea) {
        return result;
    }

    for (const CTrk& track : tracks) {
        if (track.mode == TrackMode::AB) {
            // AB line
            AgShareDtos::AbLineUploadDto abLine;
            abLine.name = track.name;
            abLine.type = "AB";

            // Convert point A
            double latA, lonA;
            nmea->ConvertLocalToWGS84(track.ptA.northing, track.ptA.easting, latA, lonA);
            abLine.coords.append(AgShareDtos::CoordinateDto(latA, lonA));

            // Convert point B
            double latB, lonB;
            nmea->ConvertLocalToWGS84(track.ptB.northing, track.ptB.easting, latB, lonB);
            abLine.coords.append(AgShareDtos::CoordinateDto(latB, lonB));

            if (abLine.isValid()) {
                result.append(abLine);
            }
        }
        else if (track.mode == TrackMode::Curve && track.curvePts.count() >= 2) {
            // Curve line
            AgShareDtos::AbLineUploadDto curveLine;
            curveLine.name = track.name;
            curveLine.type = "Curve";

            for (const Vec3& point : track.curvePts) {
                double lat, lon;
                nmea->ConvertLocalToWGS84(point.northing, point.easting, lat, lon);
                curveLine.coords.append(AgShareDtos::CoordinateDto(lat, lon));
            }

            if (curveLine.isValid()) {
                result.append(curveLine);
            }
        }
    }

    return result;
}

AgShareDtos::AgShareFieldDto AgShareUploader::prepareUploadData(const AgShareDtos::FieldSnapshot& snapshot)
{
    AgShareDtos::AgShareFieldDto dto;

    if (!snapshot.isValid()) {
        qWarning() << "AgShareUploader: Cannot prepare data from invalid snapshot";
        return dto;
    }

    // Основная информация
    dto.id = snapshot.fieldId;
    dto.name = snapshot.fieldName;
    dto.isPublic = false; // По умолчанию приватное
    dto.createdAt = QDateTime::currentDateTime();
    dto.latitude = snapshot.originLat;
    dto.longitude = snapshot.originLon;
    dto.convergence = snapshot.convergence;

    // Границы
    if (!snapshot.boundaries.isEmpty()) {
        // Outer boundary (первая граница)
        QList<AgShareDtos::CoordinateDto> outerCoords = convertBoundaryToWgs84(snapshot.boundaries.first(), snapshot.converter);
        if (outerCoords.size() >= 3) {
            dto.boundaries.append(outerCoords);
        }

        // Holes (остальные границы)
        for (int i = 1; i < snapshot.boundaries.size(); ++i) {
            QList<AgShareDtos::CoordinateDto> holeCoords = convertBoundaryToWgs84(snapshot.boundaries[i], snapshot.converter);
            if (holeCoords.size() >= 3) {
                dto.boundaries.append(holeCoords);
            }
        }
    }

    // AB Lines
    dto.abLines = convertTracksToWgs84(snapshot.tracks, snapshot.converter);

    qDebug() << "AgShareUploader: Prepared upload data with"
             << dto.boundaries.size() << "boundaries and"
             << dto.abLines.size() << "AB lines";

    return dto;
}

QJsonObject AgShareUploader::prepareUploadPayload(const AgShareDtos::AgShareFieldDto& dto)
{
    return AgShareDtos::DtoFactory::createUploadPayload(dto);
}

void AgShareUploader::saveFieldId(const QString& fieldDir, const QUuid& fieldId)
{
    QString idPath = fieldDir + "/agshare.txt";
    QFile file(idPath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        stream << fieldId.toString();
        file.close();
        qDebug() << "AgShareUploader: Saved field ID to" << idPath;
    } else {
        qWarning() << "AgShareUploader: Failed to save field ID to" << idPath;
    }
}

QUuid AgShareUploader::loadFieldId(const QString& fieldDir)
{
    QString idPath = fieldDir + "/agshare.txt";
    QFile file(idPath);

    if (file.exists() && file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        QString raw = stream.readAll().trimmed();
        file.close();

        QUuid id = QUuid(raw);
        if (!id.isNull()) {
            return id;
        }
    }

    return QUuid();
}

bool AgShareUploader::hasFieldId(const QString& fieldDir)
{
    return !loadFieldId(fieldDir).isNull();
}

bool AgShareUploader::saveFieldIdToDisk(const QString& fieldDir, const QUuid& fieldId)
{
    if (fieldId.isNull()) {
        qWarning() << "AgShareUploader: Cannot save null field ID";
        return false;
    }

    if (fieldDir.isEmpty()) {
        qWarning() << "AgShareUploader: Field directory is empty";
        return false;
    }

    // Создаем директорию если не существует
    QDir dir(fieldDir);
    if (!dir.exists()) {
        if (!dir.mkpath(".")) {
            qWarning() << "AgShareUploader: Failed to create directory:" << fieldDir;
            return false;
        }
    }

    saveFieldId(fieldDir, fieldId);
    return true;
}

bool AgShareUploader::validateFieldForUpload(const AgShareDtos::FieldSnapshot& snapshot)
{
    if (!snapshot.isValid()) {
        qWarning() << "AgShareUploader: Invalid snapshot";
        return false;
    }

    if (snapshot.fieldName.isEmpty()) {
        qWarning() << "AgShareUploader: Field name is empty";
        return false;
    }

    if (snapshot.boundaries.isEmpty()) {
        qWarning() << "AgShareUploader: No boundaries in field";
        return false;
    }

    // Проверяем что внешняя граница имеет хотя бы 3 точки
    if (snapshot.boundaries.first().size() < 3) {
        qWarning() << "AgShareUploader: Outer boundary has less than 3 points";
        return false;
    }

    return true;
}

bool AgShareUploader::validateFieldForUpload(FormGPS* gps)
{
    if (!gps) {
        qWarning() << "AgShareUploader: GPS is null";
        return false;
    }

    if (gps->currentFieldDirectory.isEmpty()) {
        qWarning() << "AgShareUploader: No field is open";
        return false;
    }

    if (gps->displayFieldName.isEmpty()) {
        qWarning() << "AgShareUploader: Field name is empty";
        return false;
    }

    if (gps->bnd.bndList.isEmpty()) {
        qWarning() << "AgShareUploader: No boundaries in field";
        return false;
    }

    return true;
}

void AgShareUploader::onUploadFinished(bool success, const QString& message, const QJsonObject& data)
{
    setUploading(false);
    setUploadProgress(100);

    if (success) {
        setStatusMessage("Upload completed successfully");

        // Сохраняем ID поля на диск
        if (!m_currentFieldId.isNull() && !m_currentFieldDir.isEmpty()) {
            if (saveFieldIdToDisk(m_currentFieldDir, m_currentFieldId)) {
                qDebug() << "AgShareUploader: Saved field ID" << m_currentFieldId << "to disk";
            } else {
                qWarning() << "AgShareUploader: Failed to save field ID to disk";
            }
        }

        qDebug() << "AgShareUploader: Upload successful:" << message;
    } else {
        setStatusMessage("Upload failed");
        setLastError(message);
        qWarning() << "AgShareUploader: Upload failed:" << message;
    }

    emit uploadFinished(success, message, m_currentFieldId);
}

void AgShareUploader::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        int progress = (bytesSent * 100) / bytesTotal;
        setUploadProgress(progress);
        emit uploadProgressDetailed(bytesSent, bytesTotal);
    }
}

void AgShareUploader::onNetworkError(const QString& error)
{
    handleError(QString("Network error: %1").arg(error));
}

void AgShareUploader::onJsonError(const QString& error)
{
    handleError(QString("JSON error: %1").arg(error));
}

void AgShareUploader::handleError(const QString& error)
{
    setUploading(false);
    setLastError(error);
    setStatusMessage("Upload failed");

    qWarning() << "AgShareUploader:" << error;
    emit uploadFinished(false, error, m_currentFieldId);
}

void AgShareUploader::updateStatus(const QString& message)
{
    setStatusMessage(message);
    qDebug() << "AgShareUploader:" << message;
}

void AgShareUploader::setUploading(bool uploading)
{
    if (m_isUploading != uploading) {
        m_isUploading = uploading;
        emit isUploadingChanged(uploading);
    }
}

void AgShareUploader::setUploadProgress(int progress)
{
    if (m_uploadProgress != progress) {
        m_uploadProgress = progress;
        emit uploadProgressChanged(progress);
    }
}

void AgShareUploader::setStatusMessage(const QString& message)
{
    if (m_statusMessage != message) {
        m_statusMessage = message;
        emit statusMessageChanged(message);
    }
}

void AgShareUploader::setLastError(const QString& error)
{
    if (m_lastError != error) {
        m_lastError = error;
        emit lastErrorChanged(error);
    }
}
