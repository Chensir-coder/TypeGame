#include "typing_game_model_base.h"

TypingGameModelBase::TypingGameModelBase()
{
    initialize();
}

void TypingGameModelBase::initialize()
{
    m_gameState = GameState::Initial;

    m_config.resetToDefault();
    m_stats.reset();

    clearActiveTargets();

    notifyStateEvent(GameEventType::GameInitialized);
    notifyStateEvent(GameEventType::GameStateChanged);
    notifyScoreChanged();
}

void TypingGameModelBase::resetModel()
{
    m_stats.reset();
    clearActiveTargets();

    m_gameState = GameState::Initial;

    notifyStateEvent(GameEventType::GameRestarted);
    notifyStateEvent(GameEventType::GameStateChanged);
    notifyScoreChanged();
}

void TypingGameModelBase::startGame()
{
    if (m_gameState == GameState::Playing) {
        return;
    }

    if (m_gameState == GameState::End) {
        resetModel();
    }

    m_gameState = GameState::Playing;

    notifyStateEvent(GameEventType::GameStarted);
    notifyStateEvent(GameEventType::GameStateChanged);
}

void TypingGameModelBase::pauseGame()
{
    if (m_gameState != GameState::Playing) {
        return;
    }

    m_gameState = GameState::Paused;

    notifyStateEvent(GameEventType::GamePaused);
    notifyStateEvent(GameEventType::GameStateChanged);
}

void TypingGameModelBase::resumeGame()
{
    if (m_gameState != GameState::Paused) {
        return;
    }

    m_gameState = GameState::Playing;

    notifyStateEvent(GameEventType::GameResumed);
    notifyStateEvent(GameEventType::GameStateChanged);
}

void TypingGameModelBase::stopGame()
{
    clearActiveTargets();

    m_gameState = GameState::End;

    notifyStateEvent(GameEventType::GameStopped);
    notifyStateEvent(GameEventType::GameStateChanged);
}

void TypingGameModelBase::update(int deltaMs)
{
    if (m_gameState != GameState::Playing) {
        return;
    }

    if (deltaMs <= 0) {
        return;
    }

    updateActiveTargets(deltaMs);

    /*
     * 先处理退场，再处理其他销毁。
     * 退场不是天然失败，具体含义交给 onTargetExited 决定。
     */
    processExitedTargets();
    processDestroyedTargets();

    if (shouldCompleteLevel()) {
        finishCurrentRound();
        return;
    }

    if (shouldGameOver()) {
        onGameOver();
        return;
    }
}

bool TypingGameModelBase::registerTarget(TypingTargetEntity* target)
{
    if (target == nullptr) {
        return false;
    }

    if (!canSpawnTarget()) {
        return false;
    }

    if (!isLetterAvailable(target->letter())) {
        return false;
    }

    if (m_activeTargets.contains(target)) {
        return false;
    }

    target->setActive(true);
    target->setVisible(true);
    target->setSceneRect(QRectF(0.0,
                                0.0,
                                m_config.sceneWidth(),
                                m_config.sceneHeight()));
    target->setExitLineRatio(m_config.exitLineRatio());

    m_activeTargets.append(target);
    occupyLetter(target->letter());

    notifyTargetSpawned(target->letter());

    return true;
}

bool TypingGameModelBase::unregisterTarget(TypingTargetEntity* target)
{
    if (target == nullptr) {
        return false;
    }

    if (!m_activeTargets.contains(target)) {
        return false;
    }

    const QChar letter = target->letter();

    removeTargetInternal(target);

    notifyTargetRemoved(letter);

    return true;
}

bool TypingGameModelBase::handleInput(QChar inputLetter)
{
    if (m_gameState != GameState::Playing) {
        return false;
    }

    TypingTargetEntity* target = findTargetByLetter(inputLetter);
    if (target == nullptr) {
        m_stats.recordRejectedInput();
        m_stats.recordTypingKeystroke(false);
        notifyInputRejected(inputLetter);
        notifyScoreChanged();
        return false;
    }

    onTargetMatched(target);
    m_stats.recordTypingKeystroke(true);
    return true;
}

