#include "ai_http_request_config.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QRegularExpression>

namespace {
const QString kResourcePath = QStringLiteral(":/config/ai/ai_word_request.json");
const QRegularExpression kEnvPattern(QStringLiteral(R"(\$\{([^}]+)\})"));

QString bearerTokenFromAuthHeader(const QString& auth)
{
    if (auth.startsWith(QStringLiteral("Bearer "), Qt::CaseInsensitive)) {
        return auth.mid(7).trimmed();
    }
    return auth.trimmed();
}

bool hasUnresolvedEnvSyntax(const QString& s)
{
    return kEnvPattern.match(s).hasMatch();
}

QString authorizationHeaderValue(const QJsonObject& headers)
{
    const QStringList keys = headers.keys();
    for (const QString& key : keys) {
        if (key.compare(QStringLiteral("Authorization"), Qt::CaseInsensitive) == 0) {
            return headers.value(key).toString();
        }
    }
    return QString();
}

bool expandEnvInString(QString* s, QString* errorOut)
{
    if (s == nullptr) {
        return false;
    }
    int pos = 0;
    while (pos < s->size()) {
        const QRegularExpressionMatch m = kEnvPattern.match(*s, pos);
        if (!m.hasMatch()) {
            break;
        }
        const QString varName = m.captured(1).trimmed();
        const QString value = QString::fromUtf8(qgetenv(varName.toUtf8().constData()));
        s->replace(m.capturedStart(), m.capturedLength(), value);
        pos = m.capturedStart() + value.size();
    }
    if (hasUnresolvedEnvSyntax(*s)) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("unresolved ${...} in config after env expand");
        }
        return false;
    }
    return true;
}
} // namespace

bool AiHttpRequestConfig::expandEnvInJson(QJsonValue* value, QString* errorOut)
{
    if (value == nullptr) {
        return false;
    }
    switch (value->type()) {
    case QJsonValue::String: {
        QString s = value->toString();
        if (!expandEnvInString(&s, errorOut)) {
            return false;
        }
        *value = QJsonValue(s);
        return true;
    }
    case QJsonValue::Array: {
        QJsonArray arr = value->toArray();
        for (int i = 0; i < arr.size(); ++i) {
            QJsonValue elem = arr.at(i);
            if (!expandEnvInJson(&elem, errorOut)) {
                return false;
            }
            arr[i] = elem;
        }
        *value = QJsonValue(arr);
        return true;
    }
    case QJsonValue::Object: {
        QJsonObject obj = value->toObject();
        const QStringList keys = obj.keys();
        for (const QString& k : keys) {
            QJsonValue v = obj.value(k);
            if (!expandEnvInJson(&v, errorOut)) {
                return false;
            }
            obj.insert(k, v);
        }
        *value = QJsonValue(obj);
        return true;
    }
    default:
        return true;
    }
}

void AiHttpRequestConfig::substitutePromptPlaceholders(QJsonValue* value,
                                                       const QString& systemPrompt,
                                                       const QString& userPrompt)
{
    if (value == nullptr) {
        return;
    }
    switch (value->type()) {
    case QJsonValue::String: {
        QString s = value->toString();
        s.replace(QStringLiteral("{{SYSTEM_PROMPT}}"), systemPrompt);
        s.replace(QStringLiteral("{{USER_PROMPT}}"), userPrompt);
        *value = QJsonValue(s);
        break;
    }
    case QJsonValue::Array: {
        QJsonArray arr = value->toArray();
        for (int i = 0; i < arr.size(); ++i) {
            QJsonValue elem = arr.at(i);
            substitutePromptPlaceholders(&elem, systemPrompt, userPrompt);
            arr[i] = elem;
        }
        *value = QJsonValue(arr);
        break;
    }
    case QJsonValue::Object: {
        QJsonObject obj = value->toObject();
        const QStringList keys = obj.keys();
        for (const QString& k : keys) {
            QJsonValue v = obj.value(k);
            substitutePromptPlaceholders(&v, systemPrompt, userPrompt);
            obj.insert(k, v);
        }
        *value = QJsonValue(obj);
        break;
    }
    default:
        break;
    }
}

