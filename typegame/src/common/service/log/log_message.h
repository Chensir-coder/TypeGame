#ifndef __TYPEGAME_LOG_MESSAGE_H__
#define __TYPEGAME_LOG_MESSAGE_H__

#include <QString>
#include <QtGlobal>
#include "log_level.h"

/* ------------------------------------------------------------------
 // 文件名     : log_message.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 日志消息结构定义
------------------------------------------------------------------ */

struct LogMessage
{
    LogLevel level = LogLevel::Info;
    QString category;      // 例如 "saveapple"
    QString text;          // 例如 "Apple hit: letter=A score=10"
    qint64 timestamp_ms = 0;
};

#endif // __TYPEGAME_LOG_MESSAGE_H__
