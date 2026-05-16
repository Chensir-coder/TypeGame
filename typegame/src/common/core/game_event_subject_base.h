#ifndef __TYPEGAME_GAME_EVENT_SUBJECT_BASE_H__
#define __TYPEGAME_GAME_EVENT_SUBJECT_BASE_H__

#include <vector>
#include <algorithm>
#include "igame_observer.h"

/* ------------------------------------------------------------------
 // 文件名     : game_event_subject_base.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 通用游戏事件主题基类
------------------------------------------------------------------ */

/**
 * @brief 通用游戏事件主题基类。
 *
 * 职责：
 * 1. 维护观察者列表。
 * 2. 提供注册、移除、通知观察者的通用能力。
 * 3. 作为控制器、游戏主对象等事件源的公共基类。
 *
 * 说明：
 * 1. 当前实现采用轻量级容器管理观察者，不负责观察者对象生命周期。
 * 2. 观察者由外部模块创建和销毁，主题类仅保存非拥有型指针。
 * 3. 为避免重复订阅，addObserver() 内部会进行去重检查。
 * 4. 当前版本不考虑多线程并发访问，如后续需要可再扩展线程安全保护。
 */
class GameEventSubjectBase
{
public:
    explicit GameEventSubjectBase() = default;
    virtual ~GameEventSubjectBase() = default;

    /**
     * @brief 添加观察者。
     *
     * @param observer 观察者指针
     */
    void addObserver(IGameObserver* observer);

    /**
     * @brief 移除观察者。
     *
     * @param observer 观察者指针
     */
    void removeObserver(IGameObserver* observer);

    /**
     * @brief 清空所有观察者。
     *
     * 说明：
     * 1. 仅清空指针记录，不负责释放对象。
     */
    void clearObservers();

protected:
    /**
     * @brief 通知所有观察者。
     *
     * @param event 游戏事件
     */
    void notifyObservers(const GameEvent& event) const;

private:
    std::vector<IGameObserver*> m_observers; ///< 观察者列表（非拥有型指针）
};

inline void GameEventSubjectBase::addObserver(IGameObserver* observer)
{
    if (observer == nullptr)
    {
        return;
    }

    const auto iterator = std::find(m_observers.begin(), m_observers.end(), observer);
    if (iterator != m_observers.end())
    {
        return;
    }

    m_observers.push_back(observer);
}

inline void GameEventSubjectBase::removeObserver(IGameObserver* observer)
{
    if (observer == nullptr)
    {
        return;
    }

    m_observers.erase(
        std::remove(m_observers.begin(), m_observers.end(), observer),
        m_observers.end());
}

inline void GameEventSubjectBase::clearObservers()
{
    m_observers.clear();
}

inline void GameEventSubjectBase::notifyObservers(const GameEvent& event) const
{
    for (IGameObserver* observer : m_observers)
    {
        if (observer != nullptr)
        {
            observer->onGameEvent(event);
        }
    }
}

#endif // __TYPEGAME_GAME_EVENT_SUBJECT_BASE_H__
