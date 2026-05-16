#include "enemy_entity.h"

#include <QtMath>

EnemyEntity::EnemyEntity()
    : TypingTargetEntity()
{
}

bool EnemyEntity::isLocked() const
{
    return m_locked;
}

void EnemyEntity::setLocked(bool locked)
{
    m_locked = locked;
}

EnemyEntity::Trajectory EnemyEntity::trajectory() const
{
    return m_trajectory;
}

void EnemyEntity::setTrajectory(Trajectory trajectory)
{
    m_trajectory = trajectory;
}

EnemyEntity::Variant EnemyEntity::variant() const
{
    return m_variant;
}

void EnemyEntity::setVariant(Variant variant)
{
    m_variant = variant;
}

qreal EnemyEntity::startX() const
{
    return m_startX;
}

qreal EnemyEntity::sineAmplitude() const
{
    return m_sineAmplitude;
}

qreal EnemyEntity::sinePhase() const
{
    return m_sinePhase;
}

qreal EnemyEntity::sineAngularSpeed() const
{
    return m_sineAngularSpeed;
}

void EnemyEntity::configureStraight(qreal startX, qreal verticalSpeed)
{
    m_trajectory = Trajectory::Straight;
    m_startX = startX;
    m_verticalSpeed = qMax<qreal>(0.0, verticalSpeed);
    setVelocity(QPointF(0.0, m_verticalSpeed));
}

void EnemyEntity::configureSine(qreal startX,
                                qreal verticalSpeed,
                                qreal amplitude,
                                qreal angularSpeed,
                                qreal phaseOffset)
{
    m_trajectory = Trajectory::Sine;
    m_startX = startX;
    m_verticalSpeed = qMax<qreal>(0.0, verticalSpeed);
    m_sineAmplitude = qMax<qreal>(0.0, amplitude);
    m_sineAngularSpeed = qMax<qreal>(0.0, angularSpeed);
    m_sinePhase = phaseOffset;
    m_elapsedSeconds = 0.0;
    setVelocity(QPointF(0.0, m_verticalSpeed));
}

void EnemyEntity::configureBounce(qreal startX,
                                  qreal verticalSpeed,
                                  qreal horizontalSpeed)
{
    m_trajectory = Trajectory::Bounce;
    m_startX = startX;
    m_verticalSpeed = qMax<qreal>(0.0, verticalSpeed);
    m_horizontalSpeed = horizontalSpeed;
    setVelocity(QPointF(m_horizontalSpeed, m_verticalSpeed));
}

bool EnemyEntity::isBelowScene() const
{
    const QRectF scene = sceneRect();
    if (scene.isNull() || !scene.isValid()) {
        return false;
    }

    return position().y() > scene.bottom();
}

void EnemyEntity::onUpdate(qreal deltaTime)
{
    if (deltaTime <= 0.0) {
        return;
    }

    m_elapsedSeconds += deltaTime;

    switch (m_trajectory) {
    case Trajectory::Straight:
        updateStraight(deltaTime);
        break;
    case Trajectory::Sine:
        updateSine(deltaTime);
        break;
    case Trajectory::Bounce:
        updateBounce(deltaTime);
        break;
    }

    /*
     * 飞机大战的退场线在 1.0，敌机正常落到底部即触发退场。
     * 仍然保留基类对 exitLine 的判定。
     */
    TypingTargetEntity::onUpdate(deltaTime);
}

void EnemyEntity::onReset()
{
    TypingTargetEntity::onReset();

    m_locked = false;
    m_trajectory = Trajectory::Straight;
    m_variant = Variant::Type0;

    m_startX = 0.0;
    m_verticalSpeed = 0.0;

    m_sineAmplitude = 0.0;
    m_sineAngularSpeed = 0.0;
    m_sinePhase = 0.0;
    m_elapsedSeconds = 0.0;

    m_horizontalSpeed = 0.0;
}

void EnemyEntity::updateStraight(qreal deltaTime)
{
    Q_UNUSED(deltaTime);

    QPointF p = position();
    p.setX(m_startX);
    setPosition(p);

    clampInsideHorizontalBounds();
}

void EnemyEntity::updateSine(qreal deltaTime)
{
    Q_UNUSED(deltaTime);

    QPointF p = position();
    const qreal offset =
        m_sineAmplitude * std::sin(m_sineAngularSpeed * m_elapsedSeconds + m_sinePhase);
    p.setX(m_startX + offset);
    setPosition(p);

    clampInsideHorizontalBounds();
}

void EnemyEntity::updateBounce(qreal deltaTime)
{
    Q_UNUSED(deltaTime);

    if (reflectIfHittingHorizontalBound()) {
        QPointF v = velocity();
        v.setX(m_horizontalSpeed);
        setVelocity(v);
    }

    clampInsideHorizontalBounds();
}

void EnemyEntity::clampInsideHorizontalBounds()
{
    const QRectF scene = sceneRect();
    if (scene.isNull() || !scene.isValid()) {
        return;
    }

    QPointF p = position();
    const qreal w = size().width();

    if (p.x() < scene.left()) {
        p.setX(scene.left());
    }

    if (p.x() + w > scene.right()) {
        p.setX(scene.right() - w);
    }

    setPosition(p);
}

bool EnemyEntity::reflectIfHittingHorizontalBound()
{
    const QRectF scene = sceneRect();
    if (scene.isNull() || !scene.isValid()) {
        return false;
    }

    const qreal w = size().width();
    QPointF p = position();
    bool reflected = false;

    if (p.x() <= scene.left() && m_horizontalSpeed < 0.0) {
        m_horizontalSpeed = -m_horizontalSpeed;
        p.setX(scene.left());
        reflected = true;
    } else if (p.x() + w >= scene.right() && m_horizontalSpeed > 0.0) {
        m_horizontalSpeed = -m_horizontalSpeed;
        p.setX(scene.right() - w);
        reflected = true;
    }

    if (reflected) {
        setPosition(p);
    }

    return reflected;
}
