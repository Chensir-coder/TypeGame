#ifndef __TYPEGAME_TYPING_GAME_CONFIG_H__
#define __TYPEGAME_TYPING_GAME_CONFIG_H__

#include <QtGlobal>

/* ------------------------------------------------------------------
 // 文件名     : typing_game_config.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : typing_game_config 头文件声明
------------------------------------------------------------------ */

class TypingGameConfig
{
public:
    TypingGameConfig();

    void resetToDefault();

    int speedLevel() const;
    void setSpeedLevel(int speedLevel);

    int maxTargetCount() const;
    void setMaxTargetCount(int count);

    int targetSuccessCount() const;
    void setTargetSuccessCount(int count);

    int maxMissCount() const;
    void setMaxMissCount(int count);

    bool soundEnabled() const;
    void setSoundEnabled(bool enabled);

    /*
     * 新语义：退场线比例。
     *
     * 苹果：0.7，苹果到这里就算漏掉。
     * 飞机：1.0，敌机到底部才自然退场。
     */
    qreal exitLineRatio() const;
    void setExitLineRatio(qreal ratio);

    /*
     * 兼容旧接口。
     */
    qreal failLineRatio() const;
    void setFailLineRatio(qreal ratio);

    int spawnIntervalMs() const;
    void setSpawnIntervalMs(int intervalMs);

    int spawnY() const;
    void setSpawnY(int y);

    int sceneWidth() const;
    void setSceneWidth(int width);

    int sceneHeight() const;
    void setSceneHeight(int height);

    /*
     * Typing 目标在逻辑场景坐标系中的占位尺寸（与 sceneWidth/Height 同源）。
     * 视图仅用窗口 / 逻辑场景比例换算像素绘制，不改变这些值。
     */
    qreal typingTargetLogicalWidth() const;
    qreal typingTargetLogicalHeight() const;
    void setTypingTargetLogicalWidth(qreal width);
    void setTypingTargetLogicalHeight(qreal height);

    /*
     * 玩家机体在逻辑场景中的尺寸与底边留白（仅供飞机类视图按比例绘制）。
     */
    qreal playerCraftLogicalWidth() const;
    qreal playerCraftLogicalHeight() const;
    qreal playerCraftLogicalBottomMargin() const;
    void setPlayerCraftLogicalWidth(qreal width);
    void setPlayerCraftLogicalHeight(qreal height);
    void setPlayerCraftLogicalBottomMargin(qreal margin);

    /*
     * 精灵表切分（列×行），与 PixmapSliceHelper::slice 一致；均为 1 时表示整张图为单帧。
     */
    int enemySpriteSheetCols() const;
    int enemySpriteSheetRows() const;
    void setEnemySpriteSheetCols(int cols);
    void setEnemySpriteSheetRows(int rows);

    int playerSpriteSheetCols() const;
    int playerSpriteSheetRows() const;
    void setPlayerSpriteSheetCols(int cols);
    void setPlayerSpriteSheetRows(int rows);

    /*
     * 精灵循环播放速度（帧/秒），0 表示不自动切帧。
     */
    qreal enemySpriteAnimationFps() const;
    qreal playerSpriteAnimationFps() const;
    void setEnemySpriteAnimationFps(qreal fps);
    void setPlayerSpriteAnimationFps(qreal fps);

    int currentMoveSpeed() const;

    bool isValidSpeedLevel(int speedLevel) const;
    bool isValidMaxTargetCount(int count) const;
    bool isValidTargetSuccessCount(int count) const;
    bool isValidMaxMissCount(int count) const;

private:
    int m_speedLevel = 5;
    int m_maxTargetCount = 3;
    int m_targetSuccessCount = 5;
    int m_maxMissCount = 3;

    bool m_soundEnabled = true;

    qreal m_exitLineRatio = 0.7;

    int m_spawnIntervalMs = 200;
    int m_spawnY = -40;

    int m_sceneWidth = 800;
    int m_sceneHeight = 1000;

    qreal m_typingTargetLogicalWidth = 120.0;
    qreal m_typingTargetLogicalHeight = 120.0;

    qreal m_playerCraftLogicalWidth = 200.0;
    qreal m_playerCraftLogicalHeight = 260.0;
    qreal m_playerCraftLogicalBottomMargin = 80.0;

    int m_enemySpriteSheetCols = 1;
    int m_enemySpriteSheetRows = 1;
    int m_playerSpriteSheetCols = 1;
    int m_playerSpriteSheetRows = 1;

    qreal m_enemySpriteAnimationFps = 0.0;
    qreal m_playerSpriteAnimationFps = 0.0;
};

#endif // __TYPEGAME_TYPING_GAME_CONFIG_H__
