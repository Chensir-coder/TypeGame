/* ------------------------------------------------------------------
 // 文件名     : saveapple_audio_observer.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-18
 // 功能描述   : 拯救苹果项目音频观察者实现
------------------------------------------------------------------ */

#include "saveapple_audio_observer.h"

#include "app/resources/asset_paths.h"
#include "common/core/game_event.h"

SaveAppleAudioObserver::SaveAppleAudioObserver(AudioService* audio_service)
    : m_audio_service(audio_service)
{
}

void SaveAppleAudioObserver::onGameEvent(const GameEvent& event)
{
    if (m_audio_service == nullptr)
    {
        return;
    }

    if (event.type == GameEventType::SettingsChanged && event.setting_key == SettingKey::SoundEnabled)
    {
        syncSoundEnabled(event.value != 0);
    }

    handleBackgroundMusicEvent(event);
    handleEffectEvent(event);
}

void SaveAppleAudioObserver::registerDefaultCues()
{
    if (m_audio_service == nullptr )
    {
        return;
    }

    // 仅保留苹果命中短音效
    m_audio_service->registerCue(AudioCue::EntityHit, UiAssetPaths::Apple::inSound());
    
}

void SaveAppleAudioObserver::registerBackgroundMusic()
{
    if (m_audio_service == nullptr)
    {
        return;
    }

    // 这里按你的资源函数名改。
    // 假设你后面在 asset_paths 里提供一个背景音乐路径：
    // UiAssetPaths::Apple::backgroundMusic()
    m_audio_service->setBackgroundMusic(UiAssetPaths::Apple::backgroundSound());
}

void SaveAppleAudioObserver::syncSoundEnabled(bool enabled)
{
    if (m_audio_service == nullptr)
    {
        return;
    }

    m_audio_service->setEnabled(enabled);
}

AudioCue SaveAppleAudioObserver::mapEffectCue(const GameEvent& event) const
{
    switch (event.type)
    {
        case GameEventType::EntityHit:
            return AudioCue::EntityHit;

        default:
            break;
    }

    return AudioCue::None;
}

void SaveAppleAudioObserver::handleBackgroundMusicEvent(const GameEvent& event)
{
    if (m_audio_service == nullptr || !m_audio_service->isEnabled())
    {
        return;
    }

    switch (event.type)
    {
        case GameEventType::GameStarted:
            m_audio_service->playBackgroundMusic();
            break;

        case GameEventType::GamePaused:
            m_audio_service->pauseBackgroundMusic();
            break;

        case GameEventType::GameResumed:
            m_audio_service->resumeBackgroundMusic();
            break;

        case GameEventType::GameStopped:
            m_audio_service->stopBackgroundMusic();
            break;

        case GameEventType::GameOver:
            m_audio_service->stopBackgroundMusic();
            break;

        default:
            break;
    }
}

void SaveAppleAudioObserver::handleEffectEvent(const GameEvent& event)
{
    if (m_audio_service == nullptr || !m_audio_service->isEnabled())
    {
        return;
    }

    const AudioCue cue = mapEffectCue(event);
    if (cue == AudioCue::None)
    {
        return;
    }

    m_audio_service->playCue(cue);
}
