#ifndef __TYPEGAME_ASYNC_LOGGER_H__
#define __TYPEGAME_ASYNC_LOGGER_H__

#include <thread>

#include <QFile>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>

#include "ilogger.h"
#include "ilog_formatter.h"

/* ------------------------------------------------------------------
 // 文件名     : async_logger.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 异步日志记录器声明
------------------------------------------------------------------ */

class AsyncLogger : public ILogger
{
public:
    AsyncLogger(const QString& file_path, ILogFormatter* formatter);
    ~AsyncLogger();

    void start() override;
    void stop() override;
    void log(const LogMessage& message) override;


private:
    void worker_loop();
    void stopLogger();

private:
    QString m_file_path;
    ILogFormatter* m_formatter = nullptr;

    bool m_running = false;
    std::thread m_thread;

    QFile m_file;
    QMutex m_mutex;
    QWaitCondition m_condition;
    QQueue<LogMessage> m_queue;
};

#endif // __TYPEGAME_ASYNC_LOGGER_H__
