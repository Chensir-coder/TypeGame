#include "gtest/gtest.h"

#include "saveapple/model/saveapple_model.h"
#include "saveapple/model/apple_model.h"
#include "common/core/game_state.h"

namespace
{
AppleModel CreateApple(QChar letter, int y = 0)
{
    AppleModel apple;
    apple.setLetter(letter);
    apple.setPosition(100, y);
    apple.setActive(true);
    return apple;
}
}

TEST(SaveAppleModelTest, InitialStateAfterConstruct)
{
    SaveAppleModel model;

    EXPECT_EQ(model.gameState(), GameState::Initial);
    EXPECT_EQ(model.speedLevel(), 5);
    EXPECT_EQ(model.maxAppleCount(), 3);
    EXPECT_EQ(model.targetSuccessCount(), 5);
    EXPECT_EQ(model.maxMissCount(), 3);
    EXPECT_TRUE(model.soundEnabled());
    EXPECT_EQ(model.failLineY(), 700);
    EXPECT_EQ(model.score(), 0);
    EXPECT_EQ(model.successCount(), 0);
    EXPECT_EQ(model.missCount(), 0);
    EXPECT_EQ(model.accuracy(), 0);
    EXPECT_TRUE(model.activeApples().isEmpty());
}

TEST(SaveAppleModelTest, StartPauseResumeStopGame)
{
    SaveAppleModel model;

    model.startGame();
    EXPECT_EQ(model.gameState(), GameState::Playing);

    model.pauseGame();
    EXPECT_EQ(model.gameState(), GameState::Paused);

    model.resumeGame();
    EXPECT_EQ(model.gameState(), GameState::Playing);

    model.stopGame();
    EXPECT_EQ(model.gameState(), GameState::End);
}

TEST(SaveAppleModelTest, InvalidSettingsShouldBeIgnored)
{
    SaveAppleModel model;

    model.setSpeedLevel(8);
    model.setMaxAppleCount(5);
    model.setTargetSuccessCount(10);
    model.setMaxMissCount(6);
    model.setFailLineY(720);

    model.setSpeedLevel(0);
    model.setSpeedLevel(11);
    model.setMaxAppleCount(0);
    model.setMaxAppleCount(6);
    model.setTargetSuccessCount(0);
    model.setMaxMissCount(0);
    model.setFailLineY(0);

    EXPECT_EQ(model.speedLevel(), 8);
    EXPECT_EQ(model.maxAppleCount(), 5);
    EXPECT_EQ(model.targetSuccessCount(), 10);
    EXPECT_EQ(model.maxMissCount(), 6);
    EXPECT_EQ(model.failLineY(), 720);
}

TEST(SaveAppleModelTest, CurrentAppleFallSpeedShouldMatchSpeedLevel)
{
    SaveAppleModel model;

    model.setSpeedLevel(1);
    EXPECT_EQ(model.currentAppleFallSpeed(), 80);

    model.setSpeedLevel(5);
    EXPECT_EQ(model.currentAppleFallSpeed(), 160);

    model.setSpeedLevel(10);
    EXPECT_EQ(model.currentAppleFallSpeed(), 260);
}

TEST(SaveAppleModelTest, RegisterAppleShouldAddActiveAppleAndOccupyLetter)
{
    SaveAppleModel model;
    model.startGame();

    AppleModel apple = CreateApple('A');

    const bool registered = model.registerApple(&apple);

    EXPECT_TRUE(registered);
    EXPECT_EQ(model.activeApples().size(), 1);
    EXPECT_FALSE(model.isLetterAvailable('A'));
    EXPECT_FALSE(model.isLetterAvailable('a'));
    EXPECT_TRUE(apple.isActive());
}

TEST(SaveAppleModelTest, RegisterAppleShouldRejectDuplicateLetter)
{
    SaveAppleModel model;
    model.startGame();

    AppleModel apple_a = CreateApple('A');
    AppleModel apple_b = CreateApple('a');

    EXPECT_TRUE(model.registerApple(&apple_a));
    EXPECT_FALSE(model.registerApple(&apple_b));
    EXPECT_EQ(model.activeApples().size(), 1);
}

TEST(SaveAppleModelTest, RegisterAppleShouldRespectMaxAppleCount)
{
    SaveAppleModel model;
    model.setMaxAppleCount(1);
    model.startGame();

    AppleModel apple_a = CreateApple('A');
    AppleModel apple_b = CreateApple('B');

    EXPECT_TRUE(model.registerApple(&apple_a));
    EXPECT_FALSE(model.registerApple(&apple_b));
    EXPECT_EQ(model.activeApples().size(), 1);
}

TEST(SaveAppleModelTest, UnregisterAppleShouldRemoveAppleAndReleaseLetter)
{
    SaveAppleModel model;
    model.startGame();

    AppleModel apple = CreateApple('A');

    ASSERT_TRUE(model.registerApple(&apple));

    EXPECT_TRUE(model.unregisterApple(&apple));
    EXPECT_TRUE(model.activeApples().isEmpty());
    EXPECT_TRUE(model.isLetterAvailable('A'));
    EXPECT_FALSE(apple.isActive());
}

