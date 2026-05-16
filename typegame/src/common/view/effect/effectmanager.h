#ifndef __TYPEGAME_EFFECTMANAGER_H__
#define __TYPEGAME_EFFECTMANAGER_H__

#include "effectitem.h"

#include <QVector>
#include <QPainter>
#include <memory>
#include <algorithm>

/* ------------------------------------------------------------------
 // 文件名     : effectmanager.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : effectmanager 头文件声明
------------------------------------------------------------------ */

class EffectManager
{
public:
    void addEffect(const std::shared_ptr<EffectItem>& effect)
    {
        if (effect) {
            m_effects.push_back(effect);
        }
    }

    void update(qreal dt)
    {
        for (auto& effect : m_effects) {
            if (effect) {
                effect->update(dt);
            }
        }

        m_effects.erase(
            std::remove_if(m_effects.begin(), m_effects.end(),
                           [](const std::shared_ptr<EffectItem>& e) {
                               return !e || !e->isAlive();
                           }),
            m_effects.end());
    }

    void paint(QPainter* painter,
               qreal viewScale,
               const QPointF& cameraOffset = QPointF(0.0, 0.0)) const
    {
        if (!painter) {
            return;
        }

        for (const auto& effect : m_effects) {
            if (effect) {
                effect->paint(painter, viewScale, cameraOffset);
            }
        }
    }

    void clear()
    {
        m_effects.clear();
    }

    bool isEmpty() const
    {
        return m_effects.isEmpty();
    }

    int size() const
    {
        return m_effects.size();
    }

private:
    QVector<std::shared_ptr<EffectItem>> m_effects;
};

#endif // __TYPEGAME_EFFECTMANAGER_H__
