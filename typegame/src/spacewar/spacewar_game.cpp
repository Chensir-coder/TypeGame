/* ------------------------------------------------------------------
 // 文件名     : spacewar_game.cpp
------------------------------------------------------------------ */

#include "spacewar_game.h"

#include <QDateTime>
#include <QDebug>
#include <QElapsedTimer>
#include <QPointer>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QtGlobal>

#include "common/core/game_state.h"
#include "common/service/ai/ai_word_service.h"
#include "common/service/audio/async_qt_audio_service.h"
#include "common/service/log/async_logger.h"
#include "common/service/log/log_formatter.h"
#include "common/service/telemetry/async_telemetry_writer.h"
#include "spacewar/controller/spacewar_controller.h"
#include "spacewar/model/spacewar_model.h"
#include "spacewar/service/audio/spacewar_audio_observer.h"
#include "spacewar/view/spacewar_settings_dialog.h"
#include "spacewar/view/spacewar_view.h"
#include "common/service/config/configure_settings.h"

SpaceWarGame::SpaceWarGame()
{
    createComponents();
    createView();
    wireUpdateTimer();
}

SpaceWarGame::~SpaceWarGame()
{
    qDebug() << "SpaceWarGame destroyed";

    stopUpdateLoop();

    if (m_controller) {
        m_controller->attachView(nullptr);
    }

    delete m_view;
    m_view = nullptr;

    if (m_audio_service) {
        m_audio_service->stopBackgroundMusic();
        m_audio_service->shutdown();
    }

    if (m_logger) {
        m_logger->stop();
    }

    if (m_telemetry_writer) {
        m_telemetry_writer->stop();
    }
}

void SpaceWarGame::initialize()
{
    if (m_controller) {
        m_controller->initialize();
    }

    if (m_view != nullptr) {
        m_view->refreshHudFromModel();
        m_view->refreshButtonStates();
        m_view->update();
        m_view->setFocus();
    }
}

void SpaceWarGame::startGame()
{
    if (m_model == nullptr || m_controller == nullptr) {
        return;
    }

    const GameState s = m_model->gameState();
    if (s == GameState::Playing) {
        return;
    }

    if (s == GameState::End) {
        m_controller->restartGame();
    } else if (s == GameState::Initial) {
        m_controller->startGame();
    } else {
        return;
    }

    startUpdateLoop();

    if (m_view != nullptr) {
        m_view->setFocus();
    }
}

void SpaceWarGame::pauseGame()
{
    if (m_model == nullptr || m_controller == nullptr) {
        return;
    }

    if (m_model->gameState() != GameState::Playing) {
        return;
    }

    m_controller->pauseGame();
    stopUpdateLoop();
}

void SpaceWarGame::resumeGame()
{
    if (m_model == nullptr || m_controller == nullptr) {
        return;
    }

    if (m_model->gameState() != GameState::Paused) {
        return;
    }

    m_controller->resumeGame();
    startUpdateLoop();

    if (m_view != nullptr) {
        m_view->setFocus();
    }
}

void SpaceWarGame::stopGame()
{
    if (m_controller != nullptr) {
        m_controller->stopGame();
    }
    stopUpdateLoop();
}

void SpaceWarGame::restartGame()
{
    if (m_controller == nullptr) {
        return;
    }

    m_controller->restartGame();
    startUpdateLoop();

    if (m_view != nullptr) {
        m_view->setFocus();
    }
}

void SpaceWarGame::exitGame()
{
    stopGame();

    if (m_audio_service) {
        m_audio_service->shutdown();
    }

    if (m_view != nullptr) {
        QWidget* top_level = m_view->window();
        if (top_level != nullptr) {
            top_level->close();
        }
    }
}

