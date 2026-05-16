/* ------------------------------------------------------------------
 // 文件名     : saveapple_logger_observer.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 拯救苹果游戏日志观察者实现
------------------------------------------------------------------ */

/* ------------------------------------------------------------------
 // 文件名     : saveapple_logger_observer.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 拯救苹果游戏日志观察者实现
------------------------------------------------------------------ */

#include "saveapple_logger_observer.h"
#include "common/core/game_event.h"
#include "common/service/log/log_message.h"
#include <QDateTime>

SaveAppleLoggerObserver::SaveAppleLoggerObserver(ILogger* logger)
    : m_logger(logger)
{
}


void SaveAppleLoggerObserver::onGameEvent(const GameEvent& event)
{
    if (m_logger == nullptr)
    {
        return;
    }

    LogMessage message;
    message.timestamp_ms = QDateTime::currentMSecsSinceEpoch();
    message.category = "saveapple";

    switch (event.type)
    {
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

        case GameEventType::GameOver:
            message.level = LogLevel::Warning;
            message.text = QString("Game over: score=%1 miss=%2")
                               .arg(event.score)
                               .arg(event.value);
            break;

        case GameEventType::LevelCompleted:
            message.level = LogLevel::Info;
            message.text = QString("Level completed: score=%1 success=%2")
                               .arg(event.score)
                               .arg(event.value);
            break;

        case GameEventType::EntitySpawned:
            message.level = LogLevel::Debug;
            message.text = QString("Apple spawned: letter=%1")
                               .arg(event.letter);
            break;

        case GameEventType::EntityHit:
            message.level = LogLevel::Info;
            message.text = QString("Apple hit: letter=%1 score=%2")
                               .arg(event.letter)
                               .arg(event.score);
            break;

        case GameEventType::EntityMissed:
            message.level = LogLevel::Warning;
            message.text = QString("Apple missed: letter=%1 miss=%2")
                               .arg(event.letter)
                               .arg(event.value);
            break;

        case GameEventType::InputAccepted:
            message.level = LogLevel::Debug;
            message.text = QString("Input accepted: letter=%1")
                               .arg(event.letter);
            break;

        case GameEventType::InputRejected:
            message.level = LogLevel::Debug;
            message.text = QString("Input rejected: letter=%1")
                               .arg(event.letter);
            break;

        case GameEventType::SettingsChanged:
            switch (event.setting_key)
            {
                case SettingKey::SpeedLevel:
                    message.text = QString("Settings changed: key=SpeedLevel value=%1")
                                       .arg(event.value);
                    break;
                case SettingKey::MaxAppleCount:
                    message.text = QString("Settings changed: key=MaxAppleCount value=%1")
                                       .arg(event.value);
                    break;
                case SettingKey::TargetSuccessCount:
                    message.text = QString("Settings changed: key=TargetSuccessCount value=%1")
                                       .arg(event.value);
                    break;
                case SettingKey::MaxMissCount:
                    message.text = QString("Settings changed: key=MaxMissCount value=%1")
                                       .arg(event.value);
                    break;
                case SettingKey::SoundEnabled:
                    message.text = QString("Settings changed: key=SoundEnabled value=%1")
                                       .arg(event.value);
                    break;
                case SettingKey::FailLineY:
                    message.text = QString("Settings changed: key=FailLineY value=%1")
                                       .arg(event.value);
                    break;
                default:
                    message.text = QString("Settings changed: key=%1 value=%2")
                                       .arg(static_cast<int>(event.setting_key))
                                       .arg(event.value);
                    break;
            }
            break;

        default:
            return;
    }

    m_logger->log(message);
}