TEST(SaveAppleModelTest, HitAppleByLetterShouldIncreaseSuccessAndScore)
{
    SaveAppleModel model;
    model.setSpeedLevel(5);
    model.startGame();

    AppleModel apple = CreateApple('A');

    ASSERT_TRUE(model.registerApple(&apple));

    EXPECT_TRUE(model.hitAppleByLetter('a'));

    EXPECT_EQ(model.successCount(), 1);
    EXPECT_EQ(model.missCount(), 0);
    EXPECT_EQ(model.score(), 18);
    EXPECT_EQ(model.accuracy(), 100);
    EXPECT_FALSE(apple.isActive());
    EXPECT_TRUE(model.activeApples().isEmpty());
}

TEST(SaveAppleModelTest, HitAppleByWrongLetterShouldBeRejected)
{
    SaveAppleModel model;
    model.startGame();

    AppleModel apple = CreateApple('A');

    ASSERT_TRUE(model.registerApple(&apple));

    EXPECT_FALSE(model.hitAppleByLetter('B'));

    EXPECT_EQ(model.successCount(), 0);
    EXPECT_EQ(model.missCount(), 0);
    EXPECT_EQ(model.score(), 0);
    EXPECT_TRUE(apple.isActive());
    EXPECT_EQ(model.activeApples().size(), 1);
}

TEST(SaveAppleModelTest, HitAppleShouldOnlyWorkWhenPlaying)
{
    SaveAppleModel model;

    AppleModel apple = CreateApple('A');

    ASSERT_TRUE(model.registerApple(&apple));

    EXPECT_FALSE(model.hitAppleByLetter('A'));
    EXPECT_EQ(model.successCount(), 0);
    EXPECT_TRUE(apple.isActive());
}

TEST(SaveAppleModelTest, UpdateShouldMissAppleWhenReachingFailLine)
{
    SaveAppleModel model;
    model.setFailLineY(100);
    model.setMaxMissCount(3);
    model.startGame();

    AppleModel apple = CreateApple('A', 99);
    apple.setFallSpeed(100);

    ASSERT_TRUE(model.registerApple(&apple));

    model.update(10);

    EXPECT_EQ(model.successCount(), 0);
    EXPECT_EQ(model.missCount(), 1);
    EXPECT_EQ(model.score(), 0);
    EXPECT_EQ(model.accuracy(), 0);
    EXPECT_FALSE(apple.isActive());
    EXPECT_TRUE(model.activeApples().isEmpty());
}

TEST(SaveAppleModelTest, MissCountReachLimitShouldEndGame)
{
    SaveAppleModel model;
    model.setFailLineY(100);
    model.setMaxMissCount(1);
    model.startGame();

    AppleModel apple = CreateApple('A', 100);

    ASSERT_TRUE(model.registerApple(&apple));

    model.update(1);

    EXPECT_EQ(model.missCount(), 1);
    EXPECT_EQ(model.gameState(), GameState::End);
}

TEST(SaveAppleModelTest, SuccessReachTargetShouldCompleteLevelAndEndGame)
{
    SaveAppleModel model;
    model.setTargetSuccessCount(2);
    model.startGame();

    AppleModel apple_a = CreateApple('A');
    AppleModel apple_b = CreateApple('B');

    ASSERT_TRUE(model.registerApple(&apple_a));
    ASSERT_TRUE(model.hitAppleByLetter('A'));

    ASSERT_TRUE(model.registerApple(&apple_b));
    ASSERT_TRUE(model.hitAppleByLetter('B'));

    model.update(1);

    EXPECT_TRUE(model.isLevelCompleted());
    EXPECT_EQ(model.successCount(), 2);
    EXPECT_EQ(model.gameState(), GameState::End);
}

TEST(SaveAppleModelTest, ResetModelShouldKeepSettingsButClearRuntimeData)
{
    SaveAppleModel model;

    model.setSpeedLevel(8);
    model.setMaxAppleCount(5);
    model.setTargetSuccessCount(12);
    model.setMaxMissCount(6);
    model.setSoundEnabled(false);
    model.setFailLineY(720);

    model.startGame();

    AppleModel apple = CreateApple('A');
    ASSERT_TRUE(model.registerApple(&apple));
    ASSERT_TRUE(model.hitAppleByLetter('A'));

    model.resetModel();

    EXPECT_EQ(model.gameState(), GameState::Initial);
    EXPECT_EQ(model.speedLevel(), 8);
    EXPECT_EQ(model.maxAppleCount(), 5);
    EXPECT_EQ(model.targetSuccessCount(), 12);
    EXPECT_EQ(model.maxMissCount(), 6);
    EXPECT_FALSE(model.soundEnabled());
    EXPECT_EQ(model.failLineY(), 720);

    EXPECT_EQ(model.score(), 0);
    EXPECT_EQ(model.successCount(), 0);
    EXPECT_EQ(model.missCount(), 0);
    EXPECT_TRUE(model.activeApples().isEmpty());
}
