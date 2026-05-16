#ifndef __TYPEGAME_GAME_APPLICATION_H__
#define __TYPEGAME_GAME_APPLICATION_H__

#include <memory>

/* ------------------------------------------------------------------
 // 文件名     : game_application.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 应用启动协调器
------------------------------------------------------------------ */

class MainWindow;

/**
 * @brief 应用启动协调器。
 *
 * 职责：
 * 1. 负责应用层初始化流程。
 * 2. 负责创建并管理主窗口。
 * 3. 为 main.cpp 提供清晰的应用启动入口。
 *
 * 边界：
 * 1. 不负责具体游戏业务逻辑。
 * 2. 不负责具体页面内部绘制与规则调度。
 */
class GameApplication final
{
public:
    GameApplication();
    ~GameApplication();

    GameApplication(const GameApplication&) = delete;
    GameApplication& operator=(const GameApplication&) = delete;

    /**
     * @brief 初始化应用。
     *
     * 说明：
     * 1. 创建主窗口及应用层必要对象。
     */
    void initialize();

    /**
     * @brief 运行应用。
     *
     * @return 返回状态码，0 表示成功
     */
    int run();

private:
    std::unique_ptr<MainWindow> m_main_window; ///< 应用主窗口
};

#endif // __TYPEGAME_GAME_APPLICATION_H__
