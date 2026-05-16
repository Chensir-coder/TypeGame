/* ------------------------------------------------------------------
 // 文件名     : saveapple_game.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 拯救苹果游戏模块总入口实现
------------------------------------------------------------------ */

#include "saveapple_game.h"

#include <QTimer>
#include <QString>
#include <QDateTime>
#include <QDebug>

#include "common/service/log/async_logger.h"
#include "common/service/log/log_formatter.h"
#include "common/service/telemetry/async_telemetry_writer.h"
#include "common/service/audio/async_qt_audio_service.h"
#include "saveapple/service/audio/saveapple_audio_observer.h"

#include "saveapple/service/telemetry/saveapple_telemetry_observer.h"

#include "saveapple_model.h"
#include "saveapple/controller/saveapple_controller.h"
#include "saveapple/factory/apple_factory.h"
#include "saveapple/view/saveapple_view.h"
#include "saveapple/view/saveapple_settings_dialog.h"

SaveAppleGame::SaveAppleGame()
{
    createComponents();
    createView();
    setupUpdateTimer();
}

SaveAppleGame::~SaveAppleGame()
{
    qDebug() << "SaveAppleGame destroyed";
    
    stopUpdateLoop();

    if (m_audio_service)
    {
        m_audio_service->stopBackgroundMusic();
        m_audio_service->shutdown();
    }

    if (m_logger)
    {
        m_logger->stop();
    }

    if (m_telemetry_writer)
    {
        m_telemetry_writer->stop();
    }
}

void SaveAppleGame::initialize()
{
    if (m_controller)
    {
        m_controller->initialize();
    }

    if (m_view)
    {
        m_view->refreshFromModel();
        m_view->refreshButtonStates();
        m_view->update();
    }
}

void SaveAppleGame::startGame()
{
    if (m_model == nullptr || m_controller == nullptr)
    {
        return;
    }

    const GameState current_state = m_model->gameState();

    if (current_state == GameState::Playing)
    {
        return;
    }

    if (current_state == GameState::End)
    {
        m_controller->restartGame();
    }
    else if (current_state == GameState::Initial)
    {
        m_controller->startGame();
    }
    else
    {
        return;
    }

    startUpdateLoop();

    if (m_view)
    {
        m_view->refreshFromModel();
        m_view->refreshButtonStates();
        m_view->update();
        m_view->setFocus();
    }
}

void SaveAppleGame::pauseGame()
{
    if (m_model == nullptr || m_controller == nullptr)
    {
        return;
    }

    if (m_model->gameState() != GameState::Playing)
    {
        return;
    }

    m_controller->pauseGame();
    stopUpdateLoop();

    if (m_view)
    {
        m_view->refreshFromModel();
        m_view->refreshButtonStates();
        m_view->update();
    }
}

void SaveAppleGame::resumeGame()
{
    if (m_model == nullptr || m_controller == nullptr)
    {
        return;
    }

    if (m_model->gameState() != GameState::Paused)
    {
        return;
    }

    m_controller->resumeGame();
    startUpdateLoop();

    if (m_view)
    {
        m_view->refreshFromModel();
        m_view->refreshButtonStates();
        m_view->update();
        m_view->setFocus();
    }
}

void SaveAppleGame::stopGame()
{
    qDebug() << "[SaveAppleGame::stopGame] Called";
    if (m_model == nullptr || m_controller == nullptr)
    {
        qDebug() << "[SaveAppleGame::stopGame] Model or controller is null, returning";
        return;
    }

    const GameState current_state = m_model->gameState();
    qDebug() << "[SaveAppleGame::stopGame] Current state:" << static_cast<int>(current_state) << "(0=Initial, 1=Playing, 2=Paused, 3=End)";
    
    qDebug() << "[SaveAppleGame::stopGame] Proceeding with stop logic";

    m_controller->stopGame();
    stopUpdateLoop();

    if (m_view)
    {
        m_view->refreshFromModel();
        m_view->refreshButtonStates();
        m_view->update();
    }
}

void SaveAppleGame::restartGame()
{
    if (m_controller == nullptr)
    {
        return;
    }

    m_controller->restartGame();
    startUpdateLoop();

    if (m_view)
    {
        m_view->refreshFromModel();
        m_view->refreshButtonStates();
        m_view->update();
        m_view->setFocus();
    }
}

void SaveAppleGame::exitGame()
{
    stopGame();

    if (m_audio_service)
    {
        m_audio_service->shutdown();
    }

    if (m_view != nullptr)
    {
        QWidget* top_level = m_view->window();
        if (top_level != nullptr)
        {
            top_level->close();
        }
    }

}

void SaveAppleGame::update(int delta_ms)
{
    if (m_controller)
    {
        m_controller->update(delta_ms);
    }

    if (m_view)
    {
        m_view->refreshFromModel();
        m_view->refreshButtonStates();
        m_view->update();
    }

    if (m_model && m_model->gameState() != GameState::Playing)
    {
        stopUpdateLoop();
    }
}

