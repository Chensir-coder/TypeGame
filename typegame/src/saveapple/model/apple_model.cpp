/* ------------------------------------------------------------------
// 文件名     : apple_model.cpp
// 创建者     : 3263297739@qq.com
// 创建时间   : 2026-04-13
// 功能描述   : 拯救苹果游戏中的苹果实体模型实现
------------------------------------------------------------------ */

#include "apple_model.h"

AppleModel::AppleModel()
{
    reset();
}

int AppleModel::x() const
{
    return static_cast<int>(position().x());
}

int AppleModel::y() const
{
    return static_cast<int>(position().y());
}

void AppleModel::setPosition(int x, int y)
{
    GameObjectBase::setPosition(QPointF(static_cast<qreal>(x), static_cast<qreal>(y)));
}

int AppleModel::fallSpeed() const
{
    return static_cast<int>(velocity().y());
}

void AppleModel::setFallSpeed(int fall_speed)
{
    if (fall_speed <= 0)
    {
        return;
    }

    setVelocity(QPointF(velocity().x(), static_cast<qreal>(fall_speed)));
}

bool AppleModel::isHit() const
{
    return destroyReason() == TypingTargetDestroyReason::Matched;
}

void AppleModel::setHit(bool is_hit)
{
    if (is_hit)
    {
        markMatched();
    }
    // setHit(false): no-op，对象池回收时由 reset() 统一清除
}

void AppleModel::update(int delta_ms)
{
    if (!isActive())
    {
        return;
    }

    if (delta_ms <= 0)
    {
        return;
    }

    TypingTargetEntity::update(static_cast<qreal>(delta_ms) / 1000.0);
}

void AppleModel::reset()
{
    TypingTargetEntity::reset();
}

void AppleModel::onReset()
{
    TypingTargetEntity::onReset();

    // 恢复默认下落速度 120 px/s
    setVelocity(QPointF(0.0, 120.0));
}
