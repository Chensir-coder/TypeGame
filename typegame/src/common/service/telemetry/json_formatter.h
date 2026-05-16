#ifndef __TYPEGAME_JSON_FORMATTER_H__
#define __TYPEGAME_JSON_FORMATTER_H__

#include <QString>
#include "telemetry_message.h"

/* ------------------------------------------------------------------
 // 文件名     : json_formatter.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-19
 // 功能描述   : 遥测 JSON 格式化器声明
------------------------------------------------------------------ */

class JsonFormatter
{
public:
    QString format(const TelemetryMessage& message) const;
};

#endif // __TYPEGAME_JSON_FORMATTER_H__
