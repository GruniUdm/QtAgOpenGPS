// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
#ifndef AGSHAREUPLOADER_H
#define AGSHAREUPLOADER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QVector>
#include <QUuid>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "agshareclient.h"
#include "agsharedtos.h"
#include "cboundarylist.h"
#include "ctrack.h"

// Forward declarations
class FormGPS;
class CNMEA;

class AgShareUploader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isUploading READ isUploading NOTIFY isUploadingChanged)
    Q_PROPERTY(int uploadProgress READ uploadProgress NOTIFY uploadProgressChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
    explicit AgShareUploader(QObject *parent = nullptr);
    ~AgShareUploader();

    // Создание снапшота поля
    static AgShareDtos::FieldSnapshot createSnapshot(FormGPS* gps);

    // Основные методы загрузки
    Q_INVOKABLE void uploadCurrentField(FormGPS* gps);
    Q_INVOKABLE void uploadFieldFromSnapshot(const AgShareDtos::FieldSnapshot& snapshot);
    Q_INVOKABLE void cancelUpload();

    // Геттеры свойств
    bool isUploading() const { return m_isUploading; }
    int uploadProgress() const { return m_uploadProgress; }
    QString statusMessage() const { return m_statusMessage; }
    QString lastError() const { return m_lastError; }

    // Настройки
    void setApiUrl(const QString& url) {
        if (m_client) m_client->setServerUrl(url);
    }
    QString apiUrl() const {
        return m_client ? m_client->serverUrl() : QString();
    }

    void setAuthToken(const QString& token) {
        if (m_client) m_client->setApiKey(token);
    }
    QString authToken() const {
        return m_client ? m_client->apiKey() : QString();
    }

    // Утилиты
    static void saveFieldId(const QString& fieldDir, const QUuid& fieldId);
    static QUuid loadFieldId(const QString& fieldDir);
    static bool hasFieldId(const QString& fieldDir);

    // Валидация
    static bool validateFieldForUpload(const AgShareDtos::FieldSnapshot& snapshot);
    static bool validateFieldForUpload(FormGPS* gps);

signals:
    void uploadStarted();
    void uploadFinished(bool success, const QString& message, const QUuid& fieldId = QUuid());
    void uploadProgressChanged(int percent);
    void uploadProgressDetailed(qint64 bytesSent, qint64 bytesTotal);
    void isUploadingChanged(bool uploading);
    void statusMessageChanged(const QString& message);
    void lastErrorChanged(const QString& error);

private slots:
    void onUploadFinished(bool success, const QString& message, const QJsonObject& data);
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onNetworkError(const QString& error);
    void onJsonError(const QString& error);

private:
    // Вспомогательные методы
    QList<AgShareDtos::CoordinateDto> convertBoundaryToWgs84(const QList<Vec3>& localPoints, CNMEA* nmea);
    QList<AgShareDtos::AbLineUploadDto> convertTracksToWgs84(const QVector<CTrk>& tracks, CNMEA* nmea);

    // Подготовка данных
    AgShareDtos::AgShareFieldDto prepareUploadData(const AgShareDtos::FieldSnapshot& snapshot);
    QJsonObject prepareUploadPayload(const AgShareDtos::AgShareFieldDto& dto);

    // Сохранение ID поля
    bool saveFieldIdToDisk(const QString& fieldDir, const QUuid& fieldId);

    // Обработка ошибок
    void handleError(const QString& error);
    void updateStatus(const QString& message);

    // Члены класса
    AgShareClient* m_client;
    QNetworkAccessManager* m_networkManager;
    QNetworkReply* m_currentReply;

    QString m_statusMessage;
    QString m_lastError;

    bool m_isUploading;
    int m_uploadProgress;
    QUuid m_currentFieldId;
    QString m_currentFieldDir;

    void setUploading(bool uploading);
    void setUploadProgress(int progress);
    void setStatusMessage(const QString& message);
    void setLastError(const QString& error);
};

#endif // AGSHAREUPLOADER_H
