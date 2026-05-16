/* ------------------------------------------------------------------
 // 文件名     : apple_factory.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 拯救苹果游戏中的苹果实体工厂实现
------------------------------------------------------------------ */

#include "apple_factory.h"

#include "saveapple/model/apple_model.h"

AppleFactory::AppleFactory(int pool_capacity)
    : m_apple_pool(pool_capacity)
{
}

AppleModel* AppleFactory::create()
{
    AppleModel* apple = m_apple_pool.acquire();
    if (apple == nullptr)
    {
        return nullptr;
    }

    apple->setId(m_next_id++);
    apple->setFallSpeed(m_default_fall_speed);
    apple->setHit(false);
    apple->setActive(true);

    return apple;
}

void AppleFactory::recycle(AppleModel* entity)
{
    if (entity == nullptr)
    {
        return;
    }

    entity->setHit(false);
    entity->setActive(false);
    m_apple_pool.release(entity);
}

void AppleFactory::setDefaultFallSpeed(int fall_speed)
{
    if (fall_speed <= 0)
    {
        return;
    }

    m_default_fall_speed = fall_speed;
}

int AppleFactory::defaultFallSpeed() const
{
    return m_default_fall_speed;
}

int AppleFactory::capacity() const
{
    return m_apple_pool.capacity();
}

int AppleFactory::availableCount() const
{
    return m_apple_pool.availableCount();
}
