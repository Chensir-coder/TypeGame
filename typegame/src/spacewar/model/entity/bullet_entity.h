#ifndef __TYPEGAME_BULLET_ENTITY_H__
#define __TYPEGAME_BULLET_ENTITY_H__

#include "common/typing/entity/game_entity.h"
#include "enemy_entity.h"

/* ------------------------------------------------------------------
 // 文件名     : bullet_entity.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : bullet_entity 头文件声明
------------------------------------------------------------------ */

class BulletEntity : public GameEntity
{
public:
    BulletEntity();
    explicit BulletEntity(int id);
    ~BulletEntity() override = default;

    BulletEntity(const BulletEntity&) = delete;
    BulletEntity& operator=(const BulletEntity&) = delete;

    void update(qreal deltaTime) override;
    void reset() override;

    EnemyEntity* target() const;
    void setTarget(EnemyEntity* target);

    qreal speed() const;
    void setSpeed(qreal speed);

    qreal hitRadius() const;
    void setHitRadius(qreal radius);

    bool hasHitTarget() const;

protected:
    void onUpdate(qreal deltaTime) override;
    void onReset() override;

private:
    QPointF centerPosition() const;
    QPointF targetCenterPosition() const;

    bool isTargetValid() const;
    bool shouldHitTarget(qreal distance) const;

    void moveToTarget(qreal deltaTime);
    void hitTarget();
    void destroySelf();

private:
    EnemyEntity* m_target = nullptr;

    qreal m_speed = 500.0;
    qreal m_hitRadius = 20.0;

    bool m_hasHitTarget = false;
};

#endif // __TYPEGAME_BULLET_ENTITY_H__
