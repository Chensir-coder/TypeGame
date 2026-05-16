/* ------------------------------------------------------------------
 // 文件名     : qt_audio_service.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 基于 Qt 的异步音频服务实现
------------------------------------------------------------------ */

#include "async_qt_audio_service.h"

#include <QMetaObject>
#include <QMutexLocker>
#include <QDebug>
#include <QFileInfo>
#include <QMediaContent>
#include "audio_command.h"
#include "qt_audio_worker.h"

QtAudioService::QtAudioService()
{
    m_background_music_player = new QMediaPlayer(this);
    QObject::connect(m_background_music_player,
                     SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
                     this,
                     SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));
}

QtAudioService::~QtAudioService()
{
    qDebug() << "QtAudioService destroyed";
    shutdown();
}

void QtAudioService::initialize()
{
    QMutexLocker locker(&m_state_mutex);

    if (m_initialized)
    {
        return;
    }

    qRegisterMetaType<AudioCommand>("AudioCommand");

    m_worker_thread = new QThread();
    m_worker = new QtAudioWorker();

    m_worker->moveToThread(m_worker_thread);

    QObject::connect(
        m_worker_thread,
        &QThread::finished,
        m_worker,
        &QObject::deleteLater);

    m_worker_thread->start();

    AudioCommand init_command;
    init_command.type = AudioCommandType::Initialize;

    QMetaObject::invokeMethod(
        m_worker,
        "handleCommand",
        Qt::BlockingQueuedConnection,
        Q_ARG(AudioCommand, init_command));


    AudioCommand enabled_command;
    enabled_command.type = AudioCommandType::SetEnabled;
    enabled_command.enabled = m_enabled;
    postCommand(enabled_command);

    AudioCommand effect_volume_command;
    effect_volume_command.type = AudioCommandType::SetEffectVolume;
    effect_volume_command.volume = m_effect_volume;
    postCommand(effect_volume_command);

    m_background_music_player->setVolume(static_cast<int>(m_background_music_volume * 100));

    m_initialized = true;
}

void QtAudioService::shutdown()
{
    QMutexLocker locker(&m_state_mutex);

    if (!m_initialized)
    {
        return;
    }

    AudioCommand shutdown_command;
    shutdown_command.type = AudioCommandType::Shutdown;

    if (m_worker != nullptr)
    {
        QMetaObject::invokeMethod(
            m_worker,
            "handleCommand",
            Qt::BlockingQueuedConnection,
            Q_ARG(AudioCommand, shutdown_command));
    }

    if (m_worker_thread != nullptr)
    {
        m_worker_thread->quit();
        m_worker_thread->wait();

        delete m_worker_thread;
        m_worker_thread = nullptr;
    }

    m_worker = nullptr;
    m_initialized = false;
}

void QtAudioService::registerCue(AudioCue cue, const QString& source_path)
{
    if (cue == AudioCue::None || source_path.isEmpty())
    {
        return;
    }

    AudioCommand command;
    command.type = AudioCommandType::RegisterCue;
    command.cue = cue;
    command.source_path = source_path;
    postCommand(command);
}

void QtAudioService::playCue(AudioCue cue)
{
    QMutexLocker locker(&m_state_mutex);

    if (!m_initialized || !m_enabled || cue == AudioCue::None)
    {
        return;
    }

    AudioCommand command;
    command.type = AudioCommandType::PlayCue;
    command.cue = cue;
    postCommand(command);
}

void QtAudioService::stopCue(AudioCue cue)
{
    AudioCommand command;
    command.type = AudioCommandType::StopCue;
    command.cue = cue;
    postCommand(command);
}

void QtAudioService::stopAllCues()
{
    AudioCommand command;
    command.type = AudioCommandType::StopAllCues;
    postCommand(command);
}

void QtAudioService::setBackgroundMusic(const QString& source_path)
{
    QMutexLocker locker(&m_state_mutex);

    if (!m_initialized)
    {
        m_background_music_path = source_path;
        return;
    }

    m_background_music_path = source_path;

    if (source_path.isEmpty())
    {
        m_background_music_player->setMedia(QMediaContent());
    }
    else
    {
        m_background_music_player->setMedia(buildAudioContent(source_path));
    }
}

