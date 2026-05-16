#ifndef __TYPEGAME_TYPING_TARGET_ENTITY_H__
#define __TYPEGAME_TYPING_TARGET_ENTITY_H__

#include "common/typing/entity/game_entity.h"

#include <QChar>
#include <QRectF>
#include <QString>

/* ------------------------------------------------------------------
 // 文件名     : typing_target_entity.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : typing_target_entity 头文件声明
------------------------------------------------------------------ */

/*
 * 公共层语义调整：
 *
 * 以前的 ReachedFailLine 表示“到达失败线”，公共层会直接把它当 miss。
 * 现在改成 ReachedExitLine，表示“到达退场线”。
 *
 * 苹果游戏：
 *   退场线 = 70%，退场 = miss。
 *
 * 飞机大战：
 *   退场线 = 100%，退场 = 自然飞出屏幕，不算 miss。
 *
 * 为了兼容旧代码，保留 ReachedFailLine 作为 ReachedExitLine 的别名。
 */
enum class TypingTargetDestroyReason
{
    None, // 未被销毁
    Matched, // 被命中

    ReachedExitLine, // 到达退场线
    ReachedFailLine = ReachedExitLine, // 到达失败线

    HitByBullet, // 被子弹命中
    CollisionWithPlayer, // 与玩家碰撞
    OutOfScene // 超出场景
};

class TypingTargetEntity : public GameEntity
{
public:
    TypingTargetEntity();
    explicit TypingTargetEntity(int id);
    ~TypingTargetEntity() override = default;

    TypingTargetEntity(const TypingTargetEntity&) = delete;
    TypingTargetEntity& operator=(const TypingTargetEntity&) = delete;

    TypingTargetEntity(TypingTargetEntity&&) noexcept = default;
    TypingTargetEntity& operator=(TypingTargetEntity&&) noexcept = default;

    void reset() override;

    QString text() const;
    void setText(const QString& text);

    QChar letter() const;
    void setLetter(QChar letter);

    bool matches(QChar input) const;
    bool matches(const QString& input) const;

    int scoreValue() const;
    void setScoreValue(int value);

    QRectF sceneRect() const;
    void setSceneRect(const QRectF& rect);

    /*
     * 新语义：exitLineRatio / exitLineY。
     * 它只表示“目标到达这里后应该退场”，不代表失败。
     */
    qreal exitLineRatio() const;
    void setExitLineRatio(qreal ratio);

    qreal exitLineY() const;
    bool hasReachedExitLine() const;

    /*
     * 兼容旧接口。
     * 旧代码如果还调用 failLineRatio / setFailLineRatio / hasReachedFailLine，
     * 仍然可以工作，但语义上建议逐步改成 exitLine。
     */
    qreal failLineRatio() const;
    void setFailLineRatio(qreal ratio);

    qreal failLineY() const;
    bool hasReachedFailLine() const;

    TypingTargetDestroyReason destroyReason() const;
    bool isDestroyed() const;

    void markMatched();
    void markReachedExitLine();
    void markReachedFailLine();
    void markHitByBullet();
    void markCollisionWithPlayer();
    void markOutOfScene();

    /*
     * 精灵循环（仅表现层消费）：帧数须与视图侧 PixmapSliceHelper 切帧数一致。
     * 由 GameEntity::update 中与游戏同一时间步 Δt 推进。
     */
    void configureSpriteAnimation(int frame_count, qreal frames_per_second);
    int spriteAnimationFrameCount() const;
    int spriteAnimationFrameIndex() const;

protected:
    void onUpdate(qreal deltaTime) override;
    void onReset() override;

    virtual void destroy(TypingTargetDestroyReason reason);

private:
    QString m_text;
    int m_scoreValue = 10;

    QRectF m_sceneRect;

    /*
     * 注意：这里已经不是“失败线”，而是“退场线”。
     */
    qreal m_exitLineRatio = 0.7;

    TypingTargetDestroyReason m_destroyReason = TypingTargetDestroyReason::None;
};

#endif // __TYPEGAME_TYPING_TARGET_ENTITY_H__
