/* ------------------------------------------------------------------
 // 文件名     : game_window.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 游戏窗口实现
------------------------------------------------------------------ */

#include "game_window.h"
#include "common/core/igame.h"
#include <QDebug>

GameWindow::GameWindow(std::unique_ptr<IGame> game, QWidget* parent)
    : QMainWindow(parent)
    , m_game(std::move(game))
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    resize(1280, 720);

    if (m_game)
    {
        setCentralWidget(m_game->gameWidget());
        m_game->initialize();
    }
}

GameWindow::~GameWindow()
{
    qDebug() << "GameWindow destroyed";
}