TypingTargetEntity* TypingGameModelBase::findTargetByLetter(QChar letter) const
{
    const QChar normalizedLetter = letter.toUpper();

    for (TypingTargetEntity* target : m_activeTargets) {
        if (target == nullptr) {
            continue;
        }

        if (!target->isActive()) {
            continue;
        }

        if (target->matches(normalizedLetter)) {
            return target;
        }
    }

    return nullptr;
}

bool TypingGameModelBase::canSpawnTarget() const
{
    return m_gameState != GameState::End
           && m_activeTargets.size() < m_config.maxTargetCount();
}

bool TypingGameModelBase::isLetterAvailable(QChar letter) const
{
    return !m_usedLetters.contains(letter.toUpper());
}

void TypingGameModelBase::clearActiveTargets()
{
    for (TypingTargetEntity* target : m_activeTargets) {
        if (target != nullptr) {
            target->setActive(false);
            target->setVisible(false);
        }
    }

    m_activeTargets.clear();
    m_usedLetters.clear();
}

GameState TypingGameModelBase::gameState() const
{
    return m_gameState;
}

TypingGameConfig& TypingGameModelBase::config()
{
    return m_config;
}

const TypingGameConfig& TypingGameModelBase::config() const
{
    return m_config;
}

TypingGameStats& TypingGameModelBase::stats()
{
    return m_stats;
}

const TypingGameStats& TypingGameModelBase::stats() const
{
    return m_stats;
}

const QList<TypingTargetEntity*>& TypingGameModelBase::activeTargets() const
{
    return m_activeTargets;
}

int TypingGameModelBase::currentTargetMoveSpeed() const
{
    return m_config.currentMoveSpeed();
}

void TypingGameModelBase::confirmTargetDestroyed(TypingTargetEntity* target,
                                                 TypingTargetDestroyReason reason)
{
    if (target == nullptr) {
        return;
    }

    if (!m_activeTargets.contains(target)) {
        return;
    }

    const QChar letter = target->letter();

    switch (reason) {
    case TypingTargetDestroyReason::Matched:
        target->markMatched();
        m_stats.recordSuccess(hitScoreValue(target));
        notifyTargetHit(letter);
        notifyScoreChanged();
        break;

    case TypingTargetDestroyReason::HitByBullet:
        target->markHitByBullet();
        m_stats.recordSuccess(hitScoreValue(target));
        notifyTargetHit(letter);
        notifyScoreChanged();
        break;

    case TypingTargetDestroyReason::CollisionWithPlayer:
        target->markCollisionWithPlayer();
        break;

    case TypingTargetDestroyReason::OutOfScene:
        target->markOutOfScene();
        break;

    case TypingTargetDestroyReason::ReachedExitLine:
        target->markReachedExitLine();
        break;

    case TypingTargetDestroyReason::None:
    default:
        break;
    }

    removeTargetInternal(target);
    notifyTargetRemoved(letter);
}

void TypingGameModelBase::onTargetMatched(TypingTargetEntity* target)
{
    /*
     * 默认玩法：输入命中就直接消除目标。
     * 苹果直接使用。
     * 飞机大战重写这个函数，改成“锁定敌机 + 发射子弹”。
     */
    confirmTargetDestroyed(target, TypingTargetDestroyReason::Matched);
}

void TypingGameModelBase::onTargetExited(TypingTargetEntity* target)
{
    /*
     * 默认解释：退场 = 漏掉 = miss。
     * 苹果直接使用。
     * 飞机大战重写这个函数，改成自然回收。
     */
    onTargetMissed(target);
}

void TypingGameModelBase::onTargetMissed(TypingTargetEntity* target)
{
    if (target == nullptr) {
        return;
    }

    if (!m_activeTargets.contains(target)) {
        return;
    }

    const QChar letter = target->letter();

    m_stats.recordMiss(missPenaltyValue(target));

    removeTargetInternal(target);

    notifyTargetMiss(letter);
    notifyTargetRemoved(letter);
    notifyScoreChanged();
}

