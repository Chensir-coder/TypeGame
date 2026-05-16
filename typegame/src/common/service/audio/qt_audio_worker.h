#ifndef __TYPEGAME_QT_AUDIO_WORKER_H__
#define __TYPEGAME_QT_AUDIO_WORKER_H__

#include <QHash>
#include <QObject>
#include <QUrl>

#include "audio_command.h"

/* ------------------------------------------------------------------
 // 文件名     : qt_audio_worker.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : Qt 音频后台工作对象
------------------------------------------------------------------ */

class QMediaContent;
class QSoundEffect;

class QtAudioWorker final : public QObject
{
    Q_OBJECT

public:
    explicit QtAudioWorker(QObject* parent = nullptr);
    ~QtAudioWorker() override;

    QtAudioWorker(const QtAudioWorker&) = delete;
    QtAudioWorker& operator=(const QtAudioWorker&) = delete;

public slots:
    void handleCommand(const AudioCommand& command);

private:
    void doInitialize();
    void doShutdown();

    void doRegisterCue(AudioCue cue, const QString& source_path);
    void doPlayCue(AudioCue cue);
    void doStopCue(AudioCue cue);
    void doStopAllCues();

    void doSetEnabled(bool enabled);
    void doSetEffectVolume(float volume);

private:
    QSoundEffect* ensureEffect(AudioCue cue);
    void applyEffectVolumeToAll();

    static float clampVolume(float volume);
    static QUrl buildAudioUrl(const QString& source_path);

private:
    bool m_initialized = false;
    bool m_enabled = true;

    float m_effect_volume = 0.8f;

    QHash<AudioCue, QString> m_source_paths;
    QHash<AudioCue, QSoundEffect*> m_effects;
};

#endif // __TYPEGAME_QT_AUDIO_WORKER_H__
