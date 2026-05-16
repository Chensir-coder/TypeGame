/* ------------------------------------------------------------------
 // 文件名     : json_formatter.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 遥测 JSON 格式化器实现
------------------------------------------------------------------ */

#include "json_formatter.h"

#include <QJsonDocument>
#include <QJsonObject>

QString JsonFormatter::format(const TelemetryMessage& message) const
{
    QJsonObject root;
    root["event_name"] = message.event_name;
    root["category"] = message.category;
    root["timestamp_ms"] = static_cast<qint64>(message.timestamp_ms);
    root["payload"] = message.payload;

    return QString::fromUtf8(QJsonDocument(root).toJson(QJsonDocument::Compact));
}
