#ifndef __TYPEGAME_APPLE_MODEL_H__
#define __TYPEGAME_APPLE_MODEL_H__

#include "common/typing/typing_target_entity.h"
#include "common/pool/ipoolable.h"

/* ------------------------------------------------------------------
// 文件名     : apple_model.h
// 创建者     : 3263297739@qq.com
// 创建时间   : 2026-04-13
// 功能描述   : 拯救苹果游戏中的苹果实体模型
------------------------------------------------------------------ */

/**
 * @brief 拯救苹果游戏中的苹果实体模型。
 *
 * 现在继承自 TypingTargetEntity，复用公共 typing 实体能力：
 *   - 位置/速度通过 GameObjectBase::position() / velocity() 管理
 *   - 字母通过 TypingTargetEntity::letter() / setLetter() 管理
 *   - 退场线检测通过 TypingTargetEntity::hasReachedExitLine() 管理
 *   - 激活状态通过 GameEntity::isActive() / setActive() 管理
 *
 * 保留旧接口作为兼容包装，View / Controller / 测试层无需修改：
 *   x(), y(), setPosition(int,int), fallSpeed(), setFallSpeed(), isHit(), setHit()
 *   update(int delta_ms) ← 毫秒版兼容入口
 */
class AppleModel final : public TypingTargetEntity, public IPoolable
{
public:
    AppleModel();
    ~AppleModel() override = default;

    AppleModel(const AppleModel&) = delete;
    AppleModel& operator=(const AppleModel&) = delete;

    AppleModel(AppleModel&&) noexcept = default;
    AppleModel& operator=(AppleModel&&) noexcept = default;

    /* ---- 兼容旧接口：整数坐标 ---- */
    int x() const;
    int y() const;
    void setPosition(int x, int y);

    /* ---- 兼容旧接口：下落速度（映射到 velocity().y） ---- */
    int fallSpeed() const;
    void setFallSpeed(int fall_speed);

    /* ---- 兼容旧接口：命中状态（映射到 destroyReason） ---- */
    bool isHit() const;
    void setHit(bool is_hit);

    /* ---- 毫秒版 update 兼容入口（测试 / 旧调用路径使用） ---- */
    using TypingTargetEntity::update; // 暴露 update(qreal) 重载
    void update(int delta_ms);

    /* ---- IPoolable / 对象池接口 ---- */
    void reset() override;

protected:
    void onReset() override;
};

#endif // __TYPEGAME_APPLE_MODEL_H__
