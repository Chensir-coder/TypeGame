#ifndef __TYPEGAME_AI_WORD_SERVICE_H__
#define __TYPEGAME_AI_WORD_SERVICE_H__

#include <QMutex>
#include <QObject>
#include <QString>
#include <QStringList>
#include <functional>

/* ------------------------------------------------------------------
 // 文件名     : ai_word_service.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : ai_word_service 头文件声明
------------------------------------------------------------------ */

class AiWordService : public QObject
{
    Q_OBJECT

public:
    using WordCallback = std::function<void(const QString& word, bool fromLocal)>;

public:
    static AiWordService& instance();

    AiWordService(const AiWordService&) = delete;
    AiWordService& operator=(const AiWordService&) = delete;

    void requestRandomWordAsync(const QString& topic,
                                int minLength,
                                int maxLength,
                                WordCallback callback);

private:
    explicit AiWordService(QObject* parent = nullptr);
    ~AiWordService() override = default;

    QString requestWordFromDoubao(const QString& topic,
                                  int minLength,
                                  int maxLength,
                                  const QStringList& avoidWords,
                                  int attemptIndex,
                                  QString* failureReasonOut = nullptr) const;

    QString requestWordFromLocal(int minLength,
                                 int maxLength) const;

    QString buildPrompt(const QString& topic,
                        int minLength,
                        int maxLength,
                        const QStringList& avoidWords,
                        int variationHint) const;

    QStringList snapshotRecentAiWords() const;
    void rememberSuccessfulAiWord(const QString& wordLower) const;

    QString parseWordFromResponse(const QByteArray& responseBody,
                                  const QString& responseKind,
                                  QString* failureReasonOut = nullptr) const;

    bool isValidWord(const QString& word,
                     int minLength,
                     int maxLength) const;

private:
    QStringList m_localWords;

    mutable QMutex m_recentAiMutex;
    /** Newest first; used to diversify repeated prompts. */
    mutable QStringList m_recentAiWords;
};

#endif // __TYPEGAME_AI_WORD_SERVICE_H__
