#include "gtest/gtest.h"

#include "saveapple/model/apple_model.h"

TEST(AppleModelTest, DefaultStateAfterConstruct)
{
    AppleModel apple;

    EXPECT_EQ(apple.x(), 0);
    EXPECT_EQ(apple.y(), 0);
    EXPECT_FALSE(apple.isActive());
    EXPECT_FALSE(apple.isHit());
    EXPECT_EQ(apple.fallSpeed(), 120);
}

TEST(AppleModelTest, SetFallSpeedWithValidValue)
{
    AppleModel apple;

    apple.setFallSpeed(200);

    EXPECT_EQ(apple.fallSpeed(), 200);
}

TEST(AppleModelTest, SetFallSpeedWithInvalidValueShouldBeIgnored)
{
    AppleModel apple;

    apple.setFallSpeed(200);
    apple.setFallSpeed(0);
    apple.setFallSpeed(-10);

    EXPECT_EQ(apple.fallSpeed(), 200);
}

TEST(AppleModelTest, UpdateShouldMoveDownWhenActive)
{
    AppleModel apple;

    apple.setActive(true);
    apple.setFallSpeed(100);
    apple.update(1000);

    EXPECT_EQ(apple.y(), 100);
}

TEST(AppleModelTest, UpdateShouldNotMoveWhenInactive)
{
    AppleModel apple;

    apple.setActive(false);
    apple.setFallSpeed(100);
    apple.update(1000);

    EXPECT_EQ(apple.y(), 0);
}

TEST(AppleModelTest, UpdateShouldIgnoreInvalidDelta)
{
    AppleModel apple;

    apple.setActive(true);
    apple.setFallSpeed(100);

    apple.update(0);
    apple.update(-100);

    EXPECT_EQ(apple.y(), 0);
}

TEST(AppleModelTest, ResetShouldRestoreReusableState)
{
    AppleModel apple;

    apple.setActive(true);
    apple.setFallSpeed(300);
    apple.setHit(true);
    apple.update(1000);

    apple.reset();

    EXPECT_EQ(apple.x(), 0);
    EXPECT_EQ(apple.y(), 0);
    EXPECT_FALSE(apple.isActive());
    EXPECT_FALSE(apple.isHit());
    EXPECT_EQ(apple.fallSpeed(), 120);
}
