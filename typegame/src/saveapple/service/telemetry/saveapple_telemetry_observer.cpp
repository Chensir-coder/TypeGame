/* ------------------------------------------------------------------
 // 文件名     : saveapple_telemetry_observer.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 拯救苹果游戏遥测观察者实现
------------------------------------------------------------------ */

/* ------------------------------------------------------------------
 // 文件名     : saveapple_telemetry_observer.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 拯救苹果游戏遥测观察者实现
------------------------------------------------------------------ */

#include "saveapple_telemetry_observer.h"

#include "common/core/game_event.h"
#include "common/service/telemetry/telemetry_message.h"

#include <QDateTime>
#include <QJsonObject>

SaveAppleTelemetryObserver::SaveAppleTelemetryObserver(ITelemetryWriter* writer)
    : m_writer(writer)
{
}

void SaveAppleTelemetryObserver::onGameEvent(const GameEvent& event)
{
    if (m_writer == nullptr)
    {
        return;
    }

    TelemetryMessage message;

    switch (event.type)
    {
        case GameEventType::GameInitialized:
        {
            message = buildMessage("game_initialized", event);
            break;
        }

        case GameEventType::GameStarted:
        {
            message = buildMessage("game_started", event);
            break;
        }

        case GameEventType::GamePaused:
        {
            message = buildMessage("game_paused", event);
            break;
        }

        case GameEventType::GameResumed:
        {
            message = buildMessage("game_resumed", event);
            break;
        }

        case GameEventType::GameStopped:
        {
            message = buildMessage("game_stopped", event);
            break;
        }

        case GameEventType::GameRestarted:
        {
            message = buildMessage("game_restarted", event);
            break;
        }

        case GameEventType::GameOver:
        {
            message = buildMessage("game_over", event);
            message.payload["miss_count"] = event.value;
            message.payload["max_miss_count"] = event.extra_value;
            break;
        }

        case GameEventType::LevelCompleted:
        {
            message = buildMessage("level_completed", event);
            message.payload["success_count"] = event.value;
            message.payload["target_success_count"] = event.extra_value;
            break;
        }

        case GameEventType::ScoreChanged:
        {
            message = buildMessage("score_changed", event);
            message.payload["score"] = event.value;
            break;
        }

        case GameEventType::LevelChanged:
        {
            message = buildMessage("level_changed", event);
            message.payload["level"] = event.value;
            break;
        }

        case GameEventType::EntitySpawned:
        {
            message = buildMessage("apple_spawned", event);
            message.payload["letter"] = QString(event.letter);
            message.payload["active_apple_count"] = event.value;
            break;
        }

        case GameEventType::EntityRemoved:
        {
            message = buildMessage("apple_removed", event);
            message.payload["letter"] = QString(event.letter);
            message.payload["active_apple_count"] = event.value;
            break;
        }

        case GameEventType::EntityHit:
        {
            message = buildMessage("apple_hit", event);
            message.payload["letter"] = QString(event.letter);
            message.payload["success_count"] = event.value;
            message.payload["miss_count"] = event.extra_value;
            break;
        }

        case GameEventType::EntityMissed:
        {
            message = buildMessage("apple_missed", event);
            message.payload["letter"] = QString(event.letter);
            message.payload["miss_count"] = event.value;
            message.payload["success_count"] = event.extra_value;
            break;
        }

        case GameEventType::InputAccepted:
        {
            message = buildMessage("input_accepted", event);
            message.payload["letter"] = QString(event.letter);
            break;
        }

        case GameEventType::InputRejected:
        {
            message = buildMessage("input_rejected", event);
            message.payload["letter"] = QString(event.letter);
            break;
        }

        case GameEventType::SettingsChanged:
        {
            switch (event.setting_key)
            {
                case SettingKey::SpeedLevel:
                    message = buildMessage("setting_changed_speed_level", event);
                    message.payload["value"] = event.value;
                    break;
                case SettingKey::MaxAppleCount:
                    message = buildMessage("setting_changed_max_apple_count", event);
                    message.payload["value"] = event.value;
                    break;
                case SettingKey::TargetSuccessCount:
                    message = buildMessage("setting_changed_target_success_count", event);
                    message.payload["value"] = event.value;
                    break;
                case SettingKey::MaxMissCount:
                    message = buildMessage("setting_changed_max_miss_count", event);
                    message.payload["value"] = event.value;
                    break;
                case SettingKey::SoundEnabled:
                    message = buildMessage("setting_changed_sound_enabled", event);
                    message.payload["value"] = (event.value != 0);
                    break;
                case SettingKey::FailLineY:
                    message = buildMessage("setting_changed_fail_line_y", event);
                    message.payload["value"] = event.value;
                    break;
                default:
                    return; // 未知设置项，不发送事件
            }
            break;
        }

        case GameEventType::GameStateChanged:
        {
            message = buildMessage("game_state_changed", event);
            break;
        }

        default:
        {
            return;
        }
    }

    m_writer->write(message);
}

TelemetryMessage SaveAppleTelemetryObserver::buildMessage(
    const QString& event_name,
    const GameEvent& event) const
{
    TelemetryMessage message;
    message.event_name = event_name;
    message.category = "saveapple";
    message.timestamp_ms = QDateTime::currentMSecsSinceEpoch();

    fillCommonFields(message.payload, event);
    return message;
}

void SaveAppleTelemetryObserver::fillCommonFields(
    QJsonObject& payload,
    const GameEvent& event) const
{
    payload["score"] = event.score;
    payload["level"] = event.level;
    payload["state"] = static_cast<int>(event.state);

    if (!event.letter.isNull())
    {
        payload["letter"] = QString(event.letter);
    }
}
