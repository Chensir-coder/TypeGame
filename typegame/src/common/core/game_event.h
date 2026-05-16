#ifndef __TYPEGAME_GAME_EVENT_H__
#define __TYPEGAME_GAME_EVENT_H__

#include <QChar>
#include "game_state.h"

/* ------------------------------------------------------------------
 // 文件名     : game_event.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 通用游戏事件定义
------------------------------------------------------------------ */

/**
 * @brief 通用游戏事件类型。
 *
 * 说明：
 * 1. 事件类型用于观察者模式中的事件分发。
 * 2. 当前仅保留项目中较高频、较通用的事件类型。
 * 3. 后续如果需要新增事件，应优先扩展该枚举，而不是为每个模块重新定义事件系统。
 */

// 事件类型定义
enum class GameEventType
{
    Unknown = 0,       ///< 未知事件

    GameInitialized,   ///< 游戏初始化完成
    GameStarted,       ///< 游戏开始
    GamePaused,        ///< 游戏暂停
    GameResumed,       ///< 游戏恢复
    GameStopped,       ///< 游戏停止
    GameRestarted,     ///< 游戏重开
    GameStateChanged,  ///< 游戏状态变化

    ScoreChanged,      ///< 分数变化
    LevelChanged,      ///< 等级变化
    LevelCompleted,    ///< 关卡完成
    HpChanged,         ///< 生命值变化

    EntitySpawned,     ///< 实体生成
    EntityRemoved,     ///< 实体移除
    EntityHit,         ///< 实体被命中
    EntityMissed,      ///< 实体漏掉/失败

    InputAccepted,     ///< 输入被接受
    InputRejected,      ///< 输入被拒绝

    SettingsChanged,   ///< 设置变化    
    GameOver,           ///< 游戏结束

};

/**
 * @brief 通用游戏事件数据。
 *
 * 说明：
 * 1. 该结构用于在模块间传递统一事件信息。
 * 2. 先保持轻量，避免一开始设计过重。
 * 3. value / extra_value 可用于传递分数、等级、生命值等简单整数信息。
 * 4. letter 字段可用于传递字母命中、输入字符等事件信息。
 */

 // 设置项键枚举，配合 SettingsChanged 事件使用
enum class SettingKey
{
    Unknown = 0,
    SpeedLevel,
    MaxAppleCount,
    TargetSuccessCount,
    MaxMissCount,
    SoundEnabled,
    FailLineY
};

// 事件数据结构
struct GameEvent
{
    GameEventType type = GameEventType::Unknown;
    GameState state = GameState::Initial;

    int value = 0;
    int extra_value = 0;
    QChar letter;

    int score = 0;
    int level = 0;
    int hp = 0;

    SettingKey setting_key = SettingKey::Unknown; ///< 具体设置项变化的键，配合 SettingsChanged 事件使用
};

#endif // __TYPEGAME_GAME_EVENT_H__
