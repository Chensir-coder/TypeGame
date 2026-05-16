#ifndef __TYPEGAME_LOG_FORMATTER_H__
#define __TYPEGAME_LOG_FORMATTER_H__

#include "log_message.h"
#include "ilog_formatter.h"

/* ------------------------------------------------------------------
 // 文件名     : log_formatter.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 日志格式化器实现声明
------------------------------------------------------------------ */

/// 简单的纯文本日志格式化器实现
class PlainTextLogFormatter : public ILogFormatter
{
public:
    QString format(const LogMessage& message) const override;
};


#endif // __TYPEGAME_LOG_FORMATTER_H__
