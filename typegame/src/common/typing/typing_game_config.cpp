#include "typing_game_config.h"

TypingGameConfig::TypingGameConfig()
{
    resetToDefault();
}

void TypingGameConfig::resetToDefault()
{
    m_speedLevel = 5;
    m_maxTargetCount = 3;
    m_targetSuccessCount = 5;
    m_maxMissCount = 3;

    m_soundEnabled = true;

    m_exitLineRatio = 0.7;

    m_spawnIntervalMs = 900;
    m_spawnY = -40;

    m_sceneWidth = 800;
    m_sceneHeight = 1000;

    m_typingTargetLogicalWidth = 120.0;
    m_typingTargetLogicalHeight = 120.0;

    m_playerCraftLogicalWidth = 200.0;
    m_playerCraftLogicalHeight = 260.0;
    m_playerCraftLogicalBottomMargin = 80.0;

    m_enemySpriteSheetCols = 1;
    m_enemySpriteSheetRows = 1;
    m_playerSpriteSheetCols = 1;
    m_playerSpriteSheetRows = 1;

    m_enemySpriteAnimationFps = 0.0;
    m_playerSpriteAnimationFps = 0.0;
}

int TypingGameConfig::speedLevel() const
{
    return m_speedLevel;
}

void TypingGameConfig::setSpeedLevel(int speedLevel)
{
    if (!isValidSpeedLevel(speedLevel)) {
        return;
    }

    m_speedLevel = speedLevel;
}

int TypingGameConfig::maxTargetCount() const
{
    return m_maxTargetCount;
}

void TypingGameConfig::setMaxTargetCount(int count)
{
    if (!isValidMaxTargetCount(count)) {
        return;
    }

    m_maxTargetCount = count;
}

int TypingGameConfig::targetSuccessCount() const
{
    return m_targetSuccessCount;
}

void TypingGameConfig::setTargetSuccessCount(int count)
{
    if (!isValidTargetSuccessCount(count)) {
        return;
    }

    m_targetSuccessCount = count;
}

int TypingGameConfig::maxMissCount() const
{
    return m_maxMissCount;
}

void TypingGameConfig::setMaxMissCount(int count)
{
    if (!isValidMaxMissCount(count)) {
        return;
    }

    m_maxMissCount = count;
}

bool TypingGameConfig::soundEnabled() const
{
    return m_soundEnabled;
}

void TypingGameConfig::setSoundEnabled(bool enabled)
{
    m_soundEnabled = enabled;
}

qreal TypingGameConfig::exitLineRatio() const
{
    return m_exitLineRatio;
}

void TypingGameConfig::setExitLineRatio(qreal ratio)
{
    if (ratio < 0.0) {
        m_exitLineRatio = 0.0;
    } else if (ratio > 1.0) {
        m_exitLineRatio = 1.0;
    } else {
        m_exitLineRatio = ratio;
    }
}

qreal TypingGameConfig::failLineRatio() const
{
    return exitLineRatio();
}

void TypingGameConfig::setFailLineRatio(qreal ratio)
{
    setExitLineRatio(ratio);
}

int TypingGameConfig::spawnIntervalMs() const
{
    return m_spawnIntervalMs;
}

void TypingGameConfig::setSpawnIntervalMs(int intervalMs)
{
    if (intervalMs <= 0) {
        return;
    }

    m_spawnIntervalMs = intervalMs;
}

int TypingGameConfig::spawnY() const
{
    return m_spawnY;
}

void TypingGameConfig::setSpawnY(int y)
{
    m_spawnY = y;
}

int TypingGameConfig::sceneWidth() const
{
    return m_sceneWidth;
}

void TypingGameConfig::setSceneWidth(int width)
{
    if (width <= 0) {
        return;
    }

    m_sceneWidth = width;
}

int TypingGameConfig::sceneHeight() const
{
    return m_sceneHeight;
}

void TypingGameConfig::setSceneHeight(int height)
{
    if (height <= 0) {
        return;
    }

    m_sceneHeight = height;
}

