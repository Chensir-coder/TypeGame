#ifndef __TYPEGAME_SPACEWAR_AUDIO_OBSERVER_H__
#define __TYPEGAME_SPACEWAR_AUDIO_OBSERVER_H__

#include <QObject>
#include <QString>

#include "common/core/igame_observer.h"
#include "common/service/audio/audio_service.h"

/* ------------------------------------------------------------------
 // 文件名     : spacewar_audio_observer.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 飞机大战音频观察者（背景音乐 + 各类一次性音效）
------------------------------------------------------------------ */

/**
 * 飞机大战音频观察者。
 *
 * 设计：
 *   1. 背景音乐走 AudioService（与 saveapple 一致的暂停 / 恢复 / 停止）。
 *   2. 短音效通过 AudioService::registerCue / playCue 在 QtAudioWorker 线程播放，
 *      主线程只投递命令，避免 QSoundEffect 解码与播放在 UI 线程造成卡顿。
 *   3. wordOut 仅在奖励整词输入完成时播放（EntityHit 且 value == -1）；
 *      奖励未打完飞出场景（EntityRemoved 且 value == -1）不播放。
 *   4. 总开关：SoundEnabled 变化时 syncSoundEnabled，由 AudioService 停止所有 cue。
 *
 * 音效触发时机（须开启游戏内音效；背景音乐在 GameStarted 后播放）：
 *   - SPACE_SHOOT：发射子弹（EntitySpawned 且 extra_value == 1）。
 *   - SPACE_BLAST：敌机被子弹击毁（EntityHit 且 letter 非空，且非奖励完成）。
 *   - SPACE_PLANEOUT：玩家被撞掉血（EntityMissed）；或生命归零（HpChanged 且 hp <= 0）。
 *   - SPACE_WORDOUT：奖励单词整词打完（EntityHit 且 value == -1）。
 *   - UPGRADE：难度等级提升（LevelChanged）。
 */
class SpaceWarAudioObserver final : public QObject, public IGameObserver
{
    Q_OBJECT

public:
    explicit SpaceWarAudioObserver(AudioService* audio_service, QObject* parent = nullptr);
    ~SpaceWarAudioObserver() override = default;

    SpaceWarAudioObserver(const SpaceWarAudioObserver&) = delete;
    SpaceWarAudioObserver& operator=(const SpaceWarAudioObserver&) = delete;

    void onGameEvent(const GameEvent& event) override;

    void registerDefaultCues();
    void registerBackgroundMusic();

    void syncSoundEnabled(bool enabled);

private:
    void playSpaceWarCue(AudioCue cue);

    void handleBackgroundMusicEvent(const GameEvent& event);
    void handleSpaceWarSpecificEffect(const GameEvent& event);

private:
    AudioService* m_audio_service = nullptr;
    bool m_sound_enabled = true;
};

#endif // __TYPEGAME_SPACEWAR_AUDIO_OBSERVER_H__
