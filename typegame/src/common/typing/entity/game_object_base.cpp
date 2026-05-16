#include "game_object_base.h"

GameObjectBase::GameObjectBase()
{
}

GameObjectBase::GameObjectBase(int id)
    : m_id(id)
{
}

void GameObjectBase::update(qreal deltaTime)
{
    if (!m_alive) {
        return;
    }

    if (deltaTime <= 0.0) {
        return;
    }

    updateMovement(deltaTime);
    onUpdate(deltaTime);
}

void GameObjectBase::reset()
{
    m_alive = true;
    m_position = QPointF(0.0, 0.0);
    m_velocity = QPointF(0.0, 0.0);
    m_size = QSizeF(0.0, 0.0);

    onReset();
}

int GameObjectBase::id() const
{
    return m_id;
}

void GameObjectBase::setId(int id)
{
    m_id = id;
}

bool GameObjectBase::isAlive() const
{
    return m_alive;
}

void GameObjectBase::setAlive(bool alive)
{
    m_alive = alive;
}

void GameObjectBase::kill()
{
    m_alive = false;
}

QPointF GameObjectBase::position() const
{
    return m_position;
}

void GameObjectBase::setPosition(const QPointF& position)
{
    m_position = position;
}

QPointF GameObjectBase::velocity() const
{
    return m_velocity;
}

void GameObjectBase::setVelocity(const QPointF& velocity)
{
    m_velocity = velocity;
}

QSizeF GameObjectBase::size() const
{
    return m_size;
}

void GameObjectBase::setSize(const QSizeF& size)
{
    m_size = size;
}

QRectF GameObjectBase::boundingRect() const
{
    return QRectF(m_position, m_size);
}

void GameObjectBase::updateMovement(qreal deltaTime)
{
    m_position += QPointF(m_velocity.x() * deltaTime,
                          m_velocity.y() * deltaTime);
}

void GameObjectBase::onUpdate(qreal deltaTime)
{
    Q_UNUSED(deltaTime);
}

void GameObjectBase::onReset()
{
}
