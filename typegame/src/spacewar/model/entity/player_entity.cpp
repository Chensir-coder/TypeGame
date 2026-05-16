#include "player_entity.h"

#include "common/typing/typing_game_config.h"

#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QRadialGradient>
#include <QString>
#include <QtGlobal>

#include <cmath>

namespace
{
constexpr int kPlayerDefaultMaxHealth = 18;
constexpr qreal kCollisionShieldDurationSec = 1.0;
constexpr qreal kHealthDeltaFxDurationSec = 1.0;
constexpr qreal kPi = 3.14159265358979323846;
}

PlayerEntity::PlayerEntity()
    : GameEntity()
{
    m_health = kPlayerDefaultMaxHealth;
    m_maxHealth = kPlayerDefaultMaxHealth;
}

PlayerEntity::PlayerEntity(int id)
    : GameEntity(id)
{
    m_health = kPlayerDefaultMaxHealth;
    m_maxHealth = kPlayerDefaultMaxHealth;
}

void PlayerEntity::update(qreal deltaTime)
{
    updateVelocityByDirection();
    GameEntity::update(deltaTime);
}

void PlayerEntity::reset()
{
    GameEntity::reset();
}

int PlayerEntity::health() const
{
    return m_health;
}

void PlayerEntity::setHealth(int health)
{
    m_health = qBound(0, health, m_maxHealth);

    if (m_health <= 0) {
        kill();
        setActive(false);
    }
}

int PlayerEntity::maxHealth() const
{
    return m_maxHealth;
}

void PlayerEntity::setMaxHealth(int maxHealth)
{
    m_maxHealth = qMax(1, maxHealth);
    m_health = qMin(m_health, m_maxHealth);
}

bool PlayerEntity::isDead() const
{
    return m_health <= 0 || !isAlive();
}

void PlayerEntity::damage(int value)
{
    if (value <= 0) {
        return;
    }

    const int beforeHealth = m_health;
    setHealth(m_health - value);
    triggerHealthDeltaFx(m_health - beforeHealth);
}

void PlayerEntity::heal(int value)
{
    if (value <= 0) {
        return;
    }

    const int beforeHealth = m_health;
    setHealth(m_health + value);
    triggerHealthDeltaFx(m_health - beforeHealth);
}

qreal PlayerEntity::moveSpeed() const
{
    return m_moveSpeed;
}

void PlayerEntity::setMoveSpeed(qreal speed)
{
    m_moveSpeed = qMax<qreal>(0.0, speed);
}

PlayerEntity::MoveDirection PlayerEntity::moveDirection() const
{
    return m_moveDirection;
}

void PlayerEntity::setMoveDirection(MoveDirection direction)
{
    m_moveDirection = direction;

    /*
     * 兼容旧调用：按 Left/Right 设置时同步 horizontalAxis；其余方向同步对应轴。
     */
    switch (direction) {
    case MoveDirection::Left:
        m_horizontalAxis = -1;
        m_verticalAxis = 0;
        break;
    case MoveDirection::Right:
        m_horizontalAxis = 1;
        m_verticalAxis = 0;
        break;
    case MoveDirection::Up:
        m_horizontalAxis = 0;
        m_verticalAxis = -1;
        break;
    case MoveDirection::Down:
        m_horizontalAxis = 0;
        m_verticalAxis = 1;
        break;
    case MoveDirection::None:
    default:
        m_horizontalAxis = 0;
        m_verticalAxis = 0;
        break;
    }
}

void PlayerEntity::setHorizontalAxis(int axis)
{
    if (axis < 0) {
        m_horizontalAxis = -1;
    } else if (axis > 0) {
        m_horizontalAxis = 1;
    } else {
        m_horizontalAxis = 0;
    }
}

void PlayerEntity::setVerticalAxis(int axis)
{
    if (axis < 0) {
        m_verticalAxis = -1;
    } else if (axis > 0) {
        m_verticalAxis = 1;
    } else {
        m_verticalAxis = 0;
    }
}

int PlayerEntity::horizontalAxis() const
{
    return m_horizontalAxis;
}

int PlayerEntity::verticalAxis() const
{
    return m_verticalAxis;
}

QRectF PlayerEntity::sceneRect() const
{
    return m_sceneRect;
}

void PlayerEntity::setSceneRect(const QRectF& sceneRect)
{
    m_sceneRect = sceneRect;
}

void PlayerEntity::applyPlaybackFromTypingGameConfig(const TypingGameConfig& cfg)
{
    const int tiles = cfg.playerSpriteSheetCols() * cfg.playerSpriteSheetRows();
    const int frames = qMax(1, tiles);
    const qreal fps = cfg.playerSpriteAnimationFps();
    if (fps > 0.0) {
        setFramePlayback(frames, fps);
    } else if (frames > 1) {
        setFramePlaybackCycle(frames, 1.0);
    } else {
        setFramePlayback(1, 0.0);
    }
}

