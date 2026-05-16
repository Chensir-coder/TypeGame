#ifndef __TYPEGAME_OBJECT_POOL_H__
#define __TYPEGAME_OBJECT_POOL_H__

#include <vector>
#include <memory>
#include <algorithm>
#include "iobject_pool.h"

/* ------------------------------------------------------------------
 // 文件名     : object_pool.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 通用对象池模板实现
------------------------------------------------------------------ */

/**
 * @brief 通用对象池模板实现。
 *
 * 职责：
 * 1. 预分配固定数量对象并统一管理。
 * 2. 提供 acquire()/release() 供工厂层复用对象。
 * 3. 降低高频创建/销毁对象带来的分配开销。
 *
 * 说明：
 * 1. T 应至少具备：
 *    - reset()
 *    - setActive(bool)
 *    - isActive() const
 * 2. 当前版本为轻量级单线程实现，不处理并发访问。
 * 3. 当前策略为：
 *    - 优先从空闲对象中复用
 *    - 若对象全部占用，则返回 nullptr
 * 4. 对象池拥有对象生命周期，外部仅借用对象指针。
 */
template <typename T>
class ObjectPool final : public IObjectPool<T>
{
public:
    /**
     * @brief 构造对象池。
     *
     * @param capacity 预分配对象数量
     */
    explicit ObjectPool(int capacity);

    ~ObjectPool() override = default;

    T* acquire() override;
    void release(T* object) override;

    int capacity() const override;
    int availableCount() const override;

private:
    std::vector<std::unique_ptr<T>> m_objects; ///< 对象存储区
};

template <typename T>
ObjectPool<T>::ObjectPool(int capacity)
{
    if (capacity <= 0)
    {
        return;
    }

    m_objects.reserve(static_cast<std::size_t>(capacity));
    for (int index = 0; index < capacity; ++index)
    {
        std::unique_ptr<T> object = std::make_unique<T>();
        object->reset();
        object->setActive(false);
        m_objects.push_back(std::move(object));
    }
}

template <typename T>
T* ObjectPool<T>::acquire()
{
    for (const std::unique_ptr<T>& object : m_objects)
    {
        if (object != nullptr && !object->isActive())
        {
            object->reset();
            object->setActive(true);
            return object.get();
        }
    }

    return nullptr;
}

template <typename T>
void ObjectPool<T>::release(T* object)
{
    if (object == nullptr)
    {
        return;
    }

    const auto iterator = std::find_if(
        m_objects.begin(),
        m_objects.end(),
        [object](const std::unique_ptr<T>& current_object)
        {
            return current_object.get() == object;
        });

    if (iterator == m_objects.end())
    {
        return;
    }

    object->reset();
    object->setActive(false);
}

template <typename T>
int ObjectPool<T>::capacity() const
{
    return static_cast<int>(m_objects.size());
}

template <typename T>
int ObjectPool<T>::availableCount() const
{
    int count = 0;
    for (const std::unique_ptr<T>& object : m_objects)
    {
        if (object != nullptr && !object->isActive())
        {
            ++count;
        }
    }

    return count;
}

#endif // __TYPEGAME_OBJECT_POOL_H__
