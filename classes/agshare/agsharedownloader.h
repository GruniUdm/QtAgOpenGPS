// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
#ifndef AGSHAREDOWNLOADER_H
#define AGSHAREDOWNLOADER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QUuid>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVariantList>
#include <QVariantMap>
#include "agshareclient.h"
#include "agsharedtos.h"
#include "ctrack.h"

class FormGPS;
class CNMEA;

class CAgShareDownloader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool isDownloading READ isDownloading NOTIFY isDownloadingChanged)
    Q_PROPERTY(QString statusMessage READ statusMessage NOTIFY statusMessageChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)
    Q_PROPERTY(QVariantList fieldList READ fieldList NOTIFY fieldListChanged)

public:
    explicit CAgShareDownloader(QObject *parent = nullptr);
    ~CAgShareDownloader();

    // Основные методы
    Q_INVOKABLE void downloadField(const QString& fieldId);
    Q_INVOKABLE void downloadAllFields(bool forceOverwrite = false);
    Q_INVOKABLE void refreshFieldList();
    Q_INVOKABLE void cancelDownload();

    Q_INVOKABLE bool importFieldToGPS(const QJsonObject& fieldData, FormGPS* gps);

    // Геттеры свойств
    bool isDownloading() const { return m_isDownloading; }
    QString statusMessage() const { return m_statusMessage; }
    QString lastError() const { return m_lastError; }
    QVariantList fieldList() const { return m_fieldList; }

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
    static QString getFieldsDirectory();
    static QString createSafeFileName(const QString& name);
    static bool fieldExistsLocally(const QString& fieldName, const QString& fieldId = QString());

    // Обработка скачанных данных
    bool processDownloadedField(const QJsonObject& json, FormGPS* gps = nullptr);
    bool saveFieldToDisk(const QJsonObject& fieldData, FormGPS* gps = nullptr);

signals:
    void downloadStarted();
    void downloadProgress(const QString& fieldName, int current, int total);
    void downloadProgressDetailed(qint64 bytesReceived, qint64 bytesTotal);
    void downloadFinished(bool success, const QString& message, const QString& fieldId = QString());
    void fieldListUpdated(bool success, const QString& message);
    void fieldImported(const QString& fieldName, const QString& fieldPath);
    void isDownloadingChanged(bool downloading);
    void statusMessageChanged(const QString& message);
    void lastErrorChanged(const QString& error);
    void fieldListChanged();

private slots:
    void onDownloadFinished(bool success, const QString& message, const QJsonObject& fieldData);
    void onOwnFieldsReceived(bool success, const QString& message, const QJsonArray& fields);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onNetworkError(const QString& error);
    void onJsonError(const QString& error);

private:
    // Вспомогательные методы
    void downloadNextField();
    bool createFieldDirectory(const QString& fieldDir);

    // Запись файлов
    bool writeAgShareId(const QString& fieldDir, const QString& fieldId);
    bool writeFieldTxt(const QString& fieldDir, const QJsonObject& origin, const QString& fieldName);
    bool writeBoundaryTxt(const QString& fieldDir, const QJsonObject& boundary, CNMEA* nmea = nullptr);
    bool writeTrackLinesTxt(const QString& fieldDir, const QJsonArray& abLines, CNMEA* nmea = nullptr);
    bool writeStaticFiles(const QString& fieldDir);

    // Обработка ошибок
    void handleError(const QString& error);
    void updateStatus(const QString& message);

    // Конвертация данных
    QList<Vec3> convertBoundaryFromWgs84(const QJsonArray& wgs84Boundary, CNMEA* nmea);
    QList<CTrk> convertAbLinesFromWgs84(const QJsonArray& abLinesArray, CNMEA* nmea);

    // Члены класса
    AgShareClient* m_client;
    QNetworkAccessManager* m_networkManager;

    QString m_statusMessage;
    QString m_lastError;

    bool m_isDownloading;
    QVariantList m_fieldList;
    QList<QJsonObject> m_fieldsToDownload;
    int m_currentDownloadIndex;
    bool m_forceOverwrite;

    void setDownloading(bool downloading);
    void setStatusMessage(const QString& message);
    void setLastError(const QString& error);
    void updateFieldList(const QJsonArray& fields);
};

#endif // AGSHAREDOWNLOADER_H
