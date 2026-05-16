#ifndef __TYPEGAME_REWARD_EXPIRE_EFFECT_H__
#define __TYPEGAME_REWARD_EXPIRE_EFFECT_H__

#include "common/view/effect/effectitem.h"

#include <QString>

/* ------------------------------------------------------------------
 // 文件名     : reward_expire_effect.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : reward_expire_effect 头文件声明
------------------------------------------------------------------ */

class RewardExpireEffect final : public EffectItem
{
public:
    RewardExpireEffect(const QPointF& logical_center,
                       const QSizeF& logical_size,
                       const QString& word);

    void update(qreal dt) override;
    void paint(QPainter* painter,
               qreal viewScale,
               const QPointF& cameraOffset = QPointF(0.0, 0.0)) const override;

private:
    QString m_word;
};

#endif // __TYPEGAME_REWARD_EXPIRE_EFFECT_H__
