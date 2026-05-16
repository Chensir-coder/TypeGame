#ifndef __TYPEGAME_TELEMETRY_MESSAGE_H__
#define __TYPEGAME_TELEMETRY_MESSAGE_H__

#include <QString>
#include <QJsonObject>
#include <QtGlobal>

/* ------------------------------------------------------------------
 // 文件名     : telemetry_message.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 遥测消息结构定义
------------------------------------------------------------------ */

struct TelemetryMessage
{
    QString event_name;      // 例如 "apple_hit"
    QString category;        // 例如 "saveapple"
    qint64 timestamp_ms = 0; // 记录时间
    QJsonObject payload;     // 结构化字段
};


#endif // __TYPEGAME_TELEMETRY_MESSAGE_H__