void PlayerEntity::triggerCollisionShieldFx()
{
    m_collisionShieldRemainSec = kCollisionShieldDurationSec;
    m_collisionShieldElapsedSec = 0.0;
}

void PlayerEntity::paint(QPainter* painter, const QRectF& viewRect, qreal viewScale) const
{
    if (painter == nullptr || viewRect.isEmpty()) {
        return;
    }

    paintCollisionShield(painter, viewRect, viewScale);
    paintHealthDeltaFx(painter, viewRect, viewScale);
}

void PlayerEntity::paintCollisionShield(QPainter* painter,
                                        const QRectF& viewRect,
                                        qreal viewScale) const
{
    if (m_collisionShieldRemainSec <= 0.0) {
        return;
    }

    const qreal t = 1.0 - (m_collisionShieldRemainSec / kCollisionShieldDurationSec);
    const qreal pulse =
        0.5 + 0.5 * std::sin(m_collisionShieldElapsedSec * kPi * 2.0 * 6.0);

    const qreal padPulse = (8.0 + 12.0 * pulse) * viewScale;
    const qreal padBase = 5.0 * viewScale;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    QPen outerPen(QColor(60, 190, 255, qRound(80 + 140 * pulse)));
    outerPen.setWidth(qMax(2, qRound(3.0 * viewScale)));
    painter->setPen(outerPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawEllipse(viewRect.adjusted(-padPulse, -padPulse, padPulse, padPulse));

    QPen innerPen(QColor(175, 235, 255, qRound(90 + 130 * (1.0 - t))));
    innerPen.setWidth(qMax(1, qRound(2.0 * viewScale)));
    painter->setPen(innerPen);
    const qreal innerPad = (3.0 + 5.0 * pulse) * viewScale;
    painter->drawEllipse(viewRect.adjusted(-innerPad, -innerPad, innerPad, innerPad));

    painter->setCompositionMode(QPainter::CompositionMode_Plus);
    QRadialGradient rg(viewRect.center(),
                       qMax(viewRect.width(), viewRect.height()) * 0.7 + padPulse * 0.5);
    rg.setColorAt(0.0, QColor(100, 210, 255, qRound(45 * (1.0 - t) * pulse)));
    rg.setColorAt(0.6, QColor(45, 145, 255, qRound(30 * (1.0 - t))));
    rg.setColorAt(1.0, QColor(0, 80, 200, 0));
    painter->setPen(Qt::NoPen);
    painter->setBrush(rg);
    painter->drawEllipse(viewRect.adjusted(
        -padPulse - padBase,
        -padPulse - padBase,
        padPulse + padBase,
        padPulse + padBase));
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->restore();
}

void PlayerEntity::paintHealthDeltaFx(QPainter* painter,
                                      const QRectF& viewRect,
                                      qreal viewScale) const
{
    if (m_healthDeltaFxRemainSec <= 0.0 || m_healthDeltaValue == 0) {
        return;
    }

    const qreal progress = 1.0 - (m_healthDeltaFxRemainSec / kHealthDeltaFxDurationSec);
    const qreal fadeAlpha = qMax<qreal>(0.0, 1.0 - progress);
    const qreal rise = (8.0 + 34.0 * progress) * viewScale;
    const qreal pulse = 0.5 + 0.5 * std::sin(m_healthDeltaFxElapsedSec * kPi * 2.0 * 9.0);
    const qreal burst = 1.0 + 0.35 * (1.0 - progress) + 0.18 * pulse;
    const qreal wobbleX = std::sin(m_healthDeltaFxElapsedSec * kPi * 2.0 * 5.0) * 2.5 * viewScale;

    const QString text = QStringLiteral("%1%2")
                             .arg((m_healthDeltaValue > 0) ? QStringLiteral("+")
                                                           : QStringLiteral("-"))
                             .arg(qAbs(m_healthDeltaValue));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSizeF(qMax<qreal>(11.0, (16.0 * viewScale + 1.8 * pulse) * burst));

    const QPointF anchor(
        viewRect.right() + (8.0 * viewScale) + wobbleX,
        viewRect.top() - (8.0 * viewScale) - rise);

    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setFont(font);

    QPainterPath path;
    path.addText(anchor, font, text);

    const int alphaBase = qRound(255.0 * fadeAlpha);
    const QColor mainColor = (m_healthDeltaValue > 0) ? QColor(105, 255, 150, alphaBase)
                                                       : QColor(255, 88, 88, alphaBase);
    const QColor innerGlow = (m_healthDeltaValue > 0)
                                 ? QColor(110, 230, 255, qRound(190.0 * fadeAlpha))
                                 : QColor(255, 170, 70, qRound(190.0 * fadeAlpha));
    const QColor outerGlow = (m_healthDeltaValue > 0)
                                 ? QColor(45, 185, 255, qRound(120.0 * fadeAlpha))
                                 : QColor(255, 120, 30, qRound(120.0 * fadeAlpha));
    const QColor highlightColor(255, 255, 255, qRound(210.0 * fadeAlpha));

    // 拖尾残影：用偏移路径制造速度感。
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(255, 255, 255, qRound(45.0 * fadeAlpha)));
    painter->drawPath(path.translated(-2.5 * viewScale, 3.0 * viewScale));
    painter->setBrush(QColor(255, 255, 255, qRound(28.0 * fadeAlpha)));
    painter->drawPath(path.translated(-4.5 * viewScale, 5.0 * viewScale));

    painter->setCompositionMode(QPainter::CompositionMode_Plus);
    painter->setPen(QPen(outerGlow, qMax<qreal>(2.0, 7.0 * viewScale)));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);

    painter->setPen(QPen(innerGlow, qMax<qreal>(1.5, 4.0 * viewScale)));
    painter->drawPath(path);

    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->setPen(QPen(highlightColor, qMax<qreal>(1.0, 1.8 * viewScale)));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);

    painter->setPen(Qt::NoPen);
    painter->setBrush(mainColor);
    painter->drawPath(path);
    painter->restore();
}

