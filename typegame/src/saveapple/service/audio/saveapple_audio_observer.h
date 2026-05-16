#ifndef __TYPEGAME_SAVEAPPLE_AUDIO_OBSERVER_H__
#define __TYPEGAME_SAVEAPPLE_AUDIO_OBSERVER_H__

#include "common/core/igame_observer.h"
#include "common/service/audio/audio_service.h"

/* ------------------------------------------------------------------
 // 文件名     : saveapple_audio_observer.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-18
 // 功能描述   : 拯救苹果项目音频观察者
------------------------------------------------------------------ */

class SaveAppleAudioObserver final : public IGameObserver
{
public:
    explicit SaveAppleAudioObserver(AudioService* audio_service);
    ~SaveAppleAudioObserver() override = default;

    SaveAppleAudioObserver(const SaveAppleAudioObserver&) = delete;
    SaveAppleAudioObserver& operator=(const SaveAppleAudioObserver&) = delete;

public:
    void onGameEvent(const GameEvent& event) override;

    void registerDefaultCues();
    void registerBackgroundMusic();

    /**
     * @brief 根据苹果模型的音效开关同步总音频状态。
     */
    void syncSoundEnabled(bool enabled);

private:
    AudioCue mapEffectCue(const GameEvent& event) const;
    void handleBackgroundMusicEvent(const GameEvent& event);
    void handleEffectEvent(const GameEvent& event);

private:
    AudioService* m_audio_service = nullptr;
};

#endif // __TYPEGAME_SAVEAPPLE_AUDIO_OBSERVER_H__
