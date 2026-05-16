#ifndef __TYPEGAME_IGAME_OBSERVER_H__
#define __TYPEGAME_IGAME_OBSERVER_H__

#include "common/core/game_event.h"

/* ------------------------------------------------------------------
 // 文件名     : igame_observer.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 通用游戏观察者接口
------------------------------------------------------------------ */

/**
 * @brief 通用游戏观察者接口。
 *
 * 职责：
 * 1. 为所有需要监听游戏事件的模块提供统一接口。
 * 2. 使 view、audio、log、telemetry 等模块可以通过观察者模式解耦接入。
 *
 * 适用对象：
 * 1. 状态面板、场景界面等 UI 模块。
 * 2. 音效服务适配层。
 * 3. 日志记录模块。
 * 4. 埋点统计模块。
 *
 * 说明：
 * 1. 观察者只关注事件，不直接依赖具体业务模块的内部实现。
 * 2. 统一使用 GameEvent 作为事件载体，避免事件接口碎片化。
 */
class IGameObserver
{
public:
    virtual ~IGameObserver() = default;

    /**
     * @brief 接收游戏事件通知。
     *
     * @param event 游戏事件
     */
    virtual void onGameEvent(const GameEvent& event) = 0;
};

#endif // __TYPEGAME_IGAME_OBSERVER_H__
