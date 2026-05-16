#ifndef __TYPEGAME_SPACEWAR_PLAYFIELD_WIDGET_H__
#define __TYPEGAME_SPACEWAR_PLAYFIELD_WIDGET_H__

#include <QChar>
#include <QElapsedTimer>
#include <QPixmap>
#include <QPointF>
#include <QSizeF>
#include <QString>
#include <QVector>
#include <QWidget>

#include "common/view/effect/effectmanager.h"
#include "spacewar/model/spacewar_model_signal_bridge.h"

/* ------------------------------------------------------------------
 // 文件名     : spacewar_playfield_widget.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : spacewar_playfield_widget 头文件声明
------------------------------------------------------------------ */

class QPainter;
class QPaintEvent;
class QResizeEvent;
class SpaceWarModel;

class SpaceWarPlayfieldWidget final : public QWidget
{
public:
    explicit SpaceWarPlayfieldWidget(QWidget* parent = nullptr);

    void setGameModel(SpaceWarModel* model);
    SpaceWarModel* gameModel() const { return m_model; }

    /**
     * 推进动画时间轴并触发重绘，与游戏帧同步调用。
     * delta_ms == 0 时仅触发重绘，不推进时间轴。
     */
    void tick(int delta_ms);
    void onEnemyHitByBullet(QChar letter);
    void onRewardCompleted(const QPointF& center, const QSizeF& size, const QString& word);
    void onRewardExpired(const QPointF& center, const QSizeF& size, const QString& word);
    void onEnemyCollidedWithPlayer(const QPointF& enemyCenter,
                                   const QSizeF& enemySize,
                                   EnemyPlayerCollisionReason reason);
    void onDifficultyUpgraded(int level, int maxLevel);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void reloadAssets();
    void logPaintCost(qint64 paint_elapsed_ms);

    void advanceAnimationMs(int delta_ms);

    void rebuildBackgroundCache();
    void drawBackground(QPainter& painter);
    void advanceBackgroundScrollMs(int delta_ms);
    void resampleBackgroundScrollSpeed();
    void drawEnemies(QPainter& painter, qreal scale_x, qreal scale_y);
    void drawBullets(QPainter& painter, qreal scale_x, qreal scale_y);
    void drawReward(QPainter& painter, qreal scale_x, qreal scale_y);
    void drawExplosionEffects(QPainter& painter, qreal scale_x, qreal scale_y);
    void drawShip(QPainter& painter, qreal scale_x, qreal scale_y);
    void drawHud(QPainter& painter);
    void drawPausedOverlay(QPainter& painter);
    void drawGameOverOverlay(QPainter& painter);

private:
    SpaceWarModel* m_model = nullptr;

    QPixmap m_background;
    QPixmap m_backgroundTile;
    int m_backgroundTileCacheWidth = 0;

    qreal m_bgScrollPx = 0.0;
    qreal m_bgSpeedPps = 0.0;
    int m_bgSpeedResampleMsRemaining = 0;

    QPixmap m_enemy0_sheet;
    QPixmap m_enemy4_sheet;
    QVector<QPixmap> m_enemy0_frames;
    QVector<QPixmap> m_enemy4_frames;

    QPixmap m_ship_sheet;
    QVector<QPixmap> m_ship_frames;

    QPixmap m_bullet_pix;
    QPixmap m_reward_sheet;
    QVector<QPixmap> m_reward_frames;

    QPixmap m_explosion_sheet;
    QVector<QPixmap> m_explosion_frames;
    EffectManager m_effects;

    QElapsedTimer m_paint_perf_timer;
    qint64 m_paint_total_ms = 0;
    qint64 m_paint_max_ms = 0;
    int m_paint_frame_count = 0;
};

#endif // __TYPEGAME_SPACEWAR_PLAYFIELD_WIDGET_H__
