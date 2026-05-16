#ifndef __TYPEGAME_DIFFICULTY_NOTICE_EFFECT_H__
#define __TYPEGAME_DIFFICULTY_NOTICE_EFFECT_H__

#include "common/view/effect/effectitem.h"

#include <QString>

/* ------------------------------------------------------------------
 // 文件名     : difficulty_notice_effect.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : difficulty_notice_effect 头文件声明
------------------------------------------------------------------ */

/// 难度升级类 UI 提示：总时长 2 秒，由透明渐显后淡出。
class DifficultyNoticeEffect final : public EffectItem
{
public:
    explicit DifficultyNoticeEffect(
        const QPointF& logical_center,
        const QSizeF& logical_size,
        const QString& message = QStringLiteral("难度升级"));

    void update(qreal dt) override;
    void paint(QPainter* painter,
               qreal viewScale,
               const QPointF& cameraOffset = QPointF(0.0, 0.0)) const override;

private:
    QString m_message;
};

#endif // __TYPEGAME_DIFFICULTY_NOTICE_EFFECT_H__
