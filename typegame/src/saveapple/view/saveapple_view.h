#ifndef __TYPEGAME_SAVEAPPLE_VIEW_H__
#define __TYPEGAME_SAVEAPPLE_VIEW_H__

#include <QPixmap>
#include <QRect>
#include <QWidget>

#include "common/core/igame_observer.h"
#include "common/view/sprite_button.h"

/* ------------------------------------------------------------------
 // 文件名     : saveapple_view.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 拯救苹果游戏主页面
------------------------------------------------------------------ */

class QLabel;
class QWidget;
class QHBoxLayout;
class SpriteButton;
class SaveAppleModel;
class SaveAppleController;
class QPaintEvent;
class QKeyEvent;
class QResizeEvent;
class QPainter;
class QButton;

struct GameEvent;


class SaveAppleView final : public QWidget, public IGameObserver
{
    Q_OBJECT

public:
    explicit SaveAppleView(QWidget* parent = nullptr);
    ~SaveAppleView() override = default;

    SaveAppleView(const SaveAppleView&) = delete;
    SaveAppleView& operator=(const SaveAppleView&) = delete;

    void setModel(SaveAppleModel* model);
    SaveAppleModel* model() const;

    void setController(SaveAppleController* controller);
    SaveAppleController* controller() const;

    void setAppleRadius(int apple_radius);
    int appleRadius() const;

    void setFailLineVisible(bool visible);
    bool isFailLineVisible() const;

    void refreshFromModel();
    void refreshButtonStates();
    void onGameEvent(const GameEvent& event) override;

signals:

    void startClicked();
    void pauseResumeClicked();
    void stopClicked();
    void settingsClicked();
    void exitClicked();

 

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void setupUi();
    void setupConnections();
    void updateSceneRect();
    void updateButtonGeometries();
    void updateOverlayGeometries();
    void showPassDialog();
    void showFailDialog();


    QString gameStateText() const;

    void drawScene(QPainter& painter);
    void drawSceneBackground(QPainter& painter);
    void drawFailLine(QPainter& painter);
    void drawBasket(QPainter& painter);
    void drawSavedApples(QPainter& painter);
    void drawApples(QPainter& painter);
    void drawAppleLetter(QPainter& painter, const QRect& apple_rect, const QChar& letter);

private:
    SaveAppleModel* m_model = nullptr;
    SaveAppleController* m_controller = nullptr;

    QWidget* m_status_container = nullptr;
    QHBoxLayout* m_status_layout = nullptr;

    QLabel* m_score_label = nullptr;
    QLabel* m_progress_label = nullptr;
    QLabel* m_state_label = nullptr;
    QLabel* m_difficulty_label = nullptr;
    QLabel* m_max_miss_label = nullptr;

    SpriteButton* m_start_button = nullptr;
    SpriteButton* m_pause_resume_button = nullptr;
    SpriteButton* m_stop_button = nullptr;
    SpriteButton* m_settings_button = nullptr;
    SpriteButton* m_exit_button = nullptr;


    QRect m_scene_rect;
    QRect m_basket_rect;
    QRect m_saved_apples_rect;

    int m_apple_radius = 26;
    bool m_fail_line_visible = true;

    QPixmap m_background_pixmap;
    QPixmap m_apple_pixmap;
    QPixmap m_small_apple_pixmap;
    QPixmap m_basket_pixmap;

    QPixmap m_start_button_pixmap;
    QPixmap m_pause_button_pixmap;
    QPixmap m_stop_button_pixmap;
    QPixmap m_settings_button_pixmap;
    QPixmap m_exit_button_pixmap;
    
};

#endif // __TYPEGAME_SAVEAPPLE_VIEW_H__
