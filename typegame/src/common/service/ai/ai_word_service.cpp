#include "ai_word_service.h"

#include "ai_http_client.h"
#include "ai_http_request_config.h"

#include <QCoreApplication>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaObject>
#include <QMutexLocker>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QThread>
#include <QSet>

namespace {
constexpr int kMaxTrackedRecentWords = 24;
constexpr int kMaxAiAvoidRetries = 3;
} // namespace

AiWordService& AiWordService::instance()
{
    
    static AiWordService service; 
    return service;
}

AiWordService::AiWordService(QObject* parent)
    : QObject(parent),
      m_localWords({
          "space",
          "planet",
          "rocket",
          "laser",
          "meteor",
          "galaxy",
          "comet",
          "orbit",
          "star",
          "alien",
          "ship",
          "energy",
          "fighter",
          "shield",
          "cosmos"
      })
{
}

void AiWordService::requestRandomWordAsync(const QString& topic,
                                           int minLength,
                                           int maxLength,
                                           WordCallback callback)
{
    const QString safeTopic = topic.trimmed().isEmpty() ? "space war" : topic.trimmed();
    const int safeMinLength = qMax(1, minLength);
    const int safeMaxLength = qMax(safeMinLength, maxLength);

    QThread* workerThread = QThread::create([=]() {
        QString word;
        bool fromLocal = false;

        const QStringList initialAvoid = snapshotRecentAiWords();
        QStringList sessionRejected;
        QString aiFailureReason;
        QString wordFromAi;
        QString lastValidAiLower;

        for (int attempt = 0; attempt < kMaxAiAvoidRetries; ++attempt) {
            QSet<QString> avoidLower;
            for (const QString& w : initialAvoid)
                avoidLower.insert(w.toLower());
            for (const QString& w : sessionRejected)
                avoidLower.insert(w.toLower());

            QStringList avoidList;
            avoidList.reserve(avoidLower.size());
            for (const QString& aw : avoidLower)
                avoidList.append(aw);

            QString attemptFailReason;
            const QString cand = requestWordFromDoubao(
                safeTopic,
                safeMinLength,
                safeMaxLength,
                avoidList,
                attempt,
                &attemptFailReason);

            if (!isValidWord(cand, safeMinLength, safeMaxLength)) {
                wordFromAi = cand;
                aiFailureReason = attemptFailReason;
                break;
            }

            const QString lc = cand.toLower();
            lastValidAiLower = lc;

            if (!avoidLower.contains(lc)) {
                wordFromAi = lc;
                aiFailureReason.clear();
                rememberSuccessfulAiWord(lc);
                break;
            }

            sessionRejected.append(lc);
        }

        if (wordFromAi.isEmpty() && !lastValidAiLower.isEmpty()) {
            wordFromAi = lastValidAiLower;
            aiFailureReason.clear();
            rememberSuccessfulAiWord(lastValidAiLower);
        }

        word = wordFromAi;

        if (!isValidWord(word, safeMinLength, safeMaxLength)) {
            word = requestWordFromLocal(safeMinLength, safeMaxLength);
            fromLocal = true;
            qWarning() << "[AiWordService]"
                       << "reason:"
                       << (aiFailureReason.isEmpty() ? QStringLiteral("unknown") : aiFailureReason)
                       << "localWord:" << word;
        }

        QMetaObject::invokeMethod(
            qApp,
            [callback, word, fromLocal]() {
                if (callback) {
                    callback(word, fromLocal);
                }
            },
            Qt::QueuedConnection);
    });

    QObject::connect(workerThread, &QThread::finished, workerThread, &QObject::deleteLater);
    workerThread->start();
}

