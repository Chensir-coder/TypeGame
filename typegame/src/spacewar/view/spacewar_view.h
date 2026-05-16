#ifndef __TYPEGAME_SPACEWAR_VIEW_H__
#define __TYPEGAME_SPACEWAR_VIEW_H__

#include <QChar>
#include <QPixmap>
#include <QPointF>
#include <QPointer>
#include <QSet>
#include <QSizeF>
#include <QString>
#include <QWidget>

#include "spacewar/model/spacewar_model_signal_bridge.h"

/* ------------------------------------------------------------------
 // 文件名     : spacewar_view.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 飞机大战主界面（背景 + 玩家 + 敌机 + 子弹 + 奖励 + HUD + 设置）
------------------------------------------------------------------ */

class QLabel;
class QHBoxLayout;
class QKeyEvent;

class SpaceWarModel;
class SpaceWarPlayfieldWidget;
class SpriteButton;
class SpaceWarFailDialog;

class SpaceWarView final : public QWidget
{
    Q_OBJECT

public:
    explicit SpaceWarView(QWidget* parent = nullptr);
    ~SpaceWarView() override;

    SpaceWarView(const SpaceWarView&) = delete;
    SpaceWarView& operator=(const SpaceWarView&) = delete;

    void setModel(SpaceWarModel* model);
    SpaceWarModel* model() const;

    /** @param delta_ms 与游戏帧同步的间隔；为 0 时仅触发重绘、不推进玩家精灵时间轴 */
    void tickSceneRefresh(int delta_ms = 16);

    void refreshHudFromModel();
    void refreshButtonStates();

public slots:
    void onModelSignalBridgeChanged();
    void onDialogGameOverRequested(int missCount, int maxMissCount, int score);
    void onDialogLevelCompletedRequested(int score);
    /** 敌机被子弹击落（模型发出 EntityHit / enemyHit 时仍可在 activeTargets 中取位姿） */
    void onEnemyHitByBullet(QChar letter);
    void onRewardCompleted(const QPointF& center, const QSizeF& size, const QString& word);
    void onRewardExpired(const QPointF& center, const QSizeF& size, const QString& word);
    void onEnemyCollidedWithPlayer(const QPointF& enemyCenter,
                                   const QSizeF& enemySize,
                                   EnemyPlayerCollisionReason reason);
    void onDifficultyUpgraded(int level, int maxLevel);

signals:
    void startClicked();
    void retryClicked();
    void pauseResumeClicked();
    void stopClicked();
    void settingsClicked();
    void exitClicked();
    void letterTyped(QChar letter);
    /**
     * 方向键状态变化（hAxis ∈ {-1, 0, 1}, vAxis ∈ {-1, 0, 1}）。
     */
    void moveKeyChanged(int hAxis, int vAxis);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;

private:
    void setupUi();
    void setupConnections();

    void emitMoveAxesIfChanged();

    void showFailDialog(int score);
    QString gameStateText() const;

private:
    SpaceWarModel* m_model = nullptr;

    SpaceWarPlayfieldWidget* m_playfield = nullptr;
    QWidget* m_bar_container = nullptr;
    QHBoxLayout* m_bar_layout = nullptr;

    QLabel* m_score_label = nullptr;
    QLabel* m_progress_label = nullptr;
    QLabel* m_state_label = nullptr;
    QLabel* m_difficulty_label = nullptr;
    QLabel* m_hp_label = nullptr;
    QLabel* m_accuracy_label = nullptr;

    SpriteButton* m_start_button = nullptr;
    SpriteButton* m_pause_resume_button = nullptr;
    SpriteButton* m_stop_button = nullptr;
    SpriteButton* m_settings_button = nullptr;
    SpriteButton* m_exit_button = nullptr;

    QPixmap m_start_pixmap;
    QPixmap m_pause_pixmap;
    QPixmap m_stop_pixmap;
    QPixmap m_settings_pixmap;
    QPixmap m_exit_pixmap;

    int m_lastEmittedHAxis = 0;
    int m_lastEmittedVAxis = 0;
    QSet<int> m_pressedKeys;
    QPointer<SpaceWarFailDialog> m_fail_dialog;
};

#endif // __TYPEGAME_SPACEWAR_VIEW_H__
