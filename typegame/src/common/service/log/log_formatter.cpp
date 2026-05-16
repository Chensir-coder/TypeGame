/* ------------------------------------------------------------------
 // 文件名     : log_formatter.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 日志格式化器实现
------------------------------------------------------------------ */

#include "log_formatter.h"
#include <QDateTime>

QString PlainTextLogFormatter::format(const LogMessage& message) const
{
    QString level_text;
    switch (message.level)
    {
        case LogLevel::Debug:   level_text = "DEBUG"; break;
        case LogLevel::Info:    level_text = "INFO";  break;
        case LogLevel::Warning: level_text = "WARN";  break;
        case LogLevel::Error:   level_text = "ERROR"; break;
        default:                level_text = "INFO";  break;
    }

    const QString time_text =
        QDateTime::fromMSecsSinceEpoch(message.timestamp_ms).toString("yyyy-MM-dd HH:mm:ss.zzz");

    return QString("[%1][%2][%3] %4")
        .arg(time_text)
        .arg(level_text)
        .arg(message.category)
        .arg(message.text);
}
