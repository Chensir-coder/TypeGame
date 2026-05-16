#ifndef __TYPEGAME_IENTITY_FACTORY_H__
#define __TYPEGAME_IENTITY_FACTORY_H__


/* ------------------------------------------------------------------
 // 文件名     : ientity_factory.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 通用实体工厂接口
------------------------------------------------------------------ */

/**
 * @brief 通用实体工厂接口。
 *
 * 职责：
 * 1. 为业务层提供统一的实体创建与回收入口。
 * 2. 隐藏对象池或具体创建逻辑的实现细节。
 *
 * 说明：
 * 1. T 通常为 AppleModel、EnemyModel、BulletModel 等实体类型。
 * 2. 当前返回裸指针，是因为对象生命周期由工厂/对象池统一管理。
 * 3. 业务层不应直接 delete 由工厂创建出的对象，而应通过 recycle() 归还。
 */
template <typename T>
class IEntityFactory
{
public:
    virtual ~IEntityFactory() = default;

    /**
     * @brief 创建或获取一个实体对象。
     *
     * @return 可用实体指针，失败时返回 nullptr
     */
    virtual T* create() = 0;

    /**
     * @brief 回收一个实体对象。
     *
     * @param entity 需要回收的实体指针
     */
    virtual void recycle(T* entity) = 0;
};

#endif // __TYPEGAME_IENTITY_FACTORY_H__
