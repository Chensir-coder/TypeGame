#include "game_entity.h"

#include <QtGlobal>

GameEntity::GameEntity()
    : GameObjectBase()
{
    setFramePlaybackCycle(12, 1.0);
}

GameEntity::GameEntity(int id)
    : GameObjectBase(id)
{
    setFramePlaybackCycle(12, 1.0);
}

void GameEntity::update(qreal deltaTime)
{
    if (!isAlive()) {
        return;
    }

    if (deltaTime <= 0.0) {
        return;
    }

    advanceFrameAnimation(deltaTime);
    GameObjectBase::update(deltaTime);
}

void GameEntity::advanceFrameAnimation(qreal deltaTime)
{
    if (deltaTime <= 0.0) {
        return;
    }

    if (m_frameCount <= 1 || m_frameFps <= 0.0) {
        return;
    }

    m_frameRemainder += deltaTime * m_frameFps;

    while (m_frameRemainder >= 1.0) {
        m_frameRemainder -= 1.0;
        m_frameIndex = (m_frameIndex + 1) % m_frameCount;
    }
}

int GameEntity::currentFrameIndex() const
{
    return m_frameIndex;
}

int GameEntity::frameCount() const
{
    return m_frameCount;
}

void GameEntity::setFramePlayback(int frameCount, qreal framesPerSecond)
{
    m_frameCount = qMax(1, frameCount);
    m_frameFps = qMax(0.0, framesPerSecond);
    m_frameIndex = 0;
    m_frameRemainder = 0.0;
}

void GameEntity::setFramePlaybackCycle(int frameCount, qreal cycleSeconds)
{
    const int c = qMax(1, frameCount);
    const qreal secs = qMax(0.000001, cycleSeconds);
    setFramePlayback(c, static_cast<qreal>(c) / secs);
}

bool GameEntity::isActive() const
{
    return m_active;
}

void GameEntity::setActive(bool active)
{
    m_active = active;
}

bool GameEntity::isVisible() const
{
    return m_visible;
}

void GameEntity::setVisible(bool visible)
{
    m_visible = visible;
}

void GameEntity::reset()
{
    GameObjectBase::reset();
}

void GameEntity::onReset()
{
    m_active = false;
    m_visible = true;
    setFramePlaybackCycle(12, 1.0);
}
