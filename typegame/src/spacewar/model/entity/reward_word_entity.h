#ifndef __TYPEGAME_REWARD_WORD_ENTITY_H__
#define __TYPEGAME_REWARD_WORD_ENTITY_H__

#include "common/typing/entity/game_entity.h"

#include <QRectF>
#include <QString>

/* ------------------------------------------------------------------
 // 文件名     : reward_word_entity.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : reward_word_entity 头文件声明
------------------------------------------------------------------ */

class RewardWordEntity : public GameEntity
{
public:
    RewardWordEntity();
    explicit RewardWordEntity(int id);
    ~RewardWordEntity() override = default;

    RewardWordEntity(const RewardWordEntity&) = delete;
    RewardWordEntity& operator=(const RewardWordEntity&) = delete;

    void update(qreal deltaTime) override;
    void reset() override;

    QString word() const;
    void setWord(const QString& word);

    int inputIndex() const;
    QString typedPart() const;
    QString remainingPart() const;

    bool handleInput(QChar ch);

    bool isCompleted() const;
    bool isExpired() const;

    int healValue() const;
    void setHealValue(int value);

    int scoreValue() const;
    void setScoreValue(int value);

    QRectF sceneRect() const;
    void setSceneRect(const QRectF& sceneRect);

    qreal outMargin() const;
    void setOutMargin(qreal margin);

protected:
    void onUpdate(qreal deltaTime) override;
    void onReset() override;

private:
    bool isOutOfScene() const;
    void complete();
    void expire();

private:
    QString m_word;
    int m_inputIndex = 0;

    bool m_completed = false;
    bool m_expired = false;

    int m_healValue = 1;
    int m_scoreValue = 50;

    QRectF m_sceneRect;
    qreal m_outMargin = 80.0;
};

#endif // __TYPEGAME_REWARD_WORD_ENTITY_H__