bool AiHttpRequestConfig::loadFromJsonFile(const QString& path,
                                           const QString& systemPrompt,
                                           const QString& userPrompt,
                                           AiHttpRequestConfig* out,
                                           QString* errorOut)
{
    QFile f(path);
    if (!f.exists()) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("missing embedded AI config resource: %1").arg(path);
        }
        return false;
    }
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("cannot open: %1").arg(path);
        }
        return false;
    }

    const QByteArray raw = f.readAll();
    QJsonParseError pe{};
    const QJsonDocument doc = QJsonDocument::fromJson(raw, &pe);
    if (pe.error != QJsonParseError::NoError || !doc.isObject()) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("JSON parse error in %1: %2").arg(path, pe.errorString());
        }
        return false;
    }

    const QJsonObject root = doc.object();
    const QString urlStr = root.value(QStringLiteral("url")).toString().trimmed();
    if (urlStr.isEmpty()) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("config missing non-empty \"url\" (%1)").arg(path);
        }
        return false;
    }

    QJsonObject headersObj = root.value(QStringLiteral("headers")).toObject();
    QJsonObject bodyObj = root.value(QStringLiteral("body")).toObject();
    if (bodyObj.isEmpty()) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("config missing \"body\" object (%1)").arg(path);
        }
        return false;
    }

    const int timeout = root.value(QStringLiteral("timeoutMs")).toInt(3000);
    out->timeoutMs = qMax(500, timeout);
    out->stream = root.value(QStringLiteral("stream")).toBool(false);
    if (out->stream) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("\"stream\" must be false for AiWordService");
        }
        return false;
    }

    QString rk = root.value(QStringLiteral("responseKind")).toString(QStringLiteral("chat_completions")).trimmed();
    if (rk != QStringLiteral("chat_completions") && rk != QStringLiteral("responses")) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("responseKind must be \"chat_completions\" or \"responses\"");
        }
        return false;
    }
    out->responseKind = rk;

    QJsonValue headersVal = headersObj;
    QJsonValue bodyVal = bodyObj;
    substitutePromptPlaceholders(&headersVal, systemPrompt, userPrompt);
    substitutePromptPlaceholders(&bodyVal, systemPrompt, userPrompt);

    QString expandErr;
    if (!expandEnvInJson(&headersVal, &expandErr)) {
        if (errorOut != nullptr) {
            *errorOut = expandErr;
        }
        return false;
    }
    if (!expandEnvInJson(&bodyVal, &expandErr)) {
        if (errorOut != nullptr) {
            *errorOut = expandErr;
        }
        return false;
    }

    out->headers = headersVal.toObject();
    out->body = bodyVal.toObject();
    out->url = urlStr;
    return true;
}

bool AiHttpRequestConfig::resolveAuthorizationBearer(AiHttpRequestConfig* out, QString* errorOut)
{
    if (out == nullptr) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("out is null");
        }
        return false;
    }

    const QString auth = authorizationHeaderValue(out->headers);
    if (auth.trimmed().isEmpty()) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral(
                "headers must include Authorization (e.g. \"Bearer ${ARK_API_KEY}\" in embedded JSON)");
        }
        return false;
    }

    const QString token = bearerTokenFromAuthHeader(auth);
    if (token.isEmpty() || hasUnresolvedEnvSyntax(token)) {
        if (errorOut != nullptr) {
            *errorOut =
                QStringLiteral("Authorization Bearer token empty (set process environment "
                               "ARK_API_KEY before launch; JSON embeds Bearer ${ARK_API_KEY})");
        }
        return false;
    }

    return true;
}

bool AiHttpRequestConfig::loadFromApplicationDir(const QString& systemPrompt,
                                                 const QString& userPrompt,
                                                 AiHttpRequestConfig* out,
                                                 QString* errorOut)
{
    if (out == nullptr) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("out is null");
        }
        return false;
    }
    *out = AiHttpRequestConfig();

    if (qApp == nullptr) {
        if (errorOut != nullptr) {
            *errorOut = QStringLiteral("QCoreApplication not available");
        }
        return false;
    }

    QString loadErr;
    if (!AiHttpRequestConfig::loadFromJsonFile(kResourcePath,
                                               systemPrompt,
                                               userPrompt,
                                               out,
                                               &loadErr)) {
        if (errorOut != nullptr) {
            *errorOut = loadErr;
        }
        return false;
    }

    return AiHttpRequestConfig::resolveAuthorizationBearer(out, errorOut);
}
