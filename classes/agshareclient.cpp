// Copyright (C) 2024 Michael Torrie and the QtAgOpenGPS Dev Team
// SPDX-License-Identifier: GNU General Public License v3.0 or later
//
#include "agshareclient.h"
#include <QNetworkRequest>
#include <QJsonParseError>
#include <QTimer>
#include <QDebug>
#include <QCoreApplication>

AgShareClient::AgShareClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_serverUrl("https://api.agshare.com")
    , m_timeout(30000) // 30 секунд по умолчанию
    , m_isBusy(false)
{
    buildClient();
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &AgShareClient::onNetworkReply);
}

AgShareClient::AgShareClient(const QString& serverUrl, const QString& apiKey, QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_serverUrl(serverUrl.trimmed())
    , m_apiKey(apiKey)
    , m_timeout(30000)
    , m_isBusy(false)
{
    if (m_serverUrl.endsWith('/')) {
        m_serverUrl.chop(1);
    }

    buildClient();
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &AgShareClient::onNetworkReply);
}

AgShareClient::~AgShareClient()
{
    QMutexLocker locker(&m_mutex);

    // Отменяем все активные запросы
    for (auto reply : m_requestTypes.keys()) {
        clearTimeoutTimer(reply);
        if (reply->isRunning()) {
            reply->abort();
        }
        reply->deleteLater();
    }
    m_requestTypes.clear();
}

void AgShareClient::buildClient()
{
    QMutexLocker locker(&m_mutex);

    // Сбрасываем менеджер сети
    if (m_networkManager) {
        m_networkManager->deleteLater();
    }

    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &AgShareClient::onNetworkReply);
}

void AgShareClient::setServerUrl(const QString& url)
{
    //QMutexLocker locker(&m_mutex);

    QString cleanedUrl = url.trimmed();
    if (cleanedUrl.endsWith('/')) {
        cleanedUrl.chop(1);
    }

    if (m_serverUrl != cleanedUrl) {
        m_serverUrl = cleanedUrl;
        buildClient();
        qDebug() << "AgShareClient: Server URL updated to" << m_serverUrl;
    }
}

void AgShareClient::setApiKey(const QString& key)
{
   // QMutexLocker locker(&m_mutex);

    if (m_apiKey != key) {
        m_apiKey = key;
        buildClient();
        qDebug() << "AgShareClient: API key updated";
    }
}

void AgShareClient::setTimeout(int milliseconds)
{
    QMutexLocker locker(&m_mutex);

    if (milliseconds > 0 && m_timeout != milliseconds) {
        m_timeout = milliseconds;
        qDebug() << "AgShareClient: Timeout set to" << milliseconds << "ms";
    }
}

QNetworkRequest AgShareClient::createRequest(const QString& endpoint) const
{
    QString url = m_serverUrl + endpoint;

    // Правильный синтаксис создания QNetworkRequest
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    // Устанавливаем заголовки
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("User-Agent", QString("%1/%2").arg(QCoreApplication::applicationName(),
                                                            QCoreApplication::applicationVersion()).toUtf8());

    // Добавляем авторизацию
    if (!m_apiKey.isEmpty()) {
        request.setRawHeader("Authorization", "ApiKey " + m_apiKey.toUtf8());
    }

    return request;
}

QByteArray AgShareClient::prepareJsonPayload(const QJsonObject& data) const
{
    QJsonDocument doc(data);
    return doc.toJson(QJsonDocument::Indented);
}

void AgShareClient::startTimeoutTimer(QNetworkReply* reply)
{
    clearTimeoutTimer(reply);

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);

    connect(timer, &QTimer::timeout, [this, reply]() {
        if (reply && reply->isRunning()) {
            qDebug() << "AgShareClient: Request timeout for" << m_requestTypes.value(reply);
            reply->abort();
        }
    });

    timer->start(m_timeout);
    m_timeoutTimers[reply] = timer;
}

void AgShareClient::clearTimeoutTimer(QNetworkReply* reply)
{
    if (m_timeoutTimers.contains(reply)) {
        QTimer* timer = m_timeoutTimers.take(reply);
        timer->stop();
        timer->deleteLater();
    }
}

QString AgShareClient::getErrorMessage(QNetworkReply* reply) const
{
    if (!reply) return "No reply object";

    QString errorString = reply->errorString();
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (statusCode > 0) {
        return QString("HTTP %1: %2").arg(statusCode).arg(errorString);
    }

    return errorString;
}

