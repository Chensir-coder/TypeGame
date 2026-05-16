#ifndef __TYPEGAME_SPACEWAR_TELEMETRY_OBSERVER_H__
#define __TYPEGAME_SPACEWAR_TELEMETRY_OBSERVER_H__

#include "common/core/igame_observer.h"
#include "common/service/telemetry/itelemetry_writer.h"

/* ------------------------------------------------------------------
 // 文件名     : spacewar_telemetry_observer.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 飞机大战遥测观察者
------------------------------------------------------------------ */

class SpaceWarTelemetryObserver : public IGameObserver
{
public:
    explicit SpaceWarTelemetryObserver(ITelemetryWriter* writer);

    void onGameEvent(const GameEvent& event) override;

private:
    TelemetryMessage buildMessage(const QString& event_name,
                                  const GameEvent& event) const;
    void fillCommonFields(QJsonObject& payload, const GameEvent& event) const;

private:
    ITelemetryWriter* m_writer = nullptr;
};
#endif // __TYPEGAME_SPACEWAR_TELEMETRY_OBSERVER_H__
