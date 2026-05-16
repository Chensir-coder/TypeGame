#ifndef __TYPEGAME_PLAYER_ENTITY_H__
#define __TYPEGAME_PLAYER_ENTITY_H__

#include "common/typing/entity/game_entity.h"

#include <QRectF>

/* ------------------------------------------------------------------
 // 文件名     : player_entity.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : player_entity 头文件声明
------------------------------------------------------------------ */

class TypingGameConfig;
class QPainter;

class PlayerEntity : public GameEntity
{
public:
    enum class MoveDirection
    {
        None,
        Left,
        Right,
        Up,
        Down
    };

public:
    PlayerEntity();
    explicit PlayerEntity(int id);
    ~PlayerEntity() override = default;

    PlayerEntity(const PlayerEntity&) = delete;
    PlayerEntity& operator=(const PlayerEntity&) = delete;

    void update(qreal deltaTime) override;
    void reset() override;

    int health() const;
    void setHealth(int health);

    int maxHealth() const;
    void setMaxHealth(int maxHealth);

    bool isDead() const;

    void damage(int value);
    void heal(int value);

    qreal moveSpeed() const;
    void setMoveSpeed(qreal speed);

    MoveDirection moveDirection() const;
    void setMoveDirection(MoveDirection direction);

    /**
     * 同时按住的方向集合（可以包含 Left/Right/Up/Down）。
     * 出现冲突方向时（同时按下 Left+Right 或 Up+Down）相互抵消。
     */
    void setHorizontalAxis(int axis);
    void setVerticalAxis(int axis);
    int horizontalAxis() const;
    int verticalAxis() const;

    QRectF sceneRect() const;
    void setSceneRect(const QRectF& sceneRect);

    void applyPlaybackFromTypingGameConfig(const TypingGameConfig& cfg);
    void triggerCollisionShieldFx();
    void paint(QPainter* painter, const QRectF& viewRect, qreal viewScale) const;

    QPointF gunMuzzlePosition() const;

protected:
    void onUpdate(qreal deltaTime) override;
    void onReset() override;

private:
    void updateVelocityByDirection();
    void clampInScene();
    void paintCollisionShield(QPainter* painter, const QRectF& viewRect, qreal viewScale) const;
    void paintHealthDeltaFx(QPainter* painter, const QRectF& viewRect, qreal viewScale) const;
    void triggerHealthDeltaFx(int deltaHealth);

private:
    int m_health = 10;
    int m_maxHealth = 10;

    qreal m_moveSpeed = 320.0;
    MoveDirection m_moveDirection = MoveDirection::None;

    int m_horizontalAxis = 0;
    int m_verticalAxis = 0;

    QRectF m_sceneRect;
    qreal m_collisionShieldRemainSec = 0.0;
    qreal m_collisionShieldElapsedSec = 0.0;
    qreal m_healthDeltaFxRemainSec = 0.0;
    qreal m_healthDeltaFxElapsedSec = 0.0;
    int m_healthDeltaValue = 0;
};

#endif // __TYPEGAME_PLAYER_ENTITY_H__
