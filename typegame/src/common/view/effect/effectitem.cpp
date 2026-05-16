#include "effectitem.h"

#include <QtMath>

EffectItem::EffectItem()
{
}

EffectItem::EffectItem(const QPointF& logicalPos,
                       const QSizeF& logicalSize,
                       const QVector<QPixmap>& frames,
                       qreal lifeTime,
                       qreal frameInterval,
                       bool loop)
    : m_logicalPos(logicalPos),
      m_logicalSize(logicalSize),
      m_frames(frames),
      m_lifeTime(lifeTime),
      m_frameInterval(frameInterval),
      m_loop(loop)
{
    if (m_lifeTime <= 0.0) {
        m_lifeTime = 0.001;
    }

    if (m_frameInterval <= 0.0) {
        m_frameInterval = 0.001;
    }
}

void EffectItem::update(qreal dt)
{
    if (!m_alive) {
        return;
    }

    if (dt <= 0.0) {
        return;
    }

    if (m_frames.isEmpty()) {
        m_alive = false;
        onLifeEnd();
        return;
    }

    m_elapsed += dt;

    updateMovement(dt);
    onUpdate(dt);
    updateFrame();

    if (!m_infiniteLife && m_elapsed >= m_lifeTime) {
        m_alive = false;
        onLifeEnd();
    }
}

void EffectItem::paint(QPainter* painter,
                       qreal viewScale,
                       const QPointF& cameraOffset) const
{
    if (!m_alive || !painter) {
        return;
    }

    const QPixmap* pix = currentPixmap();
    if (!pix || pix->isNull()) {
        return;
    }

    const QPointF screenPos = (m_logicalPos - cameraOffset) * viewScale;
    const QSizeF finalSize = m_logicalSize * (m_localScale * viewScale);

    if (finalSize.width() <= 0.0 || finalSize.height() <= 0.0) {
        return;
    }

    const QRectF targetRect(-finalSize.width() / 2.0,
                            -finalSize.height() / 2.0,
                            finalSize.width(),
                            finalSize.height());

    painter->save();

    painter->setOpacity(m_opacity);
    painter->translate(screenPos);
    painter->rotate(m_rotation);
    painter->drawPixmap(targetRect, *pix, pix->rect());

    painter->restore();
}

bool EffectItem::isAlive() const
{
    return m_alive;
}

void EffectItem::kill()
{
    m_alive = false;
}

void EffectItem::setLogicalPosition(const QPointF& pos)
{
    m_logicalPos = pos;
}

QPointF EffectItem::logicalPosition() const
{
    return m_logicalPos;
}

void EffectItem::setVelocity(const QPointF& vel)
{
    m_velocity = vel;
}

QPointF EffectItem::velocity() const
{
    return m_velocity;
}

void EffectItem::setLogicalSize(const QSizeF& size)
{
    m_logicalSize = size;
}

QSizeF EffectItem::logicalSize() const
{
    return m_logicalSize;
}

void EffectItem::setLocalScale(qreal scale)
{
    m_localScale = scale;
}

qreal EffectItem::localScale() const
{
    return m_localScale;
}

void EffectItem::setRotation(qreal rotation)
{
    m_rotation = rotation;
}

qreal EffectItem::rotation() const
{
    return m_rotation;
}

void EffectItem::setOpacity(qreal opacity)
{
    m_opacity = qBound<qreal>(0.0, opacity, 1.0);
}

qreal EffectItem::opacity() const
{
    return m_opacity;
}

void EffectItem::setLoop(bool loop)
{
    m_loop = loop;
}

bool EffectItem::loop() const
{
    return m_loop;
}

void EffectItem::setLifeTime(qreal lifeTime)
{
    m_lifeTime = (lifeTime > 0.0) ? lifeTime : 0.001;
}

qreal EffectItem::lifeTime() const
{
    return m_lifeTime;
}

void EffectItem::setFrameInterval(qreal interval)
{
    m_frameInterval = (interval > 0.0) ? interval : 0.001;
}

qreal EffectItem::frameInterval() const
{
    return m_frameInterval;
}

qreal EffectItem::elapsedTime() const
{
    return m_elapsed;
}

void EffectItem::setFrames(const QVector<QPixmap>& frames)
{
    m_frames = frames;
    m_currentFrame = 0;
}

const QVector<QPixmap>& EffectItem::frames() const
{
    return m_frames;
}

void EffectItem::updateMovement(qreal dt)
{
    m_logicalPos += QPointF(m_velocity.x() * dt, m_velocity.y() * dt);
}

void EffectItem::onUpdate(qreal dt)
{
    Q_UNUSED(dt);
}

void EffectItem::updateFrame()
{
    if (m_frames.isEmpty()) {
        return;
    }

    if (m_loop) {
        const int frameCount = m_frames.size();
        const int frame = static_cast<int>(m_elapsed / m_frameInterval) % frameCount;
        m_currentFrame = frame;
    } else {
        int frame = static_cast<int>(m_elapsed / m_frameInterval);
        if (frame >= m_frames.size()) {
            frame = m_frames.size() - 1;
        }
        m_currentFrame = qMax(0, frame);
    }
}

void EffectItem::onLifeEnd()
{
}

int EffectItem::currentFrameIndex() const
{
    return m_currentFrame;
}

const QPixmap* EffectItem::currentPixmap() const
{
    if (m_frames.isEmpty()) {
        return nullptr;
    }

    if (m_currentFrame < 0 || m_currentFrame >= m_frames.size()) {
        return nullptr;
    }

    return &m_frames[m_currentFrame];
}

void EffectItem::setInfiniteLife(bool infinite)
{
    m_infiniteLife = infinite;
}

bool EffectItem::hasInfiniteLife() const
{
    return m_infiniteLife;
}
