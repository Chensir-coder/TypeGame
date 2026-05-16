#ifndef __TYPEGAME_SAVEAPPLE_GAME_H__
#define __TYPEGAME_SAVEAPPLE_GAME_H__

#include <memory>

#include "common/core/igame.h"
#include "common/service/log/ilog_formatter.h"
#include "common/service/log/ilogger.h"
#include "saveapple/service/log/saveapple_logger_observer.h"
#include "saveapple/service/telemetry/saveapple_telemetry_observer.h"
#include "saveapple/service/audio/saveapple_audio_observer.h"
#include "common/service/audio/async_qt_audio_service.h"

/* ------------------------------------------------------------------
 // 文件名     : saveapple_game.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 拯救苹果游戏模块总入口
------------------------------------------------------------------ */

class QTimer;
class SaveAppleModel;
class SaveAppleController;
class AppleFactory;
class SaveAppleView;
class SaveAppleSettingsDialog;

/**
 * @brief 拯救苹果游戏模块总入口。
 *
 * 职责：
 * 1. 实现 IGame，向外提供统一游戏模块接口。
 * 2. 组装 SaveAppleModel、SaveAppleController、AppleFactory、SaveAppleView、
 *    SaveAppleSettingsDialog 等核心组件。
 * 3. 管理苹果游戏模块主要对象的生命周期。
 * 4. 向 app 层提供可挂载的游戏页面控件。
 * 5. 提供模块内部最小更新驱动能力。
 */
class SaveAppleGame final : public IGame
{
public:
    explicit SaveAppleGame();
    ~SaveAppleGame() override;

    SaveAppleGame(const SaveAppleGame&) = delete;
    SaveAppleGame& operator=(const SaveAppleGame&) = delete;

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

    SaveAppleModel* model() const;
    SaveAppleController* controller() const;
    AppleFactory* appleFactory() const;
    SaveAppleView* view() const;
    SaveAppleSettingsDialog* settingsDialog() const;

private:

    void createComponents();
    void createView();
    void setupUpdateTimer();
    void startUpdateLoop();
    void stopUpdateLoop();

    QString buildLogFilePath() const;
    QString buildTelemetryFilePath() const;


private:

    std::unique_ptr<SaveAppleModel> m_model;
    std::unique_ptr<AppleFactory> m_apple_factory;
    std::unique_ptr<SaveAppleController> m_controller;
    SaveAppleView* m_view = nullptr;
    SaveAppleSettingsDialog* m_settings_dialog = nullptr;
    std::unique_ptr<QTimer> m_update_timer;

    std::unique_ptr<ILogFormatter> m_log_formatter;
    std::unique_ptr<ILogger> m_logger;
    std::unique_ptr<SaveAppleLoggerObserver> m_logger_observer;

    std::unique_ptr<ITelemetryWriter> m_telemetry_writer;
    std::unique_ptr<SaveAppleTelemetryObserver> m_telemetry_observer;

    std::unique_ptr<SaveAppleAudioObserver> m_audio_observer;
    std::unique_ptr<QtAudioService> m_audio_service;

};

#endif // __TYPEGAME_SAVEAPPLE_GAME_H__