QPointF PlayerEntity::gunMuzzlePosition() const
{
    const QRectF rect = boundingRect();

    /*
     * 子弹从飞机精灵中靠上方的位置射出。
     */
    const qreal cx = rect.left() + rect.width() / 2.0;
    const qreal cy = rect.top() + rect.height() * 0.18;
    return QPointF(cx, cy);
}

void PlayerEntity::onUpdate(qreal deltaTime)
{
    if (deltaTime > 0.0 && m_collisionShieldRemainSec > 0.0) {
        m_collisionShieldRemainSec = qMax<qreal>(0.0, m_collisionShieldRemainSec - deltaTime);
        m_collisionShieldElapsedSec += deltaTime;
    }
    if (deltaTime > 0.0 && m_healthDeltaFxRemainSec > 0.0) {
        m_healthDeltaFxRemainSec = qMax<qreal>(0.0, m_healthDeltaFxRemainSec - deltaTime);
        m_healthDeltaFxElapsedSec += deltaTime;
    }

    clampInScene();
}

void PlayerEntity::onReset()
{
    GameEntity::onReset();

    m_health = kPlayerDefaultMaxHealth;
    m_maxHealth = kPlayerDefaultMaxHealth;
    m_moveSpeed = 320.0;
    m_moveDirection = MoveDirection::None;
    m_horizontalAxis = 0;
    m_verticalAxis = 0;
    m_sceneRect = QRectF();
    m_collisionShieldRemainSec = 0.0;
    m_collisionShieldElapsedSec = 0.0;
    m_healthDeltaFxRemainSec = 0.0;
    m_healthDeltaFxElapsedSec = 0.0;
    m_healthDeltaValue = 0;
}

void PlayerEntity::triggerHealthDeltaFx(int deltaHealth)
{
    if (deltaHealth == 0) {
        return;
    }
    m_healthDeltaValue = deltaHealth;
    m_healthDeltaFxRemainSec = kHealthDeltaFxDurationSec;
    m_healthDeltaFxElapsedSec = 0.0;
}

void PlayerEntity::updateVelocityByDirection()
{
    const qreal vx = static_cast<qreal>(m_horizontalAxis) * m_moveSpeed;
    const qreal vy = static_cast<qreal>(m_verticalAxis) * m_moveSpeed;
    setVelocity(QPointF(vx, vy));
}

void PlayerEntity::clampInScene()
{
    if (m_sceneRect.isNull() || !m_sceneRect.isValid()) {
        return;
    }

    QRectF rect = boundingRect();
    QPointF pos = position();

    if (rect.left() < m_sceneRect.left()) {
        pos.setX(m_sceneRect.left());
    }

    if (rect.right() > m_sceneRect.right()) {
        pos.setX(m_sceneRect.right() - size().width());
    }

    if (rect.top() < m_sceneRect.top()) {
        pos.setY(m_sceneRect.top());
    }

    if (rect.bottom() > m_sceneRect.bottom()) {
        pos.setY(m_sceneRect.bottom() - size().height());
    }

    setPosition(pos);
}
