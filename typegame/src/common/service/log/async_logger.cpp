/* ------------------------------------------------------------------
 // 文件名     : async_logger.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 异步日志记录器实现
------------------------------------------------------------------ */

#include "async_logger.h"

#include <QDateTime>
#include <QDir>
#include <QTextStream>

AsyncLogger::AsyncLogger(const QString& file_path, ILogFormatter* formatter)
    : m_file_path(file_path)
    , m_formatter(formatter)
{
}

AsyncLogger::~AsyncLogger()
{
    stopLogger();
}

void AsyncLogger::stopLogger()
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


void AsyncLogger::start()
{

    QMutexLocker locker(&m_mutex);

    if (m_running)
    {
        return;
    }

    // 确保目录存在
    QFileInfo file_info(m_file_path);
    QDir dir = file_info.dir();
    if (!dir.exists())
    {
        dir.mkpath(".");
    }

    m_file.setFileName(m_file_path);
    if (!m_file.open(QIODevice::Append | QIODevice::Text))
    {
        // 这里你可以加 fallback 或 debug 输出
        return;
    }

    m_running = true;

    // 启动后台线程
    m_thread = std::thread(&AsyncLogger::worker_loop, this);
}

void AsyncLogger::stop()
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

void AsyncLogger::log(const LogMessage& message)
{
    QMutexLocker locker(&m_mutex);

    if (!m_running)
    {
        return;
    }

    m_queue.enqueue(message); // 入队新消息
    m_condition.wakeOne(); // 唤醒 worker_loop 线程处理日志
}

void AsyncLogger::worker_loop()
{
    QTextStream stream(&m_file);

    while (true)
    {
        m_mutex.lock();

        while (m_queue.isEmpty() && m_running)
        {
            m_condition.wait(&m_mutex);
        }

        // 如果已经 stop 且没有消息了 → 退出线程
        if (!m_running && m_queue.isEmpty())
        {
            m_mutex.unlock();
            break;
        }

        LogMessage message = m_queue.dequeue();
        m_mutex.unlock();

        // 格式化日志
        QString line;
        if (m_formatter != nullptr)
        {
            line = m_formatter->format(message);
        }
        else
        {
            // fallback（理论上不该发生）
            line = message.text;
        }

        stream << line << "\n";
        stream.flush();   // 保证实时写入（作业阶段推荐开启）
    }
}