void AgShareClient::uploadField(const QUuid& fieldId, const QJsonObject& fieldData)
{
    QMutexLocker locker(&m_mutex);

    if (m_isBusy) {
        emit uploadFinished(false, "Another operation is in progress");
        return;
    }

    if (fieldId.isNull()) {
        emit uploadFinished(false, "Invalid field ID");
        return;
    }

    m_isBusy = true;
    emit operationStarted();

    QNetworkRequest request = createRequest("/api/fields/" + fieldId.toString());
    QByteArray jsonData = prepareJsonPayload(fieldData);

    QNetworkReply* reply = m_networkManager->put(request, jsonData);
    m_requestTypes[reply] = Upload;

    connect(reply, &QNetworkReply::uploadProgress,
            this, &AgShareClient::onUploadProgress);

    startTimeoutTimer(reply);

    qDebug() << "AgShareClient: Uploading field" << fieldId << "(" << jsonData.size() << "bytes)";
}

void AgShareClient::downloadField(const QUuid& fieldId)
{
    QMutexLocker locker(&m_mutex);

    if (m_isBusy) {
        emit downloadFinished(false, "Another operation is in progress");
        return;
    }

    if (fieldId.isNull()) {
        emit downloadFinished(false, "Invalid field ID");
        return;
    }

    m_isBusy = true;
    emit operationStarted();

    QNetworkRequest request = createRequest("/api/fields/" + fieldId.toString());

    QNetworkReply* reply = m_networkManager->get(request);
    m_requestTypes[reply] = Download;

    connect(reply, &QNetworkReply::downloadProgress,
            this, &AgShareClient::onDownloadProgress);

    startTimeoutTimer(reply);

    qDebug() << "AgShareClient: Downloading field" << fieldId;
}

void AgShareClient::getOwnFields()
{
    QMutexLocker locker(&m_mutex);

    if (m_isBusy) {
        emit ownFieldsReceived(false, "Another operation is in progress");
        return;
    }

    m_isBusy = true;
    emit operationStarted();

    QNetworkRequest request = createRequest("/api/fields/");

    QNetworkReply* reply = m_networkManager->get(request);
    m_requestTypes[reply] = OwnFields;

    startTimeoutTimer(reply);

    qDebug() << "AgShareClient: Fetching own fields";
}

void AgShareClient::getPublicFields(double latitude, double longitude, double radius)
{
    QMutexLocker locker(&m_mutex);

    if (m_isBusy) {
        emit publicFieldsReceived(false, "Another operation is in progress");
        return;
    }

    m_isBusy = true;
    emit operationStarted();

    QString endpoint = QString("/api/fields/public?lat=%1&lon=%2&radius=%3")
                           .arg(latitude, 0, 'f', 9)
                           .arg(longitude, 0, 'f', 9)
                           .arg(radius, 0, 'f', 1);

    QNetworkRequest request = createRequest(endpoint);

    QNetworkReply* reply = m_networkManager->get(request);
    m_requestTypes[reply] = PublicFields;

    startTimeoutTimer(reply);

    qDebug() << "AgShareClient: Fetching public fields near" << latitude << longitude;
}

void AgShareClient::checkApi()
{
    QMutexLocker locker(&m_mutex);

    if (m_isBusy) {
        emit apiChecked(false, "Another operation is in progress");
        return;
    }

    m_isBusy = true;
    emit operationStarted();

    QNetworkRequest request = createRequest("/api/fields");

    QNetworkReply* reply = m_networkManager->get(request);
    m_requestTypes[reply] = ApiCheck;

    startTimeoutTimer(reply);

    qDebug() << "AgShareClient: Checking API connection";
}

void AgShareClient::onUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    emit uploadProgress(bytesSent, bytesTotal);
}

void AgShareClient::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit downloadProgress(bytesReceived, bytesTotal);
}

bool AgShareClient::validateJson(const QByteArray& response, QJsonDocument& doc, QJsonParseError& error)
{
    doc = QJsonDocument::fromJson(response, &error);

    if (error.error != QJsonParseError::NoError) {
        handleJsonError(error, "JSON parse error");
        return false;
    }

    if (doc.isNull()) {
        emit jsonError("Invalid JSON document");
        return false;
    }

    return true;
}

void AgShareClient::handleJsonError(const QJsonParseError& error, const QString& context)
{
    QString message = QString("%1: %2").arg(context).arg(error.errorString());
    emit jsonError(message);

    qWarning() << "AgShareClient:" << message;
}