void SpaceWarGame::update(int delta_ms)
{
    QElapsedTimer updateTimer;
    updateTimer.start();

    if (m_controller != nullptr) {
        m_controller->update(delta_ms);
    }

    if (m_view != nullptr) {
        m_view->tickSceneRefresh(delta_ms);
        m_view->refreshHudFromModel();
        m_view->refreshButtonStates();
    }

    if (m_model != nullptr && m_model->gameState() != GameState::Playing) {
        stopUpdateLoop();
    }

    const qint64 updateElapsedMs = updateTimer.elapsed();
    if (!m_update_perf_timer.isValid()) {
        m_update_perf_timer.start();
    }

    m_perf_update_total_ms += updateElapsedMs;
    m_perf_update_max_ms = qMax(m_perf_update_max_ms, updateElapsedMs);
    m_perf_delta_max_ms = qMax(m_perf_delta_max_ms, static_cast<qint64>(delta_ms));
    ++m_perf_frame_count;

    if (updateElapsedMs >= 20) {
        qDebug() << "[spacewar perf] slow update"
                 << "deltaMs=" << delta_ms
                 << "updateMs=" << updateElapsedMs;
    }

    if (m_update_perf_timer.elapsed() >= 1000 && m_perf_frame_count > 0) {
        qDebug() << "[spacewar perf] update summary"
                 << "frames=" << m_perf_frame_count
                 << "maxDeltaMs=" << m_perf_delta_max_ms
                 << "avgUpdateMs="
                 << (static_cast<double>(m_perf_update_total_ms) / m_perf_frame_count)
                 << "maxUpdateMs=" << m_perf_update_max_ms;

        m_update_perf_timer.restart();
        m_perf_update_total_ms = 0;
        m_perf_update_max_ms = 0;
        m_perf_delta_max_ms = 0;
        m_perf_frame_count = 0;
    }
}

GameState SpaceWarGame::gameState() const
{
    if (m_model != nullptr) {
        return m_model->gameState();
    }
    return GameState::Initial;
}

QWidget* SpaceWarGame::gameWidget()
{
    return m_view;
}

SpaceWarModel* SpaceWarGame::model() const
{
    return m_model.get();
}

SpaceWarController* SpaceWarGame::controller() const
{
    return m_controller.get();
}

SpaceWarView* SpaceWarGame::view() const
{
    return m_view;
}

SpaceWarSettingsDialog* SpaceWarGame::settingsDialog() const
{
    return m_settings_dialog;
}

void SpaceWarGame::createComponents()
{
    ConfigureSettings::instance().load();

    m_model = std::make_unique<SpaceWarModel>();
    m_controller = std::make_unique<SpaceWarController>(m_model.get());

    m_log_formatter = std::make_unique<PlainTextLogFormatter>();
    m_logger = std::make_unique<AsyncLogger>(buildLogFilePath(), m_log_formatter.get());
    m_logger->start();
    m_logger_observer = std::make_unique<SpaceWarLoggerObserver>(m_logger.get());

    m_telemetry_writer = std::make_unique<AsyncTelemetryWriter>(buildTelemetryFilePath());
    m_telemetry_writer->start();
    m_telemetry_observer = std::make_unique<SpaceWarTelemetryObserver>(m_telemetry_writer.get());

    m_audio_service = std::make_unique<QtAudioService>();
    m_audio_service->initialize();
    m_audio_service->setEffectVolume(0.7f);
    m_audio_service->setBackgroundMusicVolume(0.45f);

    m_audio_observer = std::make_unique<SpaceWarAudioObserver>(m_audio_service.get());
    m_audio_observer->registerDefaultCues();
    m_audio_observer->registerBackgroundMusic();
    m_audio_observer->syncSoundEnabled(m_model->spaceWarSoundEnabled());

    m_model->addObserver(m_logger_observer.get());
    m_model->addObserver(m_telemetry_observer.get());
    m_model->addObserver(m_audio_observer.get());
}

void SpaceWarGame::createView()
{
    m_view = new SpaceWarView();
    m_view->setModel(m_model.get());

    if (m_controller != nullptr) {
        m_controller->attachView(m_view);
    }

    m_settings_dialog = new SpaceWarSettingsDialog(m_view);
    m_settings_dialog->setModel(m_model.get());

    QObject::connect(m_view, &SpaceWarView::settingsClicked,
                     m_view, [this]() { onSettingsClicked(); });
}

