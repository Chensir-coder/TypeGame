#ifndef __TYPEGAME_ILOGGER_H__
#define __TYPEGAME_ILOGGER_H__

#include "log_message.h"

/* ------------------------------------------------------------------
 // 文件名     : ilogger.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 日志接口定义
------------------------------------------------------------------ */

class ILogger
{
public:
    virtual ~ILogger() = default;

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void log(const LogMessage& message) = 0;
};

#endif // __TYPEGAME_ILOGGER_H__
