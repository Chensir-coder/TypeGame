#ifndef __TYPEGAME_AI_HTTP_REQUEST_CONFIG_H__
#define __TYPEGAME_AI_HTTP_REQUEST_CONFIG_H__

#include <QByteArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>

/**
 * Loads Ark-compatible HTTP template (URL, headers, body) for AiWordService.
 * Source: Qt resource path :/config/ai/ai_word_request.json (typegame_assets.qrc).
 * ${ENV_NAME} in JSON is expanded via process env (e.g. ARK_API_KEY).
 */
struct AiHttpRequestConfig
{
    QString url;
    /** Header name -> value (after env expansion and placeholders). */
    QJsonObject headers;
    QJsonObject body;
    int timeoutMs = 3000;
    bool stream = false;
    /** "chat_completions" | "responses" -- parsing mode for AiWordService. */
    QString responseKind = QStringLiteral("chat_completions");

    /**
     * Load embedded config and validate Authorization Bearer.
     * Replaces {{SYSTEM_PROMPT}}, {{USER_PROMPT}} from JSON strings.
     */
    static bool loadFromApplicationDir(const QString& systemPrompt,
                                       const QString& userPrompt,
                                       AiHttpRequestConfig* out,
                                       QString* errorOut);

private:
    static bool loadFromJsonFile(const QString& path,
                                 const QString& systemPrompt,
                                 const QString& userPrompt,
                                 AiHttpRequestConfig* out,
                                 QString* errorOut);
    static bool resolveAuthorizationBearer(AiHttpRequestConfig* out, QString* errorOut);
    static bool expandEnvInJson(QJsonValue* value, QString* errorOut);
    static void substitutePromptPlaceholders(QJsonValue* value,
                                             const QString& systemPrompt,
                                             const QString& userPrompt);
};

#endif // __TYPEGAME_AI_HTTP_REQUEST_CONFIG_H__
