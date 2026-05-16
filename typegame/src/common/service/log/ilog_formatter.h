#ifndef __TYPEGAME_ILOG_FORMATTER_H__
#define __TYPEGAME_ILOG_FORMATTER_H__

#include <QString>
#include "log_message.h"

/* ------------------------------------------------------------------
 // 文件名     : ilog_formatter.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 日志格式化器接口定义
------------------------------------------------------------------ */

class ILogFormatter
{
public:
    virtual ~ILogFormatter() = default;
    virtual QString format(const LogMessage& message) const = 0;
};

#endif // __TYPEGAME_ILOG_FORMATTER_H__
