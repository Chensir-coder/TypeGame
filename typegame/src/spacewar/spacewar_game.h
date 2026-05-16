#ifndef __TYPEGAME_SPACEWAR_GAME_H__
#define __TYPEGAME_SPACEWAR_GAME_H__

#include <QElapsedTimer>

#include <memory>

#include "common/core/igame.h"
#include "common/service/audio/async_qt_audio_service.h"
#include "common/service/log/ilog_formatter.h"
#include "common/service/log/ilogger.h"
#include "common/service/telemetry/itelemetry_writer.h"
#include "spacewar/service/audio/spacewar_audio_observer.h"
#include "spacewar/service/log/spacewar_logger_observer.h"
#include "spacewar/service/telemetry/spacewar_telemetry_observer.h"

/* ------------------------------------------------------------------
 // 文件名     : spacewar_game.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 飞机大战模块入口：组件创建 / 公共服务接入 / 设置弹窗 / 主循环
------------------------------------------------------------------ */

class QTimer;
class SpaceWarController;
class SpaceWarModel;
class SpaceWarSettingsDialog;
class SpaceWarView;

class SpaceWarGame final : public IGame
{
public:
    SpaceWarGame();
    ~SpaceWarGame() override;

    SpaceWarGame(const SpaceWarGame&) = delete;
    SpaceWarGame& operator=(const SpaceWarGame&) = delete;

    void initialize() override;
    void startGame() override;
    void pauseGame() override;
    void resumeGame() override;
    void stopGame() override;
    void restartGame() override;
    void exitGame() override;
    void update(int delta_ms) override;

    GameState gameState() const override;
    QWidget* gameWidget() override;

    SpaceWarModel* model() const;
    SpaceWarController* controller() const;
    SpaceWarView* view() const;
    SpaceWarSettingsDialog* settingsDialog() const;

private:
    void createComponents();
    void createView();
    void wireUpdateTimer();
    void startUpdateLoop();
    void stopUpdateLoop();

    void onSettingsClicked();
    void requestRewardWord();

    QString buildLogFilePath() const;
    QString buildTelemetryFilePath() const;

private:
    std::unique_ptr<SpaceWarModel> m_model;
    std::unique_ptr<SpaceWarController> m_controller;
    SpaceWarView* m_view = nullptr;
    SpaceWarSettingsDialog* m_settings_dialog = nullptr;
    std::unique_ptr<QTimer> m_update_timer;
    QElapsedTimer m_frame_timer;
    QElapsedTimer m_update_perf_timer;

    qint64 m_perf_update_total_ms = 0;
    qint64 m_perf_update_max_ms = 0;
    qint64 m_perf_delta_max_ms = 0;
    int m_perf_frame_count = 0;

    std::unique_ptr<ILogFormatter> m_log_formatter;
    std::unique_ptr<ILogger> m_logger;
    std::unique_ptr<SpaceWarLoggerObserver> m_logger_observer;

    std::unique_ptr<ITelemetryWriter> m_telemetry_writer;
    std::unique_ptr<SpaceWarTelemetryObserver> m_telemetry_observer;

    std::unique_ptr<SpaceWarAudioObserver> m_audio_observer;
    std::unique_ptr<QtAudioService> m_audio_service;
};

#endif // __TYPEGAME_SPACEWAR_GAME_H__
