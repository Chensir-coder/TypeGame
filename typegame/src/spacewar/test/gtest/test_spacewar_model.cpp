#include <gtest/gtest.h>

#include "common/core/game_state.h"
#include "spacewar/model/entity/enemy_entity.h"
#include "spacewar/model/spacewar_model.h"

namespace
{

void FillEnemyForRegister(EnemyEntity& enemy, SpaceWarModel& model, QChar letter, qreal x, qreal y)
{
    const auto& c = model.config();
    enemy.setLetter(letter);
    enemy.setScoreValue(10);
    enemy.setSize(QSizeF(qMax<qreal>(40.0, c.typingTargetLogicalWidth()),
                         qMax<qreal>(40.0, c.typingTargetLogicalHeight())));
    enemy.setLocked(false);
    enemy.setSceneRect(QRectF(0.0, 0.0, static_cast<qreal>(c.sceneWidth()),
                              static_cast<qreal>(c.sceneHeight())));
    enemy.setPosition(QPointF(x, y));
    enemy.configureStraight(x, 0.0);

    const int frame_count = qMax(1, c.enemySpriteSheetCols() * c.enemySpriteSheetRows());
    enemy.configureSpriteAnimation(frame_count, c.enemySpriteAnimationFps());
}

} // namespace

TEST(SpaceWarModelTest, InitialStateAfterInitialize)
{
    SpaceWarModel model;
    model.initialize();

    EXPECT_EQ(model.gameState(), GameState::Initial);
    EXPECT_EQ(model.maxEnemyCount(), 3);
    EXPECT_EQ(model.enemySpeedLevel(), 1);
    EXPECT_TRUE(model.rewardModeEnabled());
    EXPECT_DOUBLE_EQ(model.config().exitLineRatio(), 1.0);
    EXPECT_EQ(model.config().maxMissCount(), 99999);
    EXPECT_EQ(model.config().targetSuccessCount(), 99999);
    EXPECT_EQ(model.stats().score(), 0);
    EXPECT_EQ(model.stats().successCount(), 0);
    EXPECT_EQ(model.stats().missCount(), 0);
    EXPECT_TRUE(model.activeTargets().isEmpty());
}

TEST(SpaceWarModelTest, StartPauseResumeStopGame)
{
    SpaceWarModel model;
    model.initialize();

    model.startGame();
    EXPECT_EQ(model.gameState(), GameState::Playing);

    model.pauseGame();
    EXPECT_EQ(model.gameState(), GameState::Paused);

    model.resumeGame();
    EXPECT_EQ(model.gameState(), GameState::Playing);

    model.stopGame();
    EXPECT_EQ(model.gameState(), GameState::End);
}

TEST(SpaceWarModelTest, InvalidSettingsShouldBeIgnored)
{
    SpaceWarModel model;
    model.initialize();

    model.setEnemySpeedLevel(5);
    model.setMaxEnemyCount(4);
    model.setDifficultyUpgradeIntervalSec(12);

    model.setEnemySpeedLevel(0);
    model.setEnemySpeedLevel(11);
    model.setMaxEnemyCount(0);
    model.setMaxEnemyCount(11);
    model.setDifficultyUpgradeIntervalSec(0);

    EXPECT_EQ(model.enemySpeedLevel(), 5);
    EXPECT_EQ(model.maxEnemyCount(), 4);
    EXPECT_EQ(model.difficultyUpgradeIntervalSec(), 12);
}

TEST(SpaceWarModelTest, SuppressAutoSpawnIncreasesSpawnInterval)
{
    SpaceWarModel model;
    model.initialize();

    const int normal_interval = model.currentEnemySpawnIntervalMs();
    model.setSuppressAutoEnemySpawn(true);
    const int suppressed_interval = model.currentEnemySpawnIntervalMs();

    EXPECT_GT(suppressed_interval, normal_interval);
    EXPECT_GT(suppressed_interval, 1'000'000);
}

TEST(SpaceWarModelTest, RegisterTargetShouldOccupyLetter)
{
    SpaceWarModel model;
    model.initialize();
    model.startGame();

    EnemyEntity enemy;
    FillEnemyForRegister(enemy, model, QLatin1Char('H'), 100.0, 120.0);

    const bool ok = model.registerTarget(&enemy);

    EXPECT_TRUE(ok);
    EXPECT_EQ(model.activeTargets().size(), 1);
    EXPECT_FALSE(model.isLetterAvailable(QLatin1Char('H')));
    EXPECT_FALSE(model.isLetterAvailable(QLatin1Char('h')));

    model.stopGame();
}

TEST(SpaceWarModelTest, RegisterTargetShouldRejectDuplicateLetter)
{
    SpaceWarModel model;
    model.initialize();
    model.startGame();

    EnemyEntity first;
    EnemyEntity second;
    FillEnemyForRegister(first, model, QLatin1Char('M'), 80.0, 100.0);
    FillEnemyForRegister(second, model, QLatin1Char('m'), 200.0, 100.0);

    EXPECT_TRUE(model.registerTarget(&first));
    EXPECT_FALSE(model.registerTarget(&second));
    EXPECT_EQ(model.activeTargets().size(), 1);

    model.stopGame();
}

TEST(SpaceWarModelTest, TryLockEnemyByLetterRequiresPlayingState)
{
    SpaceWarModel model;
    model.initialize();

    EnemyEntity enemy;
    FillEnemyForRegister(enemy, model, QLatin1Char('P'), 90.0, 110.0);

    ASSERT_EQ(model.gameState(), GameState::Initial);
    EXPECT_EQ(model.tryLockEnemyByLetter(QLatin1Char('P')), nullptr);

    model.startGame();
    ASSERT_TRUE(model.registerTarget(&enemy));

    EnemyEntity* locked = model.tryLockEnemyByLetter(QLatin1Char('p'));
    ASSERT_NE(locked, nullptr);
    EXPECT_TRUE(locked->isLocked());

    model.stopGame();
}

TEST(SpaceWarModelTest, DestroyEnemyByBulletRecordsSuccess)
{
    SpaceWarModel model;
    model.initialize();
    model.startGame();

    EnemyEntity enemy;
    FillEnemyForRegister(enemy, model, QLatin1Char('T'), 150.0, 200.0);
    ASSERT_TRUE(model.registerTarget(&enemy));

    ASSERT_EQ(model.stats().successCount(), 0);
    model.destroyEnemyByBullet(&enemy);

    EXPECT_EQ(model.stats().successCount(), 1);
    EXPECT_GT(model.stats().score(), 0);
    EXPECT_TRUE(model.activeTargets().isEmpty());

    model.stopGame();
}