void SpaceWarGame::wireUpdateTimer()
{
    m_update_timer = std::make_unique<QTimer>();
    m_update_timer->setTimerType(Qt::PreciseTimer);

    QObject::connect(m_update_timer.get(), &QTimer::timeout,
                     m_controller.get(), [this]() {
        const qint64 rawElapsedMs = m_frame_timer.isValid()
            ? m_frame_timer.restart()
            : 16;

        const int deltaMs = qMax(1, static_cast<int>(rawElapsedMs));
        if (rawElapsedMs >= 50) {
            qDebug() << "[spacewar perf] timer delayed"
                     << "rawDeltaMs=" << rawElapsedMs
                     << "usedDeltaMs=" << deltaMs;
        }
        this->update(deltaMs);
    });

    QObject::connect(m_controller.get(), &SpaceWarController::requestStartUpdateLoop,
                     m_controller.get(), [this]() { this->startUpdateLoop(); });

    QObject::connect(m_controller.get(), &SpaceWarController::requestStopUpdateLoop,
                     m_controller.get(), [this]() { this->stopUpdateLoop(); });

    QObject::connect(m_controller.get(), &SpaceWarController::requestExitToLauncher,
                     m_controller.get(), [this]() { this->exitGame(); });

    QObject::connect(m_controller.get(), &SpaceWarController::requestRewardWord,
                     m_controller.get(), [this]() { this->requestRewardWord(); });

    m_update_timer->setInterval(16);
}

void SpaceWarGame::startUpdateLoop()
{
    if (m_update_timer != nullptr && !m_update_timer->isActive()) {
        m_frame_timer.restart();
        m_update_perf_timer.restart();
        m_update_timer->start();
    }
}

void SpaceWarGame::stopUpdateLoop()
{
    if (m_update_timer != nullptr && m_update_timer->isActive()) {
        m_update_timer->stop();
    }
    m_frame_timer.invalidate();
    m_update_perf_timer.invalidate();
}

void SpaceWarGame::onSettingsClicked()
{
    if (m_settings_dialog == nullptr || m_model == nullptr) {
        return;
    }

    /*
     * 弹窗期间暂停游戏，关闭后由用户主动恢复。
     */
    const bool was_playing = (m_model->gameState() == GameState::Playing);
    if (was_playing) {
        pauseGame();
    }

    m_settings_dialog->setModel(m_model.get());
    const int rc = m_settings_dialog->exec();
    Q_UNUSED(rc);

    // 应用设置到运行时
    if (m_controller != nullptr) {
        m_controller->applySettingsToRuntime();
    }

    if (m_view != nullptr) {
        m_view->refreshHudFromModel();
        m_view->refreshButtonStates();
        m_view->update();
        m_view->setFocus();
    }
}

void SpaceWarGame::requestRewardWord()
{
    if (m_controller == nullptr || m_model == nullptr) {
        return;
    }

    QPointer<SpaceWarController> safeController(m_controller.get());

    const auto& ai = ConfigureSettings::instance().spacewar().rewardWordAi;

    AiWordService::instance().requestRandomWordAsync(
        ai.topic,
        ai.minLength,
        ai.maxLength,
        [safeController](const QString& word, bool fromLocal) {
            Q_UNUSED(fromLocal);
            if (safeController.isNull()) {
                return;
            }
            safeController->provideRewardWord(word);
        });
}

QString SpaceWarGame::buildLogFilePath() const
{
    const QString timestamp =
        QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");
    return QString("logs/spacewar_%1.log").arg(timestamp);
}

QString SpaceWarGame::buildTelemetryFilePath() const
{
    const QString timestamp =
        QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss_zzz");
    return QString("telemetry/spacewar_%1.jsonl").arg(timestamp);
}
