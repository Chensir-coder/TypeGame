#ifndef __TYPEGAME_IGAME_H__
#define __TYPEGAME_IGAME_H__

#include <QWidget>
#include "game_state.h"

/* ------------------------------------------------------------------
 // 文件名     : igame.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 通用游戏模块接口
------------------------------------------------------------------ */

/**
 * @brief 通用游戏模块接口。
 *
 * 职责：
 * 1. 定义一个“游戏模块”对外必须提供的统一能力。
 * 2. 便于 app 层通过统一入口管理不同游戏，而不依赖具体实现细节。
 * 3. 便于后续扩展更多游戏模块时保持一致的对外接口。
 *
 * 边界：
 * 1. IGame 只定义游戏模块的公共能力，不定义具体业务细节。
 * 2. 具体的苹果/飞机规则应由各自模块内部实现。
 */
class IGame
{
public:
    virtual ~IGame() = default;

    /**
     * @brief 初始化游戏模块。
     *
     * 用途：
     * 1. 创建和连接模块内部组件。
     * 2. 完成必要的初始化准备。
     */
    virtual void initialize() = 0;

    /**
     * @brief 开始游戏。
     */
    virtual void startGame() = 0;

    /**
     * @brief 暂停游戏。
     */
    virtual void pauseGame() = 0;

    /**
     * @brief 恢复游戏。
     */
    virtual void resumeGame() = 0;

    /**
     * @brief 停止游戏。
     */
    virtual void stopGame() = 0;

    /**
     * @brief 重新开始游戏。
     */
    virtual void restartGame() = 0;

    /**
     * @brief 执行一次游戏更新。
     *
     * @param delta_ms 距离上一次更新的毫秒数
     */
    virtual void update(int delta_ms) = 0;

    /**
     * @brief 获取当前游戏状态。
     *
     * @return 当前状态
     */
    virtual GameState gameState() const = 0;

    /**
     * @brief 获取当前游戏页面控件。
     *
     * 说明：
     * 1. app 层可通过该接口将具体游戏页面挂载到主窗口中。
     * 2. 这里返回 QWidget*，是为了方便与 Qt UI 集成。
     *
     * @return 游戏主页面控件指针，所有权由具体模块管理
     */
    virtual QWidget* gameWidget() = 0;

    virtual void exitGame() = 0;
};

#endif // __TYPEGAME_IGAME_H__
