#ifndef __TYPEGAME_ASYNC_TELEMETRY_WRITER_H__
#define __TYPEGAME_ASYNC_TELEMETRY_WRITER_H__

#include <QFile>
#include <QMutex>
#include <QQueue>
#include <QWaitCondition>

#include <thread>

#include "itelemetry_writer.h"
#include "json_formatter.h"

/* ------------------------------------------------------------------
 // 文件名     : async_telemetry_writer.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 异步遥测写入器声明
------------------------------------------------------------------ */

class AsyncTelemetryWriter : public ITelemetryWriter
{
public:
    explicit AsyncTelemetryWriter(const QString& file_path);
    ~AsyncTelemetryWriter();

    void start() override;
    void stop() override;
    void write(const TelemetryMessage& message) override;

private:
    void worker_loop();
    void stopTelemetryWriter();

private:
    QString m_file_path;
    JsonFormatter m_formatter;

    bool m_running = false;
    std::thread m_thread;

    QFile m_file;
    QMutex m_mutex;
    QWaitCondition m_condition;
    QQueue<TelemetryMessage> m_queue;
};

#endif // __TYPEGAME_ASYNC_TELEMETRY_WRITER_H__
