#ifndef __TYPEGAME_SIMPLEEFFECTS_H__
#define __TYPEGAME_SIMPLEEFFECTS_H__

#include "effectitem.h"
#include <QtGlobal>
#include <QtMath>

/* ------------------------------------------------------------------
 // 文件名     : simpleeffects.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : simpleeffects 头文件声明
------------------------------------------------------------------ */

// 一些简单的预设效果类，方便快速创建常见的效果类型。
// 你可以根据需要添加更多的预设效果类，或者直接使用 EffectItem 来创建自定义效果。
// 这些预设效果类都继承自 EffectItem，并重写了 onUpdate 和/或 updateMovement 方法来实现特定的动画行为。

class FadeEffectItem : public EffectItem
{
// 逐渐淡出效果，适合用于消失的对象。
public:
    using EffectItem::EffectItem;

protected:
    void onUpdate(qreal dt) override
    {
        Q_UNUSED(dt);

        qreal t = elapsedTime() / lifeTime();
        t = qBound<qreal>(0.0, t, 1.0);

        setOpacity(1.0 - t);
    }
};

class ScaleEffectItem : public EffectItem
{
// 逐渐放大效果，适合用于出现的对象。
public:
    using EffectItem::EffectItem;

protected:
    void onUpdate(qreal dt) override
    {
        Q_UNUSED(dt);

        qreal t = elapsedTime() / lifeTime();
        t = qBound<qreal>(0.0, t, 1.0);

        setLocalScale(1.0 + 0.5 * t);
    }
};

class FadeScaleEffectItem : public EffectItem
{

// 淡出并放大效果，适合用于消失的对象，增加一些动感。
public:
    using EffectItem::EffectItem;

protected:
    void onUpdate(qreal dt) override
    {
        Q_UNUSED(dt);

        qreal t = elapsedTime() / lifeTime();
        t = qBound<qreal>(0.0, t, 1.0);

        setOpacity(1.0 - t);
        setLocalScale(1.0 + 0.5 * t);
    }
};

class FloatUpFadeEffectItem : public EffectItem
{
// 向上漂浮并淡出效果，适合用于消失的对象，增加一些动感。
public:
    using EffectItem::EffectItem;

protected:
    void updateMovement(qreal dt) override
    {
        QPointF pos = logicalPosition();
        pos.ry() -= 80.0 * dt;
        setLogicalPosition(pos);
        EffectItem::updateMovement(dt);
    }

    void onUpdate(qreal dt) override
    {
        Q_UNUSED(dt);

        qreal t = elapsedTime() / lifeTime();
        t = qBound<qreal>(0.0, t, 1.0);

        setOpacity(1.0 - t);
    }
};

class SwingEffectItem : public EffectItem
{

// 摇摆漂浮并淡出效果，适合用于消失的对象，增加一些动感。
// 水平方向：v_x = A*sin(ωt)，每步位移 v_x*dt；再叠加基类 velocity。
public:
    using EffectItem::EffectItem;

protected:
    void updateMovement(qreal dt) override
    {
        constexpr qreal kAngularFreq = 10.0;
        constexpr qreal kHorizontalAmp = 40.0;
        constexpr qreal kRiseSpeed = 50.0;

        QPointF pos = logicalPosition();
        const qreal vx = kHorizontalAmp * qSin(elapsedTime() * kAngularFreq);
        pos.rx() += vx * dt;
        pos.ry() -= kRiseSpeed * dt;
        setLogicalPosition(pos);
        EffectItem::updateMovement(dt);
    }

    void onUpdate(qreal dt) override
    {
        Q_UNUSED(dt);

        qreal t = elapsedTime() / lifeTime();
        t = qBound<qreal>(0.0, t, 1.0);

        setOpacity(1.0 - t);
        setRotation(360.0 * t);
    }
};

#endif // __TYPEGAME_SIMPLEEFFECTS_H__
