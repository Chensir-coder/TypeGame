#ifndef __TYPEGAME_ASYNC_QT_AUDIO_SERVICE_H__
#define __TYPEGAME_ASYNC_QT_AUDIO_SERVICE_H__

#include <QMutex>
#include <QThread>
#include <QMediaPlayer>

#include "audio_service.h"
#include "audio_command.h"

/* ------------------------------------------------------------------
 // 文件名     : async_qt_audio_service.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 基于 Qt 的异步音频服务实现
------------------------------------------------------------------ */

class QtAudioWorker;

class QtAudioService final : public QObject, public AudioService
{
    Q_OBJECT

public:
    explicit QtAudioService();
    ~QtAudioService() override;

    QtAudioService(const QtAudioService&) = delete;
    QtAudioService& operator=(const QtAudioService&) = delete;

public:
    void initialize() override;
    void shutdown() override;

    // ---------- 短音效 ----------
    void registerCue(AudioCue cue, const QString& source_path) override;
    void playCue(AudioCue cue) override;
    void stopCue(AudioCue cue) override;
    void stopAllCues() override;

    // ---------- 背景音乐 ----------
    void setBackgroundMusic(const QString& source_path) override;
    void playBackgroundMusic() override;
    void pauseBackgroundMusic() override;
    void resumeBackgroundMusic() override;
    void stopBackgroundMusic() override;
    bool isBackgroundMusicPlaying() const override;

    // ---------- 总开关 ----------
    void setEnabled(bool enabled) override;
    bool isEnabled() const override;

    // ---------- 音量 ----------
    void setEffectVolume(float volume) override;
    float effectVolume() const override;

    void setBackgroundMusicVolume(float volume) override;
    float backgroundMusicVolume() const override;

private:
    void postCommand(const AudioCommand& command);

    static QUrl buildAudioUrl(const QString& source_path);
    static QMediaContent buildAudioContent(const QString& source_path);

private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

private:
    bool m_initialized = false;
    bool m_enabled = true;

    float m_effect_volume = 0.8f;
    float m_background_music_volume = 0.5f;

    mutable QMutex m_state_mutex;

    QString m_background_music_path;
    QMediaPlayer* m_background_music_player = nullptr;

    QThread* m_worker_thread = nullptr;
    QtAudioWorker* m_worker = nullptr;
};

#endif // __TYPEGAME_ASYNC_QT_AUDIO_SERVICE_H__
