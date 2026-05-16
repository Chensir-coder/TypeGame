#include <gtest/gtest.h>

#include <QSizeF>

#include "spacewar/model/entity/enemy_entity.h"

TEST(EnemyEntityTest, DefaultNotLocked)
{
    EnemyEntity enemy;

    EXPECT_FALSE(enemy.isLocked());
}

TEST(EnemyEntityTest, LockToggle)
{
    EnemyEntity enemy;

    enemy.setLocked(true);
    EXPECT_TRUE(enemy.isLocked());

    enemy.setLocked(false);
    EXPECT_FALSE(enemy.isLocked());
}

TEST(EnemyEntityTest, StraightTrajectoryMovesDownAtVerticalSpeed)
{
    EnemyEntity enemy;
    enemy.setActive(true);
    enemy.setAlive(true);
    enemy.setSize(QSizeF(60.0, 60.0));
    enemy.setSceneRect(QRectF(0.0, 0.0, 800.0, 1000.0));
    enemy.setPosition(QPointF(120.0, 80.0));
    enemy.configureStraight(120.0, 150.0);

    enemy.update(1.0);

    EXPECT_NEAR(enemy.position().x(), 120.0, 0.001);
    EXPECT_NEAR(enemy.position().y(), 80.0 + 150.0, 0.001);
}

TEST(EnemyEntityTest, StraightKeepsStartXWhileFalling)
{
    EnemyEntity enemy;
    enemy.setActive(true);
    enemy.setAlive(true);
    enemy.setSize(QSizeF(60.0, 60.0));
    enemy.setSceneRect(QRectF(0.0, 0.0, 800.0, 1000.0));
    enemy.setPosition(QPointF(500.0, 0.0));
    enemy.setVelocity(QPointF(999.0, 50.0));
    enemy.configureStraight(120.0, 40.0);

    enemy.update(0.5);

    EXPECT_NEAR(enemy.position().x(), 120.0, 0.001);
}