void TypingGameModelBase::onTargetRemoved(TypingTargetEntity* target)
{
    Q_UNUSED(target);
}

void TypingGameModelBase::onLevelCompleted()
{
}

void TypingGameModelBase::onGameOver()
{
    if (m_gameState == GameState::End) {
        return;
    }

    notifyGameOver();

    m_gameState = GameState::End;

    notifyStateEvent(GameEventType::GameStateChanged);
}

bool TypingGameModelBase::shouldCompleteLevel() const
{
    return m_stats.isTargetCompleted(m_config.targetSuccessCount());
}

bool TypingGameModelBase::shouldGameOver() const
{
    return m_stats.isMissLimitReached(m_config.maxMissCount());
}

int TypingGameModelBase::hitScoreValue(const TypingTargetEntity* target) const
{
    if (target != nullptr && target->scoreValue() > 0) {
        return target->scoreValue();
    }

    return 10 + (m_config.speedLevel() - 1) * 2;
}

int TypingGameModelBase::missPenaltyValue(const TypingTargetEntity* target) const
{
    Q_UNUSED(target);
    return 5;
}

void TypingGameModelBase::removeTargetInternal(TypingTargetEntity* target)
{
    if (target == nullptr) {
        return;
    }

    const int index = m_activeTargets.indexOf(target);
    if (index < 0) {
        return;
    }

    releaseLetter(target->letter());

    m_activeTargets.removeAt(index);

    target->setActive(false);
    target->setVisible(false);

    onTargetRemoved(target);
}

void TypingGameModelBase::occupyLetter(QChar letter)
{
    if (!letter.isNull()) {
        m_usedLetters.insert(letter.toUpper());
    }
}

void TypingGameModelBase::releaseLetter(QChar letter)
{
    if (!letter.isNull()) {
        m_usedLetters.remove(letter.toUpper());
    }
}

void TypingGameModelBase::updateActiveTargets(int deltaMs)
{
    const qreal deltaSeconds = static_cast<qreal>(deltaMs) / 1000.0;

    for (TypingTargetEntity* target : m_activeTargets) {
        if (target == nullptr) {
            continue;
        }

        if (!target->isActive()) {
            continue;
        }

        target->update(deltaSeconds);
    }
}

void TypingGameModelBase::processExitedTargets()
{
    QList<TypingTargetEntity*> exitedTargets;

    for (TypingTargetEntity* target : m_activeTargets) {
        if (target == nullptr) {
            continue;
        }

        if (!target->isActive()) {
            continue;
        }

        if (target->destroyReason() == TypingTargetDestroyReason::ReachedExitLine) {
            exitedTargets.append(target);
        }
    }

    for (TypingTargetEntity* target : exitedTargets) {
        onTargetExited(target);
    }
}

void TypingGameModelBase::processMissedTargets()
{
    processExitedTargets();
}

void TypingGameModelBase::processDestroyedTargets()
{
    QList<TypingTargetEntity*> removedTargets;

    for (TypingTargetEntity* target : m_activeTargets) {
        if (target == nullptr) {
            continue;
        }

        if (!target->isDestroyed()) {
            continue;
        }

        if (target->destroyReason() == TypingTargetDestroyReason::ReachedExitLine) {
            continue;
        }

        removedTargets.append(target);
    }

    for (TypingTargetEntity* target : removedTargets) {
        unregisterTarget(target);
    }
}

void TypingGameModelBase::finishCurrentRound()
{
    if (m_gameState == GameState::End) {
        return;
    }

    m_gameState = GameState::End;

    notifyStateEvent(GameEventType::GameStateChanged);
    notifyLevelCompleted();

    onLevelCompleted();
}

