/* ------------------------------------------------------------------
 // 文件名     : game_factory.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 应用层游戏模块工厂实现
------------------------------------------------------------------ */

#include "game_factory.h"

#include "common/core/igame.h"
#include "saveapple/model/saveapple_game.h"
#include "spacewar/spacewar_game.h"

std::unique_ptr<IGame> GameFactory::createGame(GameType game_type)
{
    switch (game_type)
    {
    case GameType::SaveApple:
        return std::make_unique<SaveAppleGame>();

    case GameType::SpaceWar:
        return std::make_unique<SpaceWarGame>();

    default:
        break;
    }

    return nullptr;
}