void AgShareClient::onNetworkReply(QNetworkReply* reply)
{
    if (!m_requestTypes.contains(reply)) {
        reply->deleteLater();
        return;
    }

    RequestType type = m_requestTypes.take(reply);
    clearTimeoutTimer(reply);

    QByteArray response;
    if (!validateResponse(reply, response)) {
        m_isBusy = false;
        emit operationFinished();
        reply->deleteLater();
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc;
    if (!validateJson(response, doc, parseError)) {
        m_isBusy = false;
        emit operationFinished();
        reply->deleteLater();
        return;
    }

    // Обрабатываем ответ в зависимости от типа запроса
    switch (type) {
    case ApiCheck:
        handleApiCheckReply(reply, response);
        break;
    case Upload:
        handleUploadReply(reply, response);
        break;
    case Download:
        handleDownloadReply(reply, response);
        break;
    case OwnFields:
        handleOwnFieldsReply(reply, response);
        break;
    case PublicFields:
        handlePublicFieldsReply(reply, response);
        break;
    default:
        break;
    }

    m_isBusy = false;
    emit operationFinished();
    reply->deleteLater();
}

void AgShareClient::handleApiCheckReply(QNetworkReply* reply, const QByteArray& response)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    bool success = (statusCode == 200);
    QString message;

    if (success) {
        message = "API connection successful";
        qDebug() << "AgShareClient:" << message;
    } else if (statusCode == 401) {
        message = "Invalid API key";
        qWarning() << "AgShareClient:" << message;
    } else {
        message = QString("API check failed with status %1").arg(statusCode);
        qWarning() << "AgShareClient:" << message;
    }

    emit apiChecked(success, message);
}

void AgShareClient::handleUploadReply(QNetworkReply* reply, const QByteArray& response)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    bool success = (statusCode >= 200 && statusCode < 300);

    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject data = doc.object();

    QString message;
    if (success) {
        message = data.value("message").toString("Upload successful");
        qDebug() << "AgShareClient: Upload successful:" << message;
    } else {
        message = data.value("message").toString(QString("Upload failed with status %1").arg(statusCode));
        qWarning() << "AgShareClient: Upload failed:" << message;
    }

    emit uploadFinished(success, message, data);
}

void AgShareClient::handleDownloadReply(QNetworkReply* reply, const QByteArray& response)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    bool success = (statusCode == 200);

    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject data = doc.object();

    QString message;
    if (success) {
        message = "Download successful";
        qDebug() << "AgShareClient: Download successful";
    } else {
        message = QString("Download failed with status %1").arg(statusCode);
        qWarning() << "AgShareClient: Download failed:" << message;
    }

    emit downloadFinished(success, message, data);
}

void AgShareClient::handleOwnFieldsReply(QNetworkReply* reply, const QByteArray& response)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    bool success = (statusCode == 200);

    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonArray fields;

    QString message;
    if (success) {
        if (doc.isArray()) {
            fields = doc.array();
            message = QString("Retrieved %1 fields").arg(fields.size());
            qDebug() << "AgShareClient:" << message;
        } else if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("fields") && obj["fields"].isArray()) {
                fields = obj["fields"].toArray();
                message = QString("Retrieved %1 fields").arg(fields.size());
                qDebug() << "AgShareClient:" << message;
            } else {
                success = false;
                message = "Invalid response format";
                qWarning() << "AgShareClient:" << message;
            }
        }
    } else {
        message = QString("Failed to fetch fields with status %1").arg(statusCode);
        qWarning() << "AgShareClient:" << message;
    }

    emit ownFieldsReceived(success, message, fields);
}

void AgShareClient::handlePublicFieldsReply(QNetworkReply* reply, const QByteArray& response)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    bool success = (statusCode == 200);

    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonArray fields;

    QString message;
    if (success) {
        if (doc.isArray()) {
            fields = doc.array();
            message = QString("Retrieved %1 public fields").arg(fields.size());
            qDebug() << "AgShareClient:" << message;
        } else if (doc.isObject()) {
            QJsonObject obj = doc.object();
            if (obj.contains("fields") && obj["fields"].isArray()) {
                fields = obj["fields"].toArray();
                message = QString("Retrieved %1 public fields").arg(fields.size());
                qDebug() << "AgShareClient:" << message;
            } else {
                success = false;
                message = "Invalid response format";
                qWarning() << "AgShareClient:" << message;
            }
        }
    } else {
        message = QString("Failed to fetch public fields with status %1").arg(statusCode);
        qWarning() << "AgShareClient:" << message;
    }

    emit publicFieldsReceived(success, message, fields);
}
void AgShareClient::handleNetworkError(QNetworkReply* reply, const QString& context)
{
    QString error = getErrorMessage(reply);
    QString message = QString("%1 failed: %2").arg(context).arg(error);
    emit networkError(message);

    qWarning() << "AgShareClient:" << message;
}

bool AgShareClient::validateResponse(QNetworkReply* reply, QByteArray& response)
{
    if (!reply) return false;

    if (reply->error() != QNetworkReply::NoError) {
        QString context = getRequestContext(m_requestTypes.value(reply, None));
        handleNetworkError(reply, context);  // Теперь передаем QString
        return false;
    }

    response = reply->readAll();

    if (response.isEmpty()) {
        emit jsonError("Empty response from server");
        return false;
    }

    return true;
}
QString AgShareClient::getRequestContext(RequestType type) const
{
    switch (type) {
    case ApiCheck: return "API check";
    case Upload: return "upload";
    case Download: return "download";
    case OwnFields: return "fetching fields";
    case PublicFields: return "fetching public fields";
    default: return "network operation";
    }
}
