#ifndef __TYPEGAME_MAIN_WINDOW_H__
#define __TYPEGAME_MAIN_WINDOW_H__

#include <QMainWindow>
#include <QPointer>
#include <QVector>

/* ------------------------------------------------------------------
 // 文件名     : main_window.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 主窗口
------------------------------------------------------------------ */

class QWidget;
class GameWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSaveAppleSelected();
    void onSpaceWarSelected();

private:
    void setupUi();

private:
    QWidget* m_home_widget = nullptr;
    QVector<QPointer<GameWindow>> m_game_windows;
};

#endif // __TYPEGAME_MAIN_WINDOW_H__
