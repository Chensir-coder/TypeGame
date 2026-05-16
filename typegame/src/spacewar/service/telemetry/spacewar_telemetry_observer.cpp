/* ------------------------------------------------------------------
 // 文件名     : spacewar_telemetry_observer.cpp
------------------------------------------------------------------ */

#include "spacewar_telemetry_observer.h"

#include <QDateTime>
#include <QJsonObject>

#include "common/core/game_event.h"
#include "common/service/telemetry/telemetry_message.h"

SpaceWarTelemetryObserver::SpaceWarTelemetryObserver(ITelemetryWriter* writer)
    : m_writer(writer)
{
}

void SpaceWarTelemetryObserver::onGameEvent(const GameEvent& event)
{
    if (m_writer == nullptr) {
        return;
    }

    TelemetryMessage message;

    switch (event.type) {
    case GameEventType::GameInitialized:
        message = buildMessage("game_initialized", event);
        break;

    case GameEventType::GameStarted:
        message = buildMessage("game_started", event);
        break;

    case GameEventType::GamePaused:
        message = buildMessage("game_paused", event);
        break;

    case GameEventType::GameResumed:
        message = buildMessage("game_resumed", event);
        break;

    case GameEventType::GameStopped:
        message = buildMessage("game_stopped", event);
        break;

    case GameEventType::GameRestarted:
        message = buildMessage("game_restarted", event);
        break;

    case GameEventType::GameOver:
        message = buildMessage("game_over", event);
        message.payload["hp"] = event.hp;
        break;

    case GameEventType::ScoreChanged:
        message = buildMessage("score_changed", event);
        message.payload["score"] = event.score;
        break;

    case GameEventType::HpChanged:
        message = buildMessage("hp_changed", event);
        message.payload["hp"] = event.hp;
        message.payload["max_hp"] = event.extra_value;
        break;

    case GameEventType::LevelChanged:
        message = buildMessage("difficulty_upgraded", event);
        message.payload["level"] = event.value;
        message.payload["max_level"] = event.extra_value;
        break;

    case GameEventType::EntitySpawned:
        if (event.extra_value == 1) {
            message = buildMessage("bullet_fired", event);
            message.payload["target_letter"] = QString(event.letter);
            message.payload["active_bullets"] = event.value;
        } else if (event.extra_value == -1) {
            message = buildMessage("reward_spawned", event);
            message.payload["word_length"] = event.value;
        } else {
            message = buildMessage("enemy_spawned", event);
            message.payload["letter"] = QString(event.letter);
            message.payload["active_enemies"] = event.value;
        }
        break;

    case GameEventType::EntityRemoved:
        if (event.value == -1) {
            message = buildMessage("reward_removed", event);
        } else {
            message = buildMessage("enemy_removed", event);
            message.payload["letter"] = QString(event.letter);
            message.payload["active_enemies"] = event.value;
        }
        break;

    case GameEventType::EntityHit:
        if (event.value == -1) {
            message = buildMessage("reward_completed", event);
        } else if (event.letter.isNull()) {
            message = buildMessage("player_healed", event);
            message.payload["heal"] = event.value;
            message.payload["hp"] = event.hp;
        } else {
            message = buildMessage("enemy_hit", event);
            message.payload["letter"] = QString(event.letter);
        }
        break;

    case GameEventType::EntityMissed:
        message = buildMessage("player_damaged", event);
        message.payload["damage"] = event.value;
        message.payload["hp_after"] = event.extra_value;
        break;

    case GameEventType::InputAccepted:
        if (event.extra_value > 0 && !event.letter.isNull()) {
            message = buildMessage("reward_input_accepted", event);
            message.payload["letter"] = QString(event.letter);
            message.payload["typed"] = event.value;
            message.payload["length"] = event.extra_value;
        } else if (!event.letter.isNull()) {
            message = buildMessage("enemy_locked", event);
            message.payload["letter"] = QString(event.letter);
        } else {
            return;
        }
        break;

    case GameEventType::InputRejected:
        message = buildMessage("input_rejected", event);
        message.payload["letter"] = QString(event.letter);
        break;

    case GameEventType::SettingsChanged:
        message = buildMessage("settings_changed", event);
        message.payload["key"] = static_cast<int>(event.setting_key);
        message.payload["value"] = event.value;
        message.payload["extra"] = event.extra_value;
        break;

    case GameEventType::GameStateChanged:
        message = buildMessage("game_state_changed", event);
        break;

    default:
        return;
    }

    m_writer->write(message);
}

TelemetryMessage SpaceWarTelemetryObserver::buildMessage(const QString& event_name,
                                                        const GameEvent& event) const
{
    TelemetryMessage message;
    message.event_name = event_name;
    message.category = "spacewar";
    message.timestamp_ms = QDateTime::currentMSecsSinceEpoch();
    fillCommonFields(message.payload, event);
    return message;
}

void SpaceWarTelemetryObserver::fillCommonFields(QJsonObject& payload,
                                                 const GameEvent& event) const
{
    payload["score"] = event.score;
    payload["level"] = event.level;
    payload["state"] = static_cast<int>(event.state);
    payload["hp"] = event.hp;

    if (!event.letter.isNull()) {
        payload["letter"] = QString(event.letter);
    }
}
