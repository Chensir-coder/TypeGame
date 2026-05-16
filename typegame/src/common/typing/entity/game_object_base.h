#ifndef __TYPEGAME_GAME_OBJECT_BASE_H__
#define __TYPEGAME_GAME_OBJECT_BASE_H__

#include <QPointF>
#include <QRectF>
#include <QSizeF>

/* ------------------------------------------------------------------
 // 文件名     : game_object_base.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : game_object_base 头文件声明
------------------------------------------------------------------ */

class GameObjectBase
{
public:
    GameObjectBase();
    explicit GameObjectBase(int id);
    virtual ~GameObjectBase() = default;

    GameObjectBase(const GameObjectBase&) = delete;
    GameObjectBase& operator=(const GameObjectBase&) = delete;

    GameObjectBase(GameObjectBase&&) noexcept = default;
    GameObjectBase& operator=(GameObjectBase&&) noexcept = default;

    virtual void update(qreal deltaTime);
    virtual void reset();

    int id() const;
    void setId(int id);

    bool isAlive() const;
    void setAlive(bool alive);
    void kill();

    QPointF position() const;
    void setPosition(const QPointF& position);

    QPointF velocity() const;
    void setVelocity(const QPointF& velocity);

    QSizeF size() const;
    void setSize(const QSizeF& size);

    QRectF boundingRect() const;

protected:
    virtual void updateMovement(qreal deltaTime);
    virtual void onUpdate(qreal deltaTime);
    virtual void onReset();

protected:
    int m_id = 0;
    bool m_alive = true;
    QPointF m_position = QPointF(0.0, 0.0);
    QPointF m_velocity = QPointF(0.0, 0.0);
    QSizeF m_size = QSizeF(0.0, 0.0);
};

#endif // __TYPEGAME_GAME_OBJECT_BASE_H__
