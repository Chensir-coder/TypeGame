#ifndef __TYPEGAME_SPACEWAR_MODEL_SIGNAL_BRIDGE_H__
#define __TYPEGAME_SPACEWAR_MODEL_SIGNAL_BRIDGE_H__

#include <QMetaType>
#include <QPointF>
#include <QObject>
#include <QSizeF>
#include <QString>

#include "common/core/game_event.h"
#include "common/core/igame_observer.h"

#include <QtGlobal>

/* ------------------------------------------------------------------
 // 文件名     : spacewar_model_signal_bridge.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 将业务模型 IGameObserver 事件转为 Qt 信号供视图 / 控制器使用
------------------------------------------------------------------ */

/** 敌机导致玩家扣血时的视图区分：几何重叠相撞 vs 敌机未击落滑出屏幕底边 */
enum class EnemyPlayerCollisionReason : quint8
{
    BoundingOverlap = 0,
    EnemyExitedBottom = 1
};
Q_DECLARE_METATYPE(EnemyPlayerCollisionReason)

class SpaceWarModelSignalBridge final : public QObject, public IGameObserver
{
    Q_OBJECT

public:
    explicit SpaceWarModelSignalBridge(QObject* parent = nullptr);

    void onGameEvent(const GameEvent& event) override;

    const GameEvent& lastEvent() const { return m_lastEvent; }

signals:
    void modelChanged();
    void hpChanged(int hp, int maxHp);
    void difficultyUpgraded(int level, int maxLevel);
    void scoreChanged(int score);
    void rewardProgress(QChar letter, int typedCount, int wordLength);
    void rewardSpawned();
    void rewardRemoved();
    void rewardCompleted(const QPointF& center, const QSizeF& size, const QString& word);
    void rewardExpired(const QPointF& center, const QSizeF& size, const QString& word);
    void enemyHit(QChar letter);
    void enemyLocked(QChar letter);
    void shotFired(QChar letter);
    void playerDamaged(int damage, int hpAfter);
    void playerHealed(int heal, int hpAfter);
    void enemyCollidedWithPlayer(const QPointF& enemyCenter,
                                 const QSizeF& enemySize,
                                 EnemyPlayerCollisionReason reason);
    void playerDied(int score);
    void dialogGameOverRequested(int missCount, int maxMissCount, int score);
    void dialogLevelCompletedRequested(int score);

private:
    GameEvent m_lastEvent;
};

#endif // __TYPEGAME_SPACEWAR_MODEL_SIGNAL_BRIDGE_H__