qreal TypingGameConfig::typingTargetLogicalWidth() const
{
    return m_typingTargetLogicalWidth;
}

qreal TypingGameConfig::typingTargetLogicalHeight() const
{
    return m_typingTargetLogicalHeight;
}

void TypingGameConfig::setTypingTargetLogicalWidth(qreal width)
{
    if (width <= 0.0) {
        return;
    }

    m_typingTargetLogicalWidth = width;
}

void TypingGameConfig::setTypingTargetLogicalHeight(qreal height)
{
    if (height <= 0.0) {
        return;
    }

    m_typingTargetLogicalHeight = height;
}

qreal TypingGameConfig::playerCraftLogicalWidth() const
{
    return m_playerCraftLogicalWidth;
}

qreal TypingGameConfig::playerCraftLogicalHeight() const
{
    return m_playerCraftLogicalHeight;
}

qreal TypingGameConfig::playerCraftLogicalBottomMargin() const
{
    return m_playerCraftLogicalBottomMargin;
}

void TypingGameConfig::setPlayerCraftLogicalWidth(qreal width)
{
    if (width <= 0.0) {
        return;
    }

    m_playerCraftLogicalWidth = width;
}

void TypingGameConfig::setPlayerCraftLogicalHeight(qreal height)
{
    if (height <= 0.0) {
        return;
    }

    m_playerCraftLogicalHeight = height;
}

void TypingGameConfig::setPlayerCraftLogicalBottomMargin(qreal margin)
{
    if (margin < 0.0) {
        return;
    }

    m_playerCraftLogicalBottomMargin = margin;
}

int TypingGameConfig::enemySpriteSheetCols() const
{
    return m_enemySpriteSheetCols;
}

int TypingGameConfig::enemySpriteSheetRows() const
{
    return m_enemySpriteSheetRows;
}

void TypingGameConfig::setEnemySpriteSheetCols(int cols)
{
    if (cols <= 0) {
        return;
    }

    m_enemySpriteSheetCols = cols;
}

void TypingGameConfig::setEnemySpriteSheetRows(int rows)
{
    if (rows <= 0) {
        return;
    }

    m_enemySpriteSheetRows = rows;
}

int TypingGameConfig::playerSpriteSheetCols() const
{
    return m_playerSpriteSheetCols;
}

int TypingGameConfig::playerSpriteSheetRows() const
{
    return m_playerSpriteSheetRows;
}

void TypingGameConfig::setPlayerSpriteSheetCols(int cols)
{
    if (cols <= 0) {
        return;
    }

    m_playerSpriteSheetCols = cols;
}

void TypingGameConfig::setPlayerSpriteSheetRows(int rows)
{
    if (rows <= 0) {
        return;
    }

    m_playerSpriteSheetRows = rows;
}

qreal TypingGameConfig::enemySpriteAnimationFps() const
{
    return m_enemySpriteAnimationFps;
}

qreal TypingGameConfig::playerSpriteAnimationFps() const
{
    return m_playerSpriteAnimationFps;
}

void TypingGameConfig::setEnemySpriteAnimationFps(qreal fps)
{
    if (fps < 0.0) {
        return;
    }

    m_enemySpriteAnimationFps = fps;
}

void TypingGameConfig::setPlayerSpriteAnimationFps(qreal fps)
{
    if (fps < 0.0) {
        return;
    }

    m_playerSpriteAnimationFps = fps;
}

int TypingGameConfig::currentMoveSpeed() const
{
    return 80 + (m_speedLevel - 1) * 20;
}

bool TypingGameConfig::isValidSpeedLevel(int speedLevel) const
{
    return speedLevel >= 1 && speedLevel <= 10;
}

bool TypingGameConfig::isValidMaxTargetCount(int count) const
{
    return count >= 1 && count <= 10;
}

bool TypingGameConfig::isValidTargetSuccessCount(int count) const
{
    return count > 0;
}

bool TypingGameConfig::isValidMaxMissCount(int count) const
{
    return count > 0;
}
