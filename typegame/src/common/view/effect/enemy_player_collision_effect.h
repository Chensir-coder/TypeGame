#ifndef __TYPEGAME_ENEMY_PLAYER_COLLISION_EFFECT_H__
#define __TYPEGAME_ENEMY_PLAYER_COLLISION_EFFECT_H__

#include "common/view/effect/effectitem.h"

/* ------------------------------------------------------------------
 // 文件名     : enemy_player_collision_effect.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : enemy_player_collision_effect 头文件声明
------------------------------------------------------------------ */

class EnemyPlayerCollisionEffect final : public EffectItem
{
public:
    EnemyPlayerCollisionEffect(const QPointF& logical_center,
                               const QSizeF& logical_size,
                               qreal impact_angle_degrees = -25.0);

    void update(qreal dt) override;
    void paint(QPainter* painter,
               qreal viewScale,
               const QPointF& cameraOffset = QPointF(0.0, 0.0)) const override;

private:
    qreal m_impactAngleDegrees = -25.0;
};

#endif // __TYPEGAME_ENEMY_PLAYER_COLLISION_EFFECT_H__
