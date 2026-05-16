#include "bullet_entity.h"

#include <QtMath>

BulletEntity::BulletEntity()
    : GameEntity()
{
}

BulletEntity::BulletEntity(int id)
    : GameEntity(id)
{
}

void BulletEntity::update(qreal deltaTime)
{
    GameEntity::update(deltaTime);
}

void BulletEntity::reset()
{
    GameEntity::reset();
}

EnemyEntity* BulletEntity::target() const
{
    return m_target;
}

void BulletEntity::setTarget(EnemyEntity* target)
{
    m_target = target;
}

qreal BulletEntity::speed() const
{
    return m_speed;
}

void BulletEntity::setSpeed(qreal speed)
{
    m_speed = qMax<qreal>(0.0, speed);
}

qreal BulletEntity::hitRadius() const
{
    return m_hitRadius;
}

void BulletEntity::setHitRadius(qreal radius)
{
    m_hitRadius = qMax<qreal>(0.0, radius);
}

bool BulletEntity::hasHitTarget() const
{
    return m_hasHitTarget;
}

void BulletEntity::onUpdate(qreal deltaTime)
{
    if (!isTargetValid()) {
        destroySelf();
        return;
    }

    moveToTarget(deltaTime);
}

void BulletEntity::onReset()
{
    GameEntity::onReset();

    m_target = nullptr;
    m_speed = 500.0;
    m_hitRadius = 20.0;
    m_hasHitTarget = false;
}

QPointF BulletEntity::centerPosition() const
{
    const QRectF rect = boundingRect();
    return rect.center();
}

QPointF BulletEntity::targetCenterPosition() const
{
    if (!m_target) {
        return QPointF();
    }

    return m_target->boundingRect().center();
}

bool BulletEntity::isTargetValid() const
{
    return m_target
           && m_target->isAlive()
           && m_target->isActive()
           && !m_target->isDestroyed();
}

bool BulletEntity::shouldHitTarget(qreal distance) const
{
    if (!m_target) {
        return false;
    }

    if (distance <= m_hitRadius) {
        return true;
    }

    return boundingRect().intersects(m_target->boundingRect());
}

void BulletEntity::moveToTarget(qreal deltaTime)
{
    if (deltaTime <= 0.0) {
        return;
    }

    const QPointF bulletCenter = centerPosition();
    const QPointF enemyCenter = targetCenterPosition();

    const QPointF direction(enemyCenter.x() - bulletCenter.x(),
                            enemyCenter.y() - bulletCenter.y());

    const qreal distance = qSqrt(direction.x() * direction.x()
                                 + direction.y() * direction.y());

    if (shouldHitTarget(distance)) {
        hitTarget();
        return;
    }

    if (distance <= 0.001) {
        hitTarget();
        return;
    }

    const QPointF normalizedDirection(direction.x() / distance,
                                      direction.y() / distance);

    const qreal moveDistance = m_speed * deltaTime;

    if (moveDistance >= distance) {
        hitTarget();
        return;
    }

    const QPointF newPosition = position()
                                + QPointF(normalizedDirection.x() * moveDistance,
                                          normalizedDirection.y() * moveDistance);

    setPosition(newPosition);
}

void BulletEntity::hitTarget()
{
    if (!isTargetValid()) {
        destroySelf();
        return;
    }

    m_hasHitTarget = true;
    m_target->markHitByBullet();

    destroySelf();
}

void BulletEntity::destroySelf()
{
    kill();
    setActive(false);
    setVisible(false);
}