void TypingGameModelBase::syncActiveTargetSpeed()
{
    const int speed = currentTargetMoveSpeed();
    const qreal vy = static_cast<qreal>(speed);

    for (TypingTargetEntity* target : m_activeTargets) {
        if (target == nullptr) {
            continue;
        }

        const qreal vx = target->velocity().x();
        target->setVelocity(QPointF(vx, vy));
    }
}

void TypingGameModelBase::notifyStateEvent(GameEventType eventType)
{
    GameEvent event;
    event.type = eventType;
    event.state = m_gameState;
    event.score = m_stats.score();
    event.level = m_config.speedLevel();
    event.value = 0;
    event.extra_value = 0;
    notifyObservers(event);
}

void TypingGameModelBase::notifyScoreChanged()
{
    GameEvent event;
    event.type = GameEventType::ScoreChanged;
    event.state = m_gameState;
    event.value = m_stats.score();
    event.extra_value = 0;
    event.score = m_stats.score();
    event.level = m_config.speedLevel();
    notifyObservers(event);
}

void TypingGameModelBase::notifyTargetSpawned(QChar letter)
{
    GameEvent event;
    event.type = GameEventType::EntitySpawned;
    event.state = m_gameState;
    event.letter = letter;
    event.value = m_activeTargets.size();
    event.extra_value = 0;
    event.score = m_stats.score();
    event.level = m_config.speedLevel();
    notifyObservers(event);
}

void TypingGameModelBase::notifyTargetRemoved(QChar letter)
{
    GameEvent event;
    event.type = GameEventType::EntityRemoved;
    event.state = m_gameState;
    event.letter = letter;
    event.value = m_activeTargets.size();
    event.extra_value = 0;
    event.score = m_stats.score();
    event.level = m_config.speedLevel();
    notifyObservers(event);
}

void TypingGameModelBase::notifyTargetHit(QChar letter)
{
    GameEvent event;
    event.type = GameEventType::EntityHit;
    event.state = m_gameState;
    event.letter = letter;
    event.value = m_stats.successCount();
    event.extra_value = m_stats.missCount();
    event.score = m_stats.score();
    event.level = m_config.speedLevel();
    notifyObservers(event);
}

void TypingGameModelBase::notifyTargetMiss(QChar letter)
{
    GameEvent event;
    event.type = GameEventType::EntityMissed;
    event.state = m_gameState;
    event.letter = letter;
    event.value = m_stats.missCount();
    event.extra_value = m_stats.successCount();
    event.score = m_stats.score();
    event.level = m_config.speedLevel();
    notifyObservers(event);
}

void TypingGameModelBase::notifyLevelCompleted()
{
    GameEvent event;
    event.type = GameEventType::LevelCompleted;
    event.state = m_gameState;
    event.value = m_stats.successCount();
    event.extra_value = m_config.targetSuccessCount();
    event.score = m_stats.score();
    event.level = m_config.speedLevel();
    notifyObservers(event);
}

void TypingGameModelBase::notifyGameOver()
{
    GameEvent event;
    event.type = GameEventType::GameOver;
    event.state = m_gameState;
    event.value = m_stats.missCount();
    event.extra_value = m_config.maxMissCount();
    event.score = m_stats.score();
    event.level = m_config.speedLevel();
    notifyObservers(event);
}

void TypingGameModelBase::notifySettingsChanged(SettingKey key, int value, int extraValue)
{
    GameEvent event;
    event.type = GameEventType::SettingsChanged;
    event.state = m_gameState;
    event.setting_key = key;
    event.value = value;
    event.extra_value = extraValue;
    event.score = m_stats.score();
    event.level = m_config.speedLevel();
    notifyObservers(event);
}

void TypingGameModelBase::notifyInputRejected(QChar letter)
{
    GameEvent event;
    event.type = GameEventType::InputRejected;
    event.state = m_gameState;
    event.letter = letter;
    event.value = m_stats.rejectedInputCount();
    event.extra_value = m_stats.inputCount();
    event.score = m_stats.score();
    event.level = m_config.speedLevel();
    notifyObservers(event);
}
