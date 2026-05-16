#ifndef __TYPEGAME_GAME_FACTORY_H__
#define __TYPEGAME_GAME_FACTORY_H__

#include <memory>

#include "common/core/game_types.h"
#include "common/core/igame.h"

/* ------------------------------------------------------------------
 // 文件名     : game_factory.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 应用层游戏模块工厂
------------------------------------------------------------------ */

/**
 * @brief 应用层游戏模块工厂。
 *
 * 职责：
 * 1. 根据游戏类型创建对应的具体游戏模块。
 * 2. 向 app 层统一返回 IGame 抽象接口。
 * 3. 隐藏具体游戏模块的创建细节。
 *
 * 说明：
 * 1. 该工厂属于 app 层，负责创建“整个游戏模块”。
 * 2. 该工厂不负责创建苹果、敌机等业务实体对象。
 */
class GameFactory
{
public:
    GameFactory() = delete;
    ~GameFactory() = delete;

    /**
     * @brief 创建指定类型的游戏模块。
     *
     * @param game_type 游戏类型
     * @return 游戏模块指针，若当前类型尚未实现则返回 nullptr
     */
    static std::unique_ptr<IGame> createGame(GameType game_type);
};

#endif // __TYPEGAME_GAME_FACTORY_H__
