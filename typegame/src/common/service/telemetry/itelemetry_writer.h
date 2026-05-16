#ifndef __TYPEGAME_ITELEMETRY_WRITER_H__
#define __TYPEGAME_ITELEMETRY_WRITER_H__

#include "telemetry_message.h"

/* ------------------------------------------------------------------
 // 文件名     : itelemetry_writer.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 遥测写入器接口定义
------------------------------------------------------------------ */

class ITelemetryWriter
{
public:
    virtual ~ITelemetryWriter() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void write(const TelemetryMessage& message) = 0;
};

#endif // __TYPEGAME_ITELEMETRY_WRITER_H__