void QtAudioService::playBackgroundMusic()
{
    QMutexLocker locker(&m_state_mutex);

    if (!m_initialized || !m_enabled)
    {
        return;
    }

    if (m_background_music_player->media().isNull() && !m_background_music_path.isEmpty())
    {
        m_background_music_player->setMedia(buildAudioContent(m_background_music_path));
    }

    if (!m_background_music_player->media().isNull())
    {
        m_background_music_player->play();
    }
}

void QtAudioService::pauseBackgroundMusic()
{
    QMutexLocker locker(&m_state_mutex);

    if (!m_initialized)
    {
        return;
    }

    m_background_music_player->pause();
}

void QtAudioService::resumeBackgroundMusic()
{
    QMutexLocker locker(&m_state_mutex);

    if (!m_initialized || !m_enabled)
    {
        return;
    }

    if (m_background_music_player->media().isNull() && !m_background_music_path.isEmpty())
    {
        m_background_music_player->setMedia(buildAudioContent(m_background_music_path));
    }

    if (!m_background_music_player->media().isNull())
    {
        m_background_music_player->play();
    }
}

void QtAudioService::stopBackgroundMusic()
{
    QMutexLocker locker(&m_state_mutex);

    if (!m_initialized)
    {
        return;
    }

    m_background_music_player->stop();
}

bool QtAudioService::isBackgroundMusicPlaying() const
{
    QMutexLocker locker(&m_state_mutex);
    return m_background_music_player->state() == QMediaPlayer::PlayingState;
}

void QtAudioService::setEnabled(bool enabled)
{
    {
        QMutexLocker locker(&m_state_mutex);
        m_enabled = enabled;

        if (!m_enabled)
        {
            m_background_music_player->stop();
        }
    }

    AudioCommand command;
    command.type = AudioCommandType::SetEnabled;
    command.enabled = enabled;
    postCommand(command);
}

bool QtAudioService::isEnabled() const
{
    QMutexLocker locker(&m_state_mutex);
    return m_enabled;
}

void QtAudioService::setEffectVolume(float volume)
{
    {
        QMutexLocker locker(&m_state_mutex);
        if (volume < 0.0f)
        {
            volume = 0.0f;
        }
        else if (volume > 1.0f)
        {
            volume = 1.0f;
        }

        m_effect_volume = volume;
    }

    AudioCommand command;
    command.type = AudioCommandType::SetEffectVolume;
    command.volume = volume;
    postCommand(command);
}

float QtAudioService::effectVolume() const
{
    QMutexLocker locker(&m_state_mutex);
    return m_effect_volume;
}

void QtAudioService::setBackgroundMusicVolume(float volume)
{
    {
        QMutexLocker locker(&m_state_mutex);
        if (volume < 0.0f)
        {
            volume = 0.0f;
        }
        else if (volume > 1.0f)
        {
            volume = 1.0f;
        }

        m_background_music_volume = volume;
        m_background_music_player->setVolume(static_cast<int>(volume * 100));
    }
}

float QtAudioService::backgroundMusicVolume() const
{
    QMutexLocker locker(&m_state_mutex);
    return m_background_music_volume;
}

void QtAudioService::postCommand(const AudioCommand& command)
{
    if (m_worker == nullptr)
    {
        return;
    }

    QMetaObject::invokeMethod(
        m_worker,
        "handleCommand",
        Qt::QueuedConnection,
        Q_ARG(AudioCommand, command));
}

void QtAudioService::onMediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia && m_background_music_player != nullptr)
    {
        m_background_music_player->play();
    }
}

QUrl QtAudioService::buildAudioUrl(const QString& source_path)
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

QMediaContent QtAudioService::buildAudioContent(const QString& source_path)
{
    return QMediaContent(buildAudioUrl(source_path));
}
