#include "typing_target_entity.h"

#include <QtGlobal>

TypingTargetEntity::TypingTargetEntity()
    : GameEntity()
{
}

TypingTargetEntity::TypingTargetEntity(int id)
    : GameEntity(id)
{
}

void TypingTargetEntity::reset()
{
    GameEntity::reset();
    onReset();
}

QString TypingTargetEntity::text() const
{
    return m_text;
}

void TypingTargetEntity::setText(const QString& text)
{
    m_text = text.toUpper();
}

QChar TypingTargetEntity::letter() const
{
    if (m_text.isEmpty()) {
        return QChar();
    }

    return m_text.at(0);
}

void TypingTargetEntity::setLetter(QChar letter)
{
    if (letter.isNull()) {
        m_text.clear();
        return;
    }

    m_text = QString(letter.toUpper());
}

bool TypingTargetEntity::matches(QChar input) const
{
    if (!isActive()) {
        return false;
    }

    if (input.isNull()) {
        return false;
    }

    return letter() == input.toUpper();
}

bool TypingTargetEntity::matches(const QString& input) const
{
    if (!isActive()) {
        return false;
    }

    return m_text == input.toUpper();
}

int TypingTargetEntity::scoreValue() const
{
    return m_scoreValue;
}

void TypingTargetEntity::setScoreValue(int value)
{
    m_scoreValue = value;
}

QRectF TypingTargetEntity::sceneRect() const
{
    return m_sceneRect;
}

void TypingTargetEntity::setSceneRect(const QRectF& rect)
{
    m_sceneRect = rect;
}

qreal TypingTargetEntity::exitLineRatio() const
{
    return m_exitLineRatio;
}

void TypingTargetEntity::setExitLineRatio(qreal ratio)
{
    if (ratio < 0.0) {
        m_exitLineRatio = 0.0;
    } else if (ratio > 1.0) {
        m_exitLineRatio = 1.0;
    } else {
        m_exitLineRatio = ratio;
    }
}

qreal TypingTargetEntity::exitLineY() const
{
    return m_sceneRect.top() + m_sceneRect.height() * m_exitLineRatio;
}

bool TypingTargetEntity::hasReachedExitLine() const
{
    const qreal gh = size().height();
    if (gh <= 0.0) {
        /*
         * 未设置占位高度时沿用旧语义：以 position.y()（通常为生成锚点）
         * 直接对比退场纵坐标。
         */
        return position().y() >= exitLineY();
    }

    const qreal bottom_y = position().y() + gh;
    return bottom_y >= exitLineY();
}

qreal TypingTargetEntity::failLineRatio() const
{
    return exitLineRatio();
}

void TypingTargetEntity::setFailLineRatio(qreal ratio)
{
    setExitLineRatio(ratio);
}

qreal TypingTargetEntity::failLineY() const
{
    return exitLineY();
}

bool TypingTargetEntity::hasReachedFailLine() const
{
    return hasReachedExitLine();
}

TypingTargetDestroyReason TypingTargetEntity::destroyReason() const
{
    return m_destroyReason;
}

bool TypingTargetEntity::isDestroyed() const
{
    return m_destroyReason != TypingTargetDestroyReason::None;
}

void TypingTargetEntity::markMatched()
{
    destroy(TypingTargetDestroyReason::Matched);
}

void TypingTargetEntity::markReachedExitLine()
{
    destroy(TypingTargetDestroyReason::ReachedExitLine);
}

void TypingTargetEntity::markReachedFailLine()
{
    markReachedExitLine();
}

void TypingTargetEntity::markHitByBullet()
{
    destroy(TypingTargetDestroyReason::HitByBullet);
}

void TypingTargetEntity::markCollisionWithPlayer()
{
    destroy(TypingTargetDestroyReason::CollisionWithPlayer);
}

void TypingTargetEntity::markOutOfScene()
{
    destroy(TypingTargetDestroyReason::OutOfScene);
}

void TypingTargetEntity::configureSpriteAnimation(int frame_count, qreal frames_per_second)
{
    const int count = qMax(1, frame_count);
    const qreal fps = qMax(0.0, frames_per_second);
    if (fps > 0.0) {
        setFramePlayback(count, fps);
    } else if (count > 1) {
        setFramePlaybackCycle(count, 1.0);
    } else {
        setFramePlayback(1, 0.0);
    }
}

int TypingTargetEntity::spriteAnimationFrameCount() const
{
    return frameCount();
}

int TypingTargetEntity::spriteAnimationFrameIndex() const
{
    return currentFrameIndex();
}

void TypingTargetEntity::onUpdate(qreal deltaTime)
{
    Q_UNUSED(deltaTime);

    if (isActive() && !isDestroyed() && hasReachedExitLine()) {
        markReachedExitLine();
    }
}

void TypingTargetEntity::onReset()
{
    GameEntity::onReset();

    m_text.clear();
    m_scoreValue = 10;
    m_sceneRect = QRectF();
    m_exitLineRatio = 0.7;
    m_destroyReason = TypingTargetDestroyReason::None;
}

void TypingTargetEntity::destroy(TypingTargetDestroyReason reason)
{
    if (m_destroyReason != TypingTargetDestroyReason::None) {
        return;
    }

    m_destroyReason = reason;
}
