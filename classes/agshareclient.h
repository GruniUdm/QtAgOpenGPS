// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
#ifndef AGSHARECLIENT_H
#define AGSHARECLIENT_H

#include <QObject>
#include <QString>
#include <QList>
#include <QUuid>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QTimer>
#include <QMutex>

class AgShareClient : public QObject
{
    Q_OBJECT
private slots:  // Должен быть private slots
    void onNetworkReply(QNetworkReply* reply);
    void onUploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

public:
    explicit AgShareClient(QObject *parent = nullptr);
    explicit AgShareClient(const QString& serverUrl, const QString& apiKey, QObject *parent = nullptr);
    ~AgShareClient();

    // Основные методы API
    void uploadField(const QUuid& fieldId, const QJsonObject& fieldData);
    void downloadField(const QUuid& fieldId);
    void getOwnFields();
    void getPublicFields(double latitude, double longitude, double radius = 50.0);
    void checkApi();

    // Настройки
    void setServerUrl(const QString& url);
    QString serverUrl() const { return m_serverUrl; }

    void setApiKey(const QString& key);
    QString apiKey() const { return m_apiKey; }

    void setTimeout(int milliseconds);
    int timeout() const { return m_timeout; }

    // Проверка состояния
    bool isBusy() const { return m_isBusy; }

signals:
    // Сигналы результатов операций
    void uploadFinished(bool success, const QString& message, const QJsonObject& data = QJsonObject());
    void downloadFinished(bool success, const QString& message, const QJsonObject& fieldData = QJsonObject());
    void ownFieldsReceived(bool success, const QString& message, const QJsonArray& fields = QJsonArray());
    void publicFieldsReceived(bool success, const QString& message, const QJsonArray& fields = QJsonArray());
    void apiChecked(bool valid, const QString& message);

    // Сигналы прогресса
    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    // Сигналы ошибок
    void networkError(const QString& error);
    void jsonError(const QString& error);
    void operationStarted();
    void operationFinished();

private:
    // Для отслеживания типа запроса - объявляем ДО использования в методах
    enum RequestType {
        None,
        ApiCheck,
        Upload,
        Download,
        OwnFields,
        PublicFields
    };

    // Вспомогательные методы
    void buildClient();
    QNetworkRequest createRequest(const QString& endpoint) const;
    QByteArray prepareJsonPayload(const QJsonObject& data) const;

    // Обработчики ответов
    void handleApiCheckReply(QNetworkReply* reply, const QByteArray& response);
    void handleUploadReply(QNetworkReply* reply, const QByteArray& response);
    void handleDownloadReply(QNetworkReply* reply, const QByteArray& response);
    void handleOwnFieldsReply(QNetworkReply* reply, const QByteArray& response);
    void handlePublicFieldsReply(QNetworkReply* reply, const QByteArray& response);

    // Обработка ошибок
    void handleNetworkError(QNetworkReply* reply, const QString& context);
    void handleJsonError(const QJsonParseError& error, const QString& context);

    // Вспомогательные методы
    QString getRequestContext(RequestType type) const;
    QString getErrorMessage(QNetworkReply* reply) const;

    // Валидация
    bool validateResponse(QNetworkReply* reply, QByteArray& response);
    bool validateJson(const QByteArray& response, QJsonDocument& doc, QJsonParseError& error);

    // Члены класса
    QNetworkAccessManager* m_networkManager;
    QString m_serverUrl;
    QString m_apiKey;
    int m_timeout;
    bool m_isBusy;
    mutable QMutex m_mutex;

    QMap<QNetworkReply*, RequestType> m_requestTypes;
    QMap<QNetworkReply*, QTimer*> m_timeoutTimers;

    // Утилиты
    void startTimeoutTimer(QNetworkReply* reply);
    void clearTimeoutTimer(QNetworkReply* reply);
};

#endif // AGSHARECLIENT_H