GameState SaveAppleGame::gameState() const
{
    if (m_model)
    {
        return m_model->gameState();
    }

    return GameState::Initial;
}

QWidget* SaveAppleGame::gameWidget()
{
    return m_view;
}

SaveAppleModel* SaveAppleGame::model() const
{
    return m_model.get();
}

SaveAppleController* SaveAppleGame::controller() const
{
    return m_controller.get();
}

AppleFactory* SaveAppleGame::appleFactory() const
{
    return m_apple_factory.get();
}

SaveAppleView* SaveAppleGame::view() const
{
    return m_view;
}

SaveAppleSettingsDialog* SaveAppleGame::settingsDialog() const
{
    return m_settings_dialog;
}

void SaveAppleGame::createComponents()
{
    // 1. 创建核心组件
    m_model = std::make_unique<SaveAppleModel>();
    m_apple_factory = std::make_unique<AppleFactory>();
    m_controller = std::make_unique<SaveAppleController>(
        m_model.get(),
        m_apple_factory.get());
    
    // 2. 创建日志组件
    m_log_formatter = std::make_unique<PlainTextLogFormatter>();
    const QString log_file_path = buildLogFilePath();
    m_logger = std::make_unique<AsyncLogger>(
        log_file_path,
        m_log_formatter.get()
    );
    m_logger->start();
    m_logger_observer = std::make_unique<SaveAppleLoggerObserver>(m_logger.get());
    
    // 3. 创建 Telemetry 组件
    m_telemetry_writer = std::make_unique<AsyncTelemetryWriter>(buildTelemetryFilePath());
    m_telemetry_writer->start();

    m_telemetry_observer = std::make_unique<SaveAppleTelemetryObserver>(m_telemetry_writer.get());

    // 4. 创建音频组件
    m_audio_service = std::make_unique<QtAudioService>();
    m_audio_service->initialize();
    m_audio_service->setEffectVolume(0.8f);
    m_audio_service->setBackgroundMusicVolume(0.45f);


    m_audio_observer = std::make_unique<SaveAppleAudioObserver>(m_audio_service.get());
    m_audio_observer->registerDefaultCues();
    m_audio_observer->registerBackgroundMusic();
    m_audio_observer->syncSoundEnabled(m_model->soundEnabled());
    
    // 4. 连接模型与观察者
    m_model->addObserver(m_telemetry_observer.get());
    m_model->addObserver(m_logger_observer.get());
    m_model->addObserver(m_audio_observer.get());

}

void SaveAppleGame::createView()
{
    m_view = new SaveAppleView();
    m_view->setModel(m_model.get());
    m_view->setController(m_controller.get());

    m_model->addObserver(m_view);

    m_settings_dialog = new SaveAppleSettingsDialog(m_view);
    m_settings_dialog->setModel(m_model.get());

    QObject::connect(
        m_view,
        &SaveAppleView::startClicked,
        [this]()
        {
            this->startGame();
        });

    QObject::connect(
        m_view,
        &SaveAppleView::pauseResumeClicked,
        [this]()
        {
            if (m_model == nullptr)
            {
                return;
            }

            if (m_model->gameState() == GameState::Playing)
            {
                this->pauseGame();
            }
            else if (m_model->gameState() == GameState::Paused)
            {
                this->resumeGame();
            }
        });

    QObject::connect(
        m_view,
        &SaveAppleView::stopClicked,
        [this]()
        {
            this->stopGame();
        });

    QObject::connect(
        m_view,
        &SaveAppleView::settingsClicked,
        [this]()
        {
            const bool was_playing =
                (m_model != nullptr && m_model->gameState() == GameState::Playing);

            if (was_playing)
            {
                this->pauseGame();
            }

            if (m_settings_dialog != nullptr)
            {
                m_settings_dialog->setModel(m_model.get());
                m_settings_dialog->exec();
            }

            if (m_view != nullptr)
            {
                m_view->refreshFromModel();
                m_view->refreshButtonStates();
                m_view->update();
                m_view->setFocus();
            }
        });

    QObject::connect(
        m_view,
        &SaveAppleView::exitClicked,
        [this]()
        {
            this->exitGame();
        });


}

void SaveAppleGame::setupUpdateTimer()
{
    m_update_timer = std::make_unique<QTimer>();

    QObject::connect(
        m_update_timer.get(),
        &QTimer::timeout,
        [this]()
        {
            this->update(16);
        });

    m_update_timer->setInterval(16);
}

void SaveAppleGame::startUpdateLoop()
{
    if (m_update_timer && !m_update_timer->isActive())
    {
        m_update_timer->start();
    }
}

void SaveAppleGame::stopUpdateLoop()
{
    if (m_update_timer && m_update_timer->isActive())
    {
        m_update_timer->stop();
    }
}

QString SaveAppleGame::buildLogFilePath() const
{
    const QString timestamp =
        QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");

    return QString("logs/saveapple_%1.log").arg(timestamp);
}

QString SaveAppleGame::buildTelemetryFilePath() const
{
    const QString timestamp =
        QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");

    return QString("telemetry/saveapple_%1.jsonl").arg(timestamp);
}
