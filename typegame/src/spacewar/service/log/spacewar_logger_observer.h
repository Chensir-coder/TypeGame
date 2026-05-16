#ifndef __TYPEGAME_SPACEWAR_LOGGER_OBSERVER_H__
#define __TYPEGAME_SPACEWAR_LOGGER_OBSERVER_H__

#include "common/core/igame_observer.h"
#include "common/service/log/ilogger.h"

/* ------------------------------------------------------------------
 // 文件名     : spacewar_logger_observer.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 飞机大战日志观察者
------------------------------------------------------------------ */

class SpaceWarLoggerObserver : public IGameObserver
{
public:
    explicit SpaceWarLoggerObserver(ILogger* logger);

    void onGameEvent(const GameEvent& event) override;

private:
    ILogger* m_logger = nullptr;
};
#endif // __TYPEGAME_SPACEWAR_LOGGER_OBSERVER_H__