QString AiWordService::requestWordFromDoubao(const QString& topic,
                                             int minLength,
                                             int maxLength,
                                             const QStringList& avoidWords,
                                             int attemptIndex,
                                             QString* failureReasonOut) const
{
    static const QString kSystemPrompt =
        QStringLiteral("You are a word generator for a typing game. "
                       "Return only one English word. No punctuation. No explanation.");

    const QString userPrompt = buildPrompt(topic, minLength, maxLength, avoidWords, attemptIndex);

    AiHttpRequestConfig cfg;
    QString loadErr;
    if (!AiHttpRequestConfig::loadFromApplicationDir(kSystemPrompt, userPrompt, &cfg, &loadErr)) {
        if (failureReasonOut) {
            *failureReasonOut = loadErr;
        }
        return QString();
    }

    QByteArray responseBody;
    QString httpErr;
    if (!AiHttpClient::postJson(cfg, &responseBody, &httpErr)) {
        if (failureReasonOut) {
            *failureReasonOut = httpErr;
        }
        return QString();
    }

    QString parseFailure;
    const QString word = parseWordFromResponse(responseBody, cfg.responseKind, &parseFailure);

    if (word.isEmpty()) {
        if (failureReasonOut) {
            *failureReasonOut = parseFailure.isEmpty()
                ? QStringLiteral("failed to parse word from API response")
                : parseFailure;
        }
        return QString();
    }

    if (!isValidWord(word, minLength, maxLength)) {
        if (failureReasonOut) {
            *failureReasonOut = QStringLiteral("model output rejected: \"%1\" (need length %2-%3)")
                .arg(word)
                .arg(minLength)
                .arg(maxLength);
        }
        return QString();
    }

    qDebug() << "[AiWordService] Request URL:" << cfg.url
             << "body:" << QString::fromUtf8(QJsonDocument(cfg.body).toJson(QJsonDocument::Compact));
    qDebug() << "[AiWordService] Response:" << QString::fromUtf8(responseBody);
    return word.toLower();
}

QString AiWordService::requestWordFromLocal(int minLength,
                                            int maxLength) const
{
    QStringList candidates;

    for (const QString& word : m_localWords) {
        if (isValidWord(word, minLength, maxLength)) {
            candidates.append(word.toLower());
        }
    }

    if (candidates.isEmpty()) {
        return "space";
    }

    const int index = QRandomGenerator::global()->bounded(candidates.size());
    return candidates.at(index);
}

QStringList AiWordService::snapshotRecentAiWords() const
{
    QMutexLocker locker(&m_recentAiMutex);
    return m_recentAiWords;
}

void AiWordService::rememberSuccessfulAiWord(const QString& wordLower) const
{
    QMutexLocker locker(&m_recentAiMutex);
    const QString w = wordLower.toLower();
    m_recentAiWords.removeAll(w);
    m_recentAiWords.prepend(w);
    while (m_recentAiWords.size() > kMaxTrackedRecentWords)
        m_recentAiWords.removeLast();
}

QString AiWordService::buildPrompt(const QString& topic,
                                   int minLength,
                                   int maxLength,
                                   const QStringList& avoidWords,
                                   int attemptIndex) const
{
    static const char* const kAngles[] = {
        "Favor spacecraft, satellites, launches, navigation, or orbital motion.",
        "Favor stars, galaxies, dust, cosmic scale, astronomy, distances.",
        "Favor engines, reactors, shields, scanners, circuitry, robotics.",
        "Favor battle tactics, fleets, interception, bombardment, squadrons.",
        "Favor hazards: debris belts, micrometeors, radiation, anomalies.",
        "Favor aliens, ecology, habitats, diplomacy, explorers.",
        "Favor lasers, pulses, arcs, explosions, kinetic weapons.",
        "Favor rare or specific vocabulary over common scifi clichés."
    };

    constexpr int angles = int(sizeof(kAngles) / sizeof(kAngles[0]));
    const int pivot = QRandomGenerator::global()->bounded(angles);
    const QString angle =
        QString::fromUtf8(kAngles[(pivot + qMax(0, attemptIndex)) % angles]);

    QString exclude;
    if (!avoidWords.isEmpty()) {
        QStringList lowers;
        lowers.reserve(avoidWords.size());
        for (const QString& w : avoidWords)
            lowers.append(w.toLower());
        lowers.removeDuplicates();
        exclude = QStringLiteral(
                      "Do not repeat any word from this exclusion list "
                      "(they were already picked recently): %1.\n")
                      .arg(lowers.join(QStringLiteral(", ")));
    }

    return exclude
           + QStringLiteral(
               "Generate exactly one English word for a typing game. "
               "Topic: %1. "
               "The word length must be between %2 and %3 characters. "
               "Return only the word. Do not return a sentence.\n")
                 .arg(topic)
                 .arg(minLength)
                 .arg(maxLength)
           + angle + QStringLiteral("\n"
               "The word MUST differ from anything in the exclusion list if present.");
}

