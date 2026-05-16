/* ------------------------------------------------------------------
 // 文件名     : spacewar_audio_observer.cpp
------------------------------------------------------------------ */

#include "spacewar_audio_observer.h"

#include "app/resources/asset_paths.h"
#include "common/core/game_event.h"

SpaceWarAudioObserver::SpaceWarAudioObserver(AudioService* audio_service, QObject* parent)
    : QObject(parent)
    , m_audio_service(audio_service)
{
}

void SpaceWarAudioObserver::onGameEvent(const GameEvent& event)
{
    if (event.type == GameEventType::SettingsChanged
        && event.setting_key == SettingKey::SoundEnabled)
    {
        syncSoundEnabled(event.value != 0);
    }

    handleBackgroundMusicEvent(event);

    if (m_sound_enabled) {
        handleSpaceWarSpecificEffect(event);
    }
}

void SpaceWarAudioObserver::registerDefaultCues()
{
    if (m_audio_service == nullptr) {
        return;
    }

    m_audio_service->registerCue(AudioCue::SpaceWarShoot,
                                   UiAssetPaths::SpaceWar::shootSound());
    m_audio_service->registerCue(AudioCue::SpaceWarBlast,
                                   UiAssetPaths::SpaceWar::blastSound());
    m_audio_service->registerCue(AudioCue::SpaceWarPlaneOut,
                                   UiAssetPaths::SpaceWar::planeOutSound());
    m_audio_service->registerCue(AudioCue::SpaceWarWordOut,
                                   UiAssetPaths::SpaceWar::wordOutSound());
    m_audio_service->registerCue(AudioCue::SpaceWarUpgrade,
                                   UiAssetPaths::SpaceWar::upgradeSound());
}

void SpaceWarAudioObserver::registerBackgroundMusic()
{
    if (m_audio_service == nullptr) {
        return;
    }
    m_audio_service->setBackgroundMusic(UiAssetPaths::SpaceWar::backgroundSound());
}

void SpaceWarAudioObserver::syncSoundEnabled(bool enabled)
{
    m_sound_enabled = enabled;

    if (m_audio_service != nullptr) {
        m_audio_service->setEnabled(enabled);
    }
}

void SpaceWarAudioObserver::playSpaceWarCue(AudioCue cue)
{
    if (m_audio_service == nullptr || cue == AudioCue::None) {
        return;
    }
    m_audio_service->playCue(cue);
}

void SpaceWarAudioObserver::handleBackgroundMusicEvent(const GameEvent& event)
{
    if (m_audio_service == nullptr || !m_audio_service->isEnabled()) {
        return;
    }

    switch (event.type) {
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
    case GameEventType::GameOver:
        m_audio_service->stopBackgroundMusic();
        break;
    default:
        break;
    }
}

void SpaceWarAudioObserver::handleSpaceWarSpecificEffect(const GameEvent& event)
{
    switch (event.type) {
    case GameEventType::EntitySpawned:
        if (event.extra_value == 1) {
            playSpaceWarCue(AudioCue::SpaceWarShoot);
        }
        break;

    case GameEventType::EntityHit:
        if (event.value == -1) {
            playSpaceWarCue(AudioCue::SpaceWarWordOut);
        } else if (!event.letter.isNull()) {
            playSpaceWarCue(AudioCue::SpaceWarBlast);
        }
        break;

    case GameEventType::EntityMissed:
        playSpaceWarCue(AudioCue::SpaceWarPlaneOut);
        break;

    case GameEventType::HpChanged:
        if (event.hp <= 0) {
            playSpaceWarCue(AudioCue::SpaceWarPlaneOut);
        }
        break;

    case GameEventType::LevelChanged:
        playSpaceWarCue(AudioCue::SpaceWarUpgrade);
        break;

    default:
        break;
    }
}
