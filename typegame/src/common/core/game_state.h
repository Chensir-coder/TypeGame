#ifndef __TYPEGAME_GAME_STATE_H__
#define __TYPEGAME_GAME_STATE_H__


/* ------------------------------------------------------------------
 // 文件名     : game_state.h
 // 创建者     : xxx@wps.cn
 // 创建时间   : 2026-04-13
 // 功能描述   : 通用游戏状态定义
------------------------------------------------------------------ */

/**
 * @brief 通用游戏状态枚举。
 *
 * 说明：
 * 1. 该状态定义面向整个项目统一使用。
 * 2. SaveApple 与 SpaceWar 应尽量共用这一套状态。
 * 3. 不同游戏如有细微差异，也应优先在业务层适配，而不是重复定义状态枚举。
 */
enum class GameState
{
    Initial = 0,   ///< 初始状态，尚未正式开始游戏
    Playing,       ///< 游戏进行中
    Paused,        ///< 游戏暂停中
    End            ///< 游戏结束
};

#endif // __TYPEGAME_GAME_STATE_H__
