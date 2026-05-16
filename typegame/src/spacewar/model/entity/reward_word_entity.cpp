#include "reward_word_entity.h"

#include <QtGlobal>

RewardWordEntity::RewardWordEntity()
    : GameEntity()
{
}

RewardWordEntity::RewardWordEntity(int id)
    : GameEntity(id)
{
}

void RewardWordEntity::update(qreal deltaTime)
{
    GameEntity::update(deltaTime);
}

void RewardWordEntity::reset()
{
    GameEntity::reset();
}

QString RewardWordEntity::word() const
{
    return m_word;
}

void RewardWordEntity::setWord(const QString& word)
{
    m_word = word;
    m_inputIndex = 0;
    m_completed = false;
    m_expired = false;
}

int RewardWordEntity::inputIndex() const
{
    return m_inputIndex;
}

QString RewardWordEntity::typedPart() const
{
    return m_word.left(m_inputIndex);
}

QString RewardWordEntity::remainingPart() const
{
    return m_word.mid(m_inputIndex);
}

bool RewardWordEntity::handleInput(QChar ch)
{
    if (!isAlive() || !isActive() || m_completed || m_expired) {
        return false;
    }

    if (m_word.isEmpty()) {
        return false;
    }

    if (m_inputIndex < 0 || m_inputIndex >= m_word.size()) {
        return false;
    }

    const QChar expected = m_word.at(m_inputIndex);

    if (expected.toLower() != ch.toLower()) {
        return false;
    }

    ++m_inputIndex;

    if (m_inputIndex >= m_word.size()) {
        complete();
    }

    return true;
}

bool RewardWordEntity::isCompleted() const
{
    return m_completed;
}

bool RewardWordEntity::isExpired() const
{
    return m_expired;
}

int RewardWordEntity::healValue() const
{
    return m_healValue;
}

void RewardWordEntity::setHealValue(int value)
{
    m_healValue = qMax(0, value);
}

int RewardWordEntity::scoreValue() const
{
    return m_scoreValue;
}

void RewardWordEntity::setScoreValue(int value)
{
    m_scoreValue = qMax(0, value);
}

QRectF RewardWordEntity::sceneRect() const
{
    return m_sceneRect;
}

void RewardWordEntity::setSceneRect(const QRectF& sceneRect)
{
    m_sceneRect = sceneRect;
}

qreal RewardWordEntity::outMargin() const
{
    return m_outMargin;
}

void RewardWordEntity::setOutMargin(qreal margin)
{
    m_outMargin = qMax<qreal>(0.0, margin);
}

void RewardWordEntity::onUpdate(qreal deltaTime)
{
    Q_UNUSED(deltaTime);

    if (isOutOfScene()) {
        expire();
    }
}

void RewardWordEntity::onReset()
{
    GameEntity::onReset();

    m_word.clear();
    m_inputIndex = 0;

    m_completed = false;
    m_expired = false;

    m_healValue = 1;
    m_scoreValue = 50;

    m_sceneRect = QRectF();
    m_outMargin = 80.0;
}

bool RewardWordEntity::isOutOfScene() const
{
    if (m_sceneRect.isNull() || !m_sceneRect.isValid()) {
        return false;
    }

    const QRectF checkRect = m_sceneRect.adjusted(-m_outMargin,
                                                  -m_outMargin,
                                                  m_outMargin,
                                                  m_outMargin);

    return !checkRect.intersects(boundingRect());
}

void RewardWordEntity::complete()
{
    m_completed = true;

    kill();
    setActive(false);
    setVisible(false);
}

void RewardWordEntity::expire()
{
    m_expired = true;

    kill();
    setActive(false);
    setVisible(false);
}
