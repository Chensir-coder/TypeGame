#ifndef __TYPEGAME_AUDIO_COMMAND_H__
#define __TYPEGAME_AUDIO_COMMAND_H__

#include <QString>
#include <QMetaType>

#include "audio_service.h"

/* ------------------------------------------------------------------
 // 文件名     : audio_command.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 音频异步命令定义
------------------------------------------------------------------ */

enum class AudioCommandType
{
    None = 0,

    Initialize,
    Shutdown,

    RegisterCue,
    PlayCue,
    StopCue,
    StopAllCues,

    SetEnabled,

    SetEffectVolume
};

struct AudioCommand
{
    AudioCommandType type = AudioCommandType::None;

    AudioCue cue = AudioCue::None;
    QString source_path;

    bool enabled = true;
    float volume = 1.0f;
};

Q_DECLARE_METATYPE(AudioCommand)

#endif // __TYPEGAME_AUDIO_COMMAND_H__