QString AiWordService::parseWordFromResponse(const QByteArray& responseBody,
                                             const QString& responseKind,
                                             QString* failureReasonOut) const
{
    if (failureReasonOut) {
        failureReasonOut->clear();
    }

    QString content;
    if (responseKind == QStringLiteral("responses")) {
        const QJsonDocument document = QJsonDocument::fromJson(responseBody);
        if (!document.isObject()) {
            if (failureReasonOut) {
                *failureReasonOut = QStringLiteral("response is not a JSON object");
            }
            return QString();
        }
        const QJsonObject root = document.object();
        const QJsonArray output = root.value(QStringLiteral("output")).toArray();
        QStringList parts;
        for (const QJsonValue& block : output) {
            const QJsonObject blockObj = block.toObject();
            const QJsonArray contentArr = blockObj.value(QStringLiteral("content")).toArray();
            for (const QJsonValue& item : contentArr) {
                const QJsonObject itemObj = item.toObject();
                const QString type = itemObj.value(QStringLiteral("type")).toString();
                const QString text = itemObj.value(QStringLiteral("text")).toString();
                if ((type == QStringLiteral("output_text") || type == QStringLiteral("input_text")
                     || type == QStringLiteral("text"))
                    && !text.isEmpty()) {
                    parts.append(text);
                }
            }
        }
        content = parts.join(QString()).trimmed();
        if (content.isEmpty()) {
            if (failureReasonOut) {
                *failureReasonOut = QStringLiteral("no text in responses API output");
            }
            return QString();
        }
    } else {
        const QJsonDocument document = QJsonDocument::fromJson(responseBody);

        if (!document.isObject()) {
            if (failureReasonOut) {
                *failureReasonOut = QStringLiteral("response is not a JSON object");
            }
            return QString();
        }

        const QJsonObject root = document.object();

        const QJsonArray choices = root.value(QStringLiteral("choices")).toArray();

        if (choices.isEmpty()) {
            if (failureReasonOut) {
                *failureReasonOut = QStringLiteral("empty choices in API response");
            }
            return QString();
        }

        const QJsonObject firstChoice = choices.first().toObject();
        const QJsonObject message = firstChoice.value(QStringLiteral("message")).toObject();

        content = message.value(QStringLiteral("content")).toString().trimmed();
    }

    content.remove('"');
    content.remove('\'');
    content.remove('.');
    content.remove(',');
    content.remove(':');
    content.remove(';');

    const QRegularExpression regex(QStringLiteral("[A-Za-z]+"));
    const QRegularExpressionMatch match = regex.match(content);

    if (!match.hasMatch()) {
        if (failureReasonOut) {
            *failureReasonOut = QStringLiteral("no English word in model content");
        }
        return QString();
    }

    const QString result = match.captured(0).toLower();

    return result;
}

bool AiWordService::isValidWord(const QString& word,
                                int minLength,
                                int maxLength) const
{
    const QString trimmed = word.trimmed();

    if (trimmed.length() < minLength || trimmed.length() > maxLength) {
        return false;
    }

    const QRegularExpression regex("^[A-Za-z]+$");
    return regex.match(trimmed).hasMatch();
}
