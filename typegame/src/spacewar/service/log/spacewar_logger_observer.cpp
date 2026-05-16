/* ------------------------------------------------------------------
 // 文件名     : spacewar_logger_observer.cpp
------------------------------------------------------------------ */

#include "spacewar_logger_observer.h"

#include <QDateTime>

#include "common/core/game_event.h"
#include "common/service/log/log_message.h"

SpaceWarLoggerObserver::SpaceWarLoggerObserver(ILogger* logger)
    : m_logger(logger)
{
}

void SpaceWarLoggerObserver::onGameEvent(const GameEvent& event)
{
    if (m_logger == nullptr) {
        return;
    }

    LogMessage message;
    message.timestamp_ms = QDateTime::currentMSecsSinceEpoch();
    message.category = "spacewar";

    switch (event.type) {
    case GameEventType::GameInitialized:
        message.level = LogLevel::Info;
        message.text = "Game initialized";
        break;

    case GameEventType::GameStarted:
        message.level = LogLevel::Info;
        message.text = "Game started";
        break;

    case GameEventType::GamePaused:
        message.level = LogLevel::Info;
        message.text = "Game paused";
        break;

    case GameEventType::GameResumed:
        message.level = LogLevel::Info;
        message.text = "Game resumed";
        break;

    case GameEventType::GameStopped:
        message.level = LogLevel::Info;
        message.text = "Game stopped";
        break;

    case GameEventType::GameRestarted:
        message.level = LogLevel::Info;
        message.text = "Game restarted";
        break;

    case GameEventType::GameOver:
        message.level = LogLevel::Warning;
        message.text = QString("Game over: score=%1 hp=%2")
                           .arg(event.score)
                           .arg(event.hp);
        break;

    case GameEventType::EntitySpawned:
        if (event.extra_value == 1) {
            message.level = LogLevel::Debug;
            message.text = QString("Bullet fired: target_letter=%1 active_bullets=%2")
                               .arg(event.letter)
                               .arg(event.value);
        } else if (event.extra_value == -1) {
            message.level = LogLevel::Info;
            message.text = QString("Reward word spawned: length=%1").arg(event.value);
        } else {
            message.level = LogLevel::Debug;
            message.text = QString("Enemy spawned: letter=%1 active=%2")
                               .arg(event.letter)
                               .arg(event.value);
        }
        break;

    case GameEventType::EntityRemoved:
        if (event.value == -1) {
            message.level = LogLevel::Info;
            message.text = QString("Reward word removed (expired)");
        } else {
            message.level = LogLevel::Debug;
            message.text = QString("Enemy removed: letter=%1 active=%2")
                               .arg(event.letter)
                               .arg(event.value);
        }
        break;

    case GameEventType::EntityHit:
        if (event.value == -1) {
            message.level = LogLevel::Info;
            message.text = QString("Reward word completed: hp=%1").arg(event.hp);
        } else if (event.letter.isNull()) {
            message.level = LogLevel::Info;
            message.text = QString("Player healed: amount=%1 hp=%2")
                               .arg(event.value)
                               .arg(event.hp);
        } else {
            message.level = LogLevel::Info;
            message.text = QString("Enemy hit: letter=%1 score=%2")
                               .arg(event.letter)
                               .arg(event.score);
        }
        break;

    case GameEventType::EntityMissed:
        message.level = LogLevel::Warning;
        message.text = QString("Player damaged: damage=%1 hp_after=%2")
                           .arg(event.value)
                           .arg(event.extra_value);
        break;

    case GameEventType::HpChanged:
        message.level = LogLevel::Debug;
        message.text = QString("HP changed: hp=%1 max=%2").arg(event.hp).arg(event.extra_value);
        break;

    case GameEventType::LevelChanged:
        message.level = LogLevel::Info;
        message.text = QString("Difficulty upgraded: level=%1 max=%2")
                           .arg(event.value)
                           .arg(event.extra_value);
        break;

    case GameEventType::InputAccepted:
        if (event.extra_value > 0 && !event.letter.isNull()) {
            message.level = LogLevel::Debug;
            message.text = QString("Reward input accepted: letter=%1 typed=%2/%3")
                               .arg(event.letter)
                               .arg(event.value)
                               .arg(event.extra_value);
        } else if (!event.letter.isNull()) {
            message.level = LogLevel::Debug;
            message.text = QString("Enemy locked: letter=%1").arg(event.letter);
        } else {
            return;
        }
        break;

    case GameEventType::InputRejected:
        message.level = LogLevel::Debug;
        message.text = QString("Input rejected: letter=%1").arg(event.letter);
        break;

    case GameEventType::SettingsChanged:
        message.level = LogLevel::Info;
        message.text = QString("Settings changed: key=%1 value=%2 extra=%3")
                           .arg(static_cast<int>(event.setting_key))
                           .arg(event.value)
                           .arg(event.extra_value);
        break;

    default:
        return;
    }

    m_logger->log(message);
}
