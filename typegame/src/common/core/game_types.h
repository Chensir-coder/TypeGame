#ifndef __TYPEGAME_GAME_TYPES_H__
#define __TYPEGAME_GAME_TYPES_H__


/* ------------------------------------------------------------------
 // 文件名     : game_types.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 通用游戏类型定义
------------------------------------------------------------------ */

/**
 * @brief 通用游戏类型枚举。
 *
 * 说明：
 * 1. 用于应用层选择和创建具体游戏模块。
 * 2. 当前项目包含 SaveApple 和 SpaceWar 两种游戏。
 */
enum class GameType
{
    SaveApple = 0, ///< 拯救苹果
    SpaceWar       ///< 太空大战
};

#endif // __TYPEGAME_GAME_TYPES_H__
