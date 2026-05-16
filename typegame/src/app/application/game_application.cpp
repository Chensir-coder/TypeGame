/* ------------------------------------------------------------------
 // 文件名     : game_application.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 应用启动协调器实现
------------------------------------------------------------------ */

#include "game_application.h"

#include "app/mainwindow/main_window.h"

GameApplication::GameApplication() = default;

GameApplication::~GameApplication() = default;

void GameApplication::initialize()
{
    m_main_window = std::make_unique<MainWindow>();
}

int GameApplication::run()
{
    if (!m_main_window)
    {
        initialize();
    }

    if (!m_main_window)
    {
        return -1;
    }

    m_main_window->show();
    return 0;
}
