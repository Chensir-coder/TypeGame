#ifndef __TYPEGAME_GAME_WINDOW_H__
#define __TYPEGAME_GAME_WINDOW_H__

#include <QMainWindow>
#include <memory>

/* ------------------------------------------------------------------
 // 文件名     : game_window.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 游戏窗口
------------------------------------------------------------------ */

class IGame;

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(std::unique_ptr<IGame> game, QWidget* parent = nullptr);
    ~GameWindow() override;

private:
    std::unique_ptr<IGame> m_game;
};

#endif // __TYPEGAME_GAME_WINDOW_H__
