#ifndef __TYPEGAME_APPLE_FACTORY_H__
#define __TYPEGAME_APPLE_FACTORY_H__

#include "common/factory/ientity_factory.h"
#include "common/pool/object_pool.h"
#include "saveapple/model/apple_model.h"

/* ------------------------------------------------------------------
 // 文件名     : apple_factory.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 拯救苹果游戏中的苹果实体工厂
------------------------------------------------------------------ */

/**
 * @brief 拯救苹果游戏中的苹果实体工厂。
 *
 * 职责：
 * 1. 作为苹果实体创建与回收的统一入口。
 * 2. 内部接入对象池，复用 AppleModel 对象。
 * 3. 隐藏苹果对象池的具体实现细节。
 *
 * 说明：
 * 1. AppleFactory 不负责整局游戏规则。
 * 2. AppleFactory 不负责随机字母、随机位置等业务策略。
 * 3. AppleFactory 主要负责提供“一个可用的苹果对象”。
 */
class AppleFactory final : public IEntityFactory<AppleModel>
{
public:
    /**
     * @brief 构造苹果工厂。
     *
     * @param pool_capacity 苹果对象池容量
     */
    explicit AppleFactory(int pool_capacity = 8);

    ~AppleFactory() override = default;

    AppleFactory(const AppleFactory&) = delete;
    AppleFactory& operator=(const AppleFactory&) = delete;

    /**
     * @brief 创建或获取一个可用苹果对象。
     *
     * @return 苹果对象指针，失败时返回 nullptr
     */
    AppleModel* create() override;

    /**
     * @brief 回收一个苹果对象。
     *
     * @param entity 苹果对象指针
     */
    void recycle(AppleModel* entity) override;

    /**
     * @brief 设置工厂创建苹果时的默认下落速度。
     *
     * @param fall_speed 默认下落速度
     */
    void setDefaultFallSpeed(int fall_speed);

    /**
     * @brief 获取当前默认下落速度。
     *
     * @return 默认下落速度
     */
    int defaultFallSpeed() const;

    /**
     * @brief 获取对象池容量。
     *
     * @return 池容量
     */
    int capacity() const;

    /**
     * @brief 获取当前空闲苹果数量。
     *
     * @return 空闲数量
     */
    int availableCount() const;

private:
    ObjectPool<AppleModel> m_apple_pool; ///< 苹果对象池
    int m_default_fall_speed = 120;      ///< 默认下落速度（像素/秒）
    int m_next_id = 1;                   ///< 自增苹果编号
};

#endif // __TYPEGAME_APPLE_FACTORY_H__
