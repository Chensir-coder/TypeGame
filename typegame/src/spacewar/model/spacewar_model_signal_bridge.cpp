/* ------------------------------------------------------------------
 // 文件名     : spacewar_model_signal_bridge.cpp
------------------------------------------------------------------ */

#include "spacewar_model_signal_bridge.h"

#include <QMetaType>

SpaceWarModelSignalBridge::SpaceWarModelSignalBridge(QObject* parent)
    : QObject(parent)
{
    qRegisterMetaType<EnemyPlayerCollisionReason>("EnemyPlayerCollisionReason");
}

void SpaceWarModelSignalBridge::onGameEvent(const GameEvent& event)
{
    m_lastEvent = event;

    emit modelChanged();

    switch (event.type) {
    case GameEventType::HpChanged:
        emit hpChanged(event.hp, event.extra_value);
        if (event.hp <= 0) {
            emit playerDied(event.score);
        }
        break;

    case GameEventType::ScoreChanged:
        emit scoreChanged(event.score);
        break;

    case GameEventType::LevelChanged:
        emit difficultyUpgraded(event.value, event.extra_value);
        break;

    case GameEventType::EntityHit:
        if (event.value == -1) {
            /* spacewar 内部约定：value == -1 表示奖励单词完成 */
            emit rewardRemoved();
        } else if (event.letter.isNull()) {
            /* 玩家回血（NotifyPlayerHealed 不带 letter） */
            emit playerHealed(event.value, event.hp);
        } else {
            emit enemyHit(event.letter);
        }
        break;

    case GameEventType::EntityMissed:
        /* spacewar 中专门表示玩家被撞掉血 */
        emit playerDamaged(event.value, event.extra_value);
        break;

    case GameEventType::EntitySpawned:
        if (event.extra_value == -1) {
            emit rewardSpawned();
        } else if (event.extra_value == 1) {
            emit shotFired(event.letter);
        }
        break;

    case GameEventType::EntityRemoved:
        if (event.value == -1) {
            emit rewardRemoved();
        }
        break;

    case GameEventType::InputAccepted:
        if (event.extra_value > 0 && !event.letter.isNull()) {
            emit rewardProgress(event.letter, event.value, event.extra_value);
        } else {
            emit enemyLocked(event.letter);
        }
        break;

    case GameEventType::GameOver:
        emit dialogGameOverRequested(event.value, event.extra_value, event.score);
        break;

    case GameEventType::LevelCompleted:
        emit dialogLevelCompletedRequested(event.score);
        break;

    default:
        break;
    }
}
