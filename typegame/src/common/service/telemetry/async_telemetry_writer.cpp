/* ------------------------------------------------------------------
 // 文件名     : async_telemetry_writer.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 异步遥测写入器实现
------------------------------------------------------------------ */

#include "async_telemetry_writer.h"

#include <QDir>
#include <QFileInfo>
#include <QTextStream>

AsyncTelemetryWriter::AsyncTelemetryWriter(const QString& file_path)
    : m_file_path(file_path)
{
}

AsyncTelemetryWriter::~AsyncTelemetryWriter()
{
    stopTelemetryWriter();
}

void AsyncTelemetryWriter::stopTelemetryWriter()
{
    {
        QMutexLocker locker(&m_mutex);

        if (!m_running)
        {
            return;
        }

        m_running = false;
        m_condition.wakeAll();
    }

    if (m_thread.joinable())
    {
        m_thread.join();
    }

    if (m_file.isOpen())
    {
        m_file.flush();
        m_file.close();
    }
}

void AsyncTelemetryWriter::start()
{
    QMutexLocker locker(&m_mutex);

    if (m_running)
    {
        return;
    }

    // 确保输出目录存在
    QFileInfo file_info(m_file_path);
    QDir dir = file_info.dir();
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    m_file.setFileName(m_file_path);
    if (!m_file.open(QIODevice::Append | QIODevice::Text))
    {
        return;
    }

    m_running = true;
    m_thread = std::thread(&AsyncTelemetryWriter::worker_loop, this);
}

void AsyncTelemetryWriter::stop()
{
    {
        QMutexLocker locker(&m_mutex);

        if (!m_running)
        {
            return;
        }

        m_running = false;
        m_condition.wakeAll();
    }

    if (m_thread.joinable())
    {
        m_thread.join();
    }

    if (m_file.isOpen())
    {
        m_file.flush();
        m_file.close();
    }
}

void AsyncTelemetryWriter::write(const TelemetryMessage& message)
{
    QMutexLocker locker(&m_mutex);

    if (!m_running)
    {
        return;
    }

    m_queue.enqueue(message);
    m_condition.wakeOne();
}

void AsyncTelemetryWriter::worker_loop()
{
    QTextStream stream(&m_file);

    while (true)
    {
        m_mutex.lock();

        while (m_queue.isEmpty() && m_running)
        {
            m_condition.wait(&m_mutex);
        }

        // stop 后把队列剩余数据写完再退出
        if (!m_running && m_queue.isEmpty())
        {
            m_mutex.unlock();
            break;
        }

        TelemetryMessage message = m_queue.dequeue();
        m_mutex.unlock();

        const QString line = m_formatter.format(message);

        stream << line << "\n";
        stream.flush();
    }
}
