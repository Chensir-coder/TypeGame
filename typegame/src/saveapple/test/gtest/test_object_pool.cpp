#include "gtest/gtest.h"

#include "common/pool/object_pool.h"
#include "saveapple/model/apple_model.h"

TEST(ObjectPoolTest, ConstructWithPositiveCapacity)
{
    ObjectPool<AppleModel> pool(3);

    EXPECT_EQ(pool.capacity(), 3);
    EXPECT_EQ(pool.availableCount(), 3);
}

TEST(ObjectPoolTest, ConstructWithInvalidCapacity)
{
    ObjectPool<AppleModel> pool(0);

    EXPECT_EQ(pool.capacity(), 0);
    EXPECT_EQ(pool.availableCount(), 0);
}

TEST(ObjectPoolTest, AcquireShouldReturnAvailableObject)
{
    ObjectPool<AppleModel> pool(1);

    AppleModel* apple = pool.acquire();

    ASSERT_NE(apple, nullptr);
    EXPECT_TRUE(apple->isActive());
    EXPECT_EQ(pool.availableCount(), 0);
}

TEST(ObjectPoolTest, AcquireShouldReturnNullWhenPoolIsFull)
{
    ObjectPool<AppleModel> pool(1);

    AppleModel* first = pool.acquire();
    AppleModel* second = pool.acquire();

    EXPECT_NE(first, nullptr);
    EXPECT_EQ(second, nullptr);
}

TEST(ObjectPoolTest, ReleaseShouldMakeObjectAvailableAgain)
{
    ObjectPool<AppleModel> pool(1);

    AppleModel* first = pool.acquire();
    ASSERT_NE(first, nullptr);

    pool.release(first);

    EXPECT_FALSE(first->isActive());
    EXPECT_EQ(pool.availableCount(), 1);

    AppleModel* second = pool.acquire();

    EXPECT_EQ(first, second);
}

TEST(ObjectPoolTest, ReleaseNullShouldBeSafe)
{
    ObjectPool<AppleModel> pool(1);

    pool.release(nullptr);

    EXPECT_EQ(pool.capacity(), 1);
    EXPECT_EQ(pool.availableCount(), 1);
}

TEST(ObjectPoolTest, ReleaseShouldResetObjectState)
{
    ObjectPool<AppleModel> pool(1);

    AppleModel* apple = pool.acquire();
    ASSERT_NE(apple, nullptr);

    apple->setFallSpeed(300);
    apple->setHit(true);
    apple->update(1000);

    pool.release(apple);

    EXPECT_FALSE(apple->isActive());
    EXPECT_FALSE(apple->isHit());
    EXPECT_EQ(apple->fallSpeed(), 120);
    EXPECT_EQ(apple->y(), 0);
}
