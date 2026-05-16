#include "ai_http_client.h"

#include <QEventLoop>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>

bool AiHttpClient::postJson(const AiHttpRequestConfig& config,
                            QByteArray* responseBodyOut,
                            QString* errorOut)
{
    if (responseBodyOut != nullptr) {
        responseBodyOut->clear();
    }

    if (config.url.trimmed().isEmpty()) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("empty url");
        }
        return false;
    }

    QNetworkAccessManager manager;
    QNetworkRequest request{QUrl(config.url)};

    bool hasContentType = false;
    const QStringList headerKeys = config.headers.keys();
    for (const QString& key : headerKeys) {
        const QByteArray name = key.toUtf8();
        const QByteArray value = config.headers.value(key).toString().toUtf8();
        if (name.isEmpty()) {
            continue;
        }
        if (QString::fromUtf8(name).compare(QStringLiteral("Content-Type"), Qt::CaseInsensitive) == 0) {
            request.setHeader(QNetworkRequest::ContentTypeHeader, QString::fromUtf8(value));
            hasContentType = true;
        } else {
            request.setRawHeader(name, value);
        }
    }
    if (!hasContentType) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    }

    const QByteArray requestBody =
        QJsonDocument(config.body).toJson(QJsonDocument::Compact);

    QNetworkReply* reply = manager.post(request, requestBody);

    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start(config.timeoutMs);
    loop.exec();

    if (timer.isActive()) {
        timer.stop();
    } else {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("request timeout (%1ms)").arg(config.timeoutMs);
        }
        reply->abort();
        reply->deleteLater();
        return false;
    }

    if (reply->error() != QNetworkReply::NoError) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("network: %1").arg(reply->errorString());
        }
        reply->deleteLater();
        return false;
    }

    const QByteArray responseBody = reply->readAll();
    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    reply->deleteLater();

    if (httpStatus < 200 || httpStatus >= 300) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("HTTP %1: %2")
                            .arg(httpStatus)
                            .arg(QString::fromUtf8(responseBody.left(512)));
        }
        return false;
    }

    if (responseBodyOut != nullptr) {
        *responseBodyOut = responseBody;
    }
    return true;
}
