#ifndef __TYPEGAME_IOBJECT_POOL_H__
#define __TYPEGAME_IOBJECT_POOL_H__


/* ------------------------------------------------------------------
 // 文件名     : iobject_pool.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 通用对象池接口
------------------------------------------------------------------ */

/**
 * @brief 通用对象池接口。
 *
 * 职责：
 * 1. 定义对象池统一的借出与归还行为。
 * 2. 使工厂层可以依赖抽象对象池，而不是具体池实现。
 *
 * 说明：
 * 1. T 通常应实现 IPoolable 接口。
 * 2. 当前采用裸指针接口，便于与现有项目风格和对象池管理方式对齐。
 */
template <typename T>
class IObjectPool
{
public:
    virtual ~IObjectPool() = default;

    /**
     * @brief 获取一个可用对象。
     *
     * @return 可用对象指针，失败时返回 nullptr
     */
    virtual T* acquire() = 0;

    /**
     * @brief 归还一个对象到池中。
     *
     * @param object 对象指针
     */
    virtual void release(T* object) = 0;

    /**
     * @brief 获取池容量。
     *
     * @return 当前池可管理对象总数
     */
    virtual int capacity() const = 0;

    /**
     * @brief 获取当前空闲对象数量。
     *
     * @return 空闲对象数量
     */
    virtual int availableCount() const = 0;
};

#endif // __TYPEGAME_IOBJECT_POOL_H__
