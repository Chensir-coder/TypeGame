/* ------------------------------------------------------------------
 // 文件名     : qt_audio_worker.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : Qt 音频后台工作对象实现
------------------------------------------------------------------ */

#include "qt_audio_worker.h"

#include <QFileInfo>
#include <QSoundEffect>
#include <QDebug>

// 为 AudioCue 定义 qHash 函数，使其可用作 QHash 的 key
uint qHash(AudioCue cue, uint seed)
{
    return qHash(static_cast<int>(cue), seed);
}

QtAudioWorker::QtAudioWorker(QObject* parent)
    : QObject(parent)
{
}

QtAudioWorker::~QtAudioWorker()

{
    qDebug() << "QtAudioWorker destroyed";
    doShutdown();
}

void QtAudioWorker::handleCommand(const AudioCommand& command)
{
    switch (command.type)
    {
        case AudioCommandType::Initialize:
            doInitialize();
            break;

        case AudioCommandType::Shutdown:
            doShutdown();
            break;

        case AudioCommandType::RegisterCue:
            doRegisterCue(command.cue, command.source_path);
            break;

        case AudioCommandType::PlayCue:
            doPlayCue(command.cue);
            break;

        case AudioCommandType::StopCue:
            doStopCue(command.cue);
            break;

        case AudioCommandType::StopAllCues:
            doStopAllCues();
            break;

        case AudioCommandType::SetEnabled:
            doSetEnabled(command.enabled);
            break;

        case AudioCommandType::SetEffectVolume:
            doSetEffectVolume(command.volume);
            break;

        case AudioCommandType::None:
        default:
            break;
    }
}

void QtAudioWorker::doInitialize()
{
    if (m_initialized)
    {
        return;
    }

    m_initialized = true;
}

void QtAudioWorker::doShutdown()
{
    if (!m_initialized)
    {
        return;
    }

    doStopAllCues();

    for (auto it = m_effects.begin(); it != m_effects.end(); ++it)
    {
        delete it.value();
    }
    m_effects.clear();

    m_source_paths.clear();

    m_initialized = false;
}

void QtAudioWorker::doRegisterCue(AudioCue cue, const QString& source_path)
{
    if (cue == AudioCue::None || source_path.isEmpty())
    {
        return;
    }

    m_source_paths.insert(cue, source_path);

    if (!m_initialized)
    {
        return;
    }

    QSoundEffect* effect = ensureEffect(cue);
    if (effect == nullptr)
    {
        return;
    }

    effect->setSource(buildAudioUrl(source_path));
}

void QtAudioWorker::doPlayCue(AudioCue cue)
{
    if (!m_initialized || !m_enabled || cue == AudioCue::None)
    {
        return;
    }

    QSoundEffect* effect = ensureEffect(cue);
    if (effect == nullptr || effect->source().isEmpty())
    {
        return;
    }

    effect->stop();
    effect->play();
}

void QtAudioWorker::doStopCue(AudioCue cue)
{
    if (!m_initialized)
    {
        return;
    }

    QSoundEffect* effect = ensureEffect(cue);
    if (effect == nullptr)
    {
        return;
    }

    effect->stop();
}

void QtAudioWorker::doStopAllCues()
{
    if (!m_initialized)
    {
        return;
    }

    for (auto it = m_effects.begin(); it != m_effects.end(); ++it)
    {
        if (it.value() != nullptr)
        {
            it.value()->stop();
        }
    }
}

void QtAudioWorker::doSetEnabled(bool enabled)
{
    m_enabled = enabled;

    if (!m_enabled)
    {
        doStopAllCues();
    }
}

void QtAudioWorker::doSetEffectVolume(float volume)
{
    m_effect_volume = clampVolume(volume);
    applyEffectVolumeToAll();
}

QSoundEffect* QtAudioWorker::ensureEffect(AudioCue cue)
{
    if (cue == AudioCue::None)
    {
        return nullptr;
    }

    auto it = m_effects.find(cue);
    if (it != m_effects.end())
    {
        return it.value();
    }

    QSoundEffect* effect = new QSoundEffect(this);
    effect->setLoopCount(1);
    effect->setVolume(m_effect_volume);

    auto source_it = m_source_paths.find(cue);
    if (source_it != m_source_paths.end())
    {
        effect->setSource(buildAudioUrl(source_it.value()));
    }

    m_effects.insert(cue, effect);
    return effect;
}

void QtAudioWorker::applyEffectVolumeToAll()
{
    for (auto it = m_effects.begin(); it != m_effects.end(); ++it)
    {
        if (it.value() != nullptr)
        {
            it.value()->setVolume(m_effect_volume);
        }
    }
}

float QtAudioWorker::clampVolume(float volume)
{
    if (volume < 0.0f)
    {
        return 0.0f;
    }

    if (volume > 1.0f)
    {
        return 1.0f;
    }

    return volume;
}

QUrl QtAudioWorker::buildAudioUrl(const QString& source_path)
{
    if (source_path.startsWith(":/"))
    {
        return QUrl(QStringLiteral("qrc%1").arg(source_path));
    }

    const QFileInfo file_info(source_path);
    if (file_info.exists())
    {
        return QUrl::fromLocalFile(file_info.absoluteFilePath());
    }

    return QUrl::fromLocalFile(source_path);
}
