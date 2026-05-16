#ifndef __TYPEGAME_ENEMY_ENTITY_H__
#define __TYPEGAME_ENEMY_ENTITY_H__

#include <QRectF>

#include "common/typing/typing_target_entity.h"

/* ------------------------------------------------------------------
 // 文件名     : enemy_entity.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : enemy_entity 头文件声明
------------------------------------------------------------------ */

/**
 * 飞机大战敌机实体。
 *
 * 在通用 TypingTargetEntity 之上扩展：
 *   1. 三种下落轨迹（直线 / 正弦摆动 / 左右反弹）。
 *   2. 锁定状态：被玩家输入命中后只允许触发一颗追踪子弹，不再重复锁定。
 *   3. 外观变体：选用不同的敌机精灵表资源。
 *   4. 沿场景左右边界 clamp 或反弹，防止飞出。
 */
class EnemyEntity : public TypingTargetEntity
{
public:
    enum class Trajectory
    {
        Straight = 0,
        Sine,
        Bounce
    };

    enum class Variant
    {
        Type0 = 0,
        Type4
    };

public:
    EnemyEntity();
    ~EnemyEntity() override = default;

    bool isLocked() const;
    void setLocked(bool locked);

    Trajectory trajectory() const;
    void setTrajectory(Trajectory trajectory);

    Variant variant() const;
    void setVariant(Variant variant);

    qreal startX() const;
    qreal sineAmplitude() const;
    qreal sinePhase() const;
    qreal sineAngularSpeed() const;

    void configureStraight(qreal startX, qreal verticalSpeed);

    void configureSine(qreal startX,
                       qreal verticalSpeed,
                       qreal amplitude,
                       qreal angularSpeed,
                       qreal phaseOffset);

    void configureBounce(qreal startX,
                         qreal verticalSpeed,
                         qreal horizontalSpeed);

    /**
     * 是否完全飞出场景下边界。
     */
    bool isBelowScene() const;

protected:
    void onUpdate(qreal deltaTime) override;
    void onReset() override;

private:
    void updateStraight(qreal deltaTime);
    void updateSine(qreal deltaTime);
    void updateBounce(qreal deltaTime);

    void clampInsideHorizontalBounds();
    bool reflectIfHittingHorizontalBound();

private:
    bool m_locked = false;

    Trajectory m_trajectory = Trajectory::Straight;
    Variant m_variant = Variant::Type0;

    qreal m_startX = 0.0;
    qreal m_verticalSpeed = 0.0;

    /* sine */
    qreal m_sineAmplitude = 0.0;
    qreal m_sineAngularSpeed = 0.0;
    qreal m_sinePhase = 0.0;
    qreal m_elapsedSeconds = 0.0;

    /* bounce */
    qreal m_horizontalSpeed = 0.0;
};

#endif // __TYPEGAME_ENEMY_ENTITY_H__
