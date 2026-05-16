/* ------------------------------------------------------------------
 // 文件名     : spacewar_model.cpp
------------------------------------------------------------------ */

#include "spacewar_model.h"

#include <QRectF>
#include <QtGlobal>

#include "common/core/game_state.h"
#include "spacewar/model/entity/bullet_entity.h"
#include "spacewar/model/entity/enemy_entity.h"
#include "spacewar/model/entity/player_entity.h"
#include "spacewar/model/entity/reward_word_entity.h"
#include "spacewar_model_signal_bridge.h"
#include "common/service/config/configure_settings.h"
#include "QDebug"

SpaceWarModel::SpaceWarModel()
{
    m_signalBridge = std::make_unique<SpaceWarModelSignalBridge>();
    addObserver(m_signalBridge.get());
}

SpaceWarModel::~SpaceWarModel()
{
    removeObserver(m_signalBridge.get());

    clearReward();
    clearBullets();
}

SpaceWarModelSignalBridge* SpaceWarModel::signalBridge() const
{
    return m_signalBridge.get();
}

PlayerEntity* SpaceWarModel::player() const
{
    return m_player.get();
}

void SpaceWarModel::preparePlayerCraftAfterSceneFix()
{
    if (!m_player) {
        m_player = std::make_unique<PlayerEntity>();
    }

    const auto& c = config();
    const qreal sw = static_cast<qreal>(c.sceneWidth());
    const qreal sh = static_cast<qreal>(c.sceneHeight());
    const qreal lw = c.playerCraftLogicalWidth();
    const qreal lh = c.playerCraftLogicalHeight();
    const qreal margin = c.playerCraftLogicalBottomMargin();

    m_player->setSize(QSizeF(lw, lh));
    m_player->setPosition(QPointF(sw / 2.0 - lw / 2.0, sh - lh - margin));
    m_player->setSceneRect(QRectF(0.0, 0.0, sw, sh));

    m_player->setAlive(true);
    m_player->setActive(true);
    m_player->setVisible(true);

    const auto& cfgSw = ConfigureSettings::instance().spacewar();
    m_player->setMoveSpeed(cfgSw.playerCraft.moveSpeed);
    m_player->setMaxHealth(cfgSw.playerCraft.maxHealth);
    m_player->setHealth(cfgSw.playerCraft.maxHealth);

    m_player->applyPlaybackFromTypingGameConfig(c);
}

int SpaceWarModel::playerHealth() const
{
    if (m_player == nullptr) {
        return 0;
    }
    return m_player->health();
}

int SpaceWarModel::playerMaxHealth() const
{
    if (m_player == nullptr) {
        return ConfigureSettings::instance().spacewar().playerCraft.maxHealth;
    }
    return m_player->maxHealth();
}

int SpaceWarModel::applyPlayerDamage(int amount)
{
    if (m_player == nullptr || amount <= 0) {
        return 0;
    }

    const int before = m_player->health();
    m_player->damage(amount);
    const int after = m_player->health();
    const int actual = before - after;

    if (actual > 0) {
        notifyPlayerDamaged(actual, after);
        notifyPlayerHpChanged();

        if (after <= 0) {
            notifyPlayerDied();
            onGameOver();
        }
    }

    return actual;
}

int SpaceWarModel::applyPlayerHeal(int amount)
{
    if (m_player == nullptr || amount <= 0) {
        return 0;
    }

    const int before = m_player->health();
    m_player->heal(amount);
    const int after = m_player->health();
    const int actual = after - before;

    if (actual > 0) {
        notifyPlayerHealed(actual, after);
        notifyPlayerHpChanged();
    }

    return actual;
}

EnemyEntity* SpaceWarModel::tryLockEnemyByLetter(QChar letter)
{
    if (gameState() != GameState::Playing) {
        return nullptr;
    }

    const QChar normalized = letter.toUpper();

    for (TypingTargetEntity* t : activeTargets()) {
        if (t == nullptr) {
            continue;
        }

        EnemyEntity* enemy = static_cast<EnemyEntity*>(t);

        if (enemy->isLocked()) {
            continue;
        }

        if (!enemy->isActive() || enemy->isDestroyed()) {
            continue;
        }

        if (enemy->matches(normalized)) {
            enemy->setLocked(true);
            notifyEnemyLocked(normalized);
            return enemy;
        }
    }

    return nullptr;
}

void SpaceWarModel::destroyEnemyByBullet(EnemyEntity* enemy)
{
    if (enemy == nullptr) {
        return;
    }

    confirmTargetDestroyed(enemy, TypingTargetDestroyReason::HitByBullet);
}

void SpaceWarModel::destroyEnemyByCollision(EnemyEntity* enemy)
{
    if (enemy == nullptr) {
        return;
    }

    confirmTargetDestroyed(enemy, TypingTargetDestroyReason::CollisionWithPlayer);
}

const QList<BulletEntity*>& SpaceWarModel::activeBullets() const
{
    return m_activeBullets;
}

bool SpaceWarModel::registerBullet(BulletEntity* bullet)
{
    if (bullet == nullptr) {
        return false;
    }

    if (m_activeBullets.contains(bullet)) {
        return false;
    }

    bullet->setAlive(true);
    bullet->setActive(true);
    bullet->setVisible(true);

    m_activeBullets.append(bullet);
    return true;
}

bool SpaceWarModel::unregisterBullet(BulletEntity* bullet)
{
    if (bullet == nullptr) {
        return false;
    }

    const int index = m_activeBullets.indexOf(bullet);
    if (index < 0) {
        return false;
    }

    bullet->setActive(false);
    bullet->setVisible(false);

    m_activeBullets.removeAt(index);
    return true;
}

RewardWordEntity* SpaceWarModel::activeReward() const
{
    return m_activeReward;
}

bool SpaceWarModel::registerReward(RewardWordEntity* reward)
{
    if (reward == nullptr || m_activeReward != nullptr) {
        return false;
    }

    reward->setAlive(true);
    reward->setActive(true);
    reward->setVisible(true);

    m_activeReward = reward;
    notifyRewardSpawned(reward->word());
    return true;
}

bool SpaceWarModel::advanceRewardInput(QChar input)
{
    if (m_activeReward == nullptr) {
        return false;
    }

    if (gameState() != GameState::Playing) {
        return false;
    }

    if (!m_activeReward->handleInput(input)) {
        return false;
    }

    notifyRewardInputProgress(input.toUpper(),
                              m_activeReward->inputIndex(),
                              m_activeReward->word().size());

    if (m_activeReward->isCompleted()) {
        completeReward();
    }

    return true;
}

void SpaceWarModel::completeReward()
{
    if (m_activeReward == nullptr) {
        return;
    }

    const QString w = m_activeReward->word();
    const int heal = m_activeReward->healValue();
    const QRectF rewardRect = m_activeReward->boundingRect();

    if (m_signalBridge) {
        emit m_signalBridge->rewardCompleted(rewardRect.center(), rewardRect.size(), w);
    }

    notifyRewardCompleted(w);
    applyPlayerHeal(heal);

    m_activeReward = nullptr;
}

void SpaceWarModel::expireReward()
{
    if (m_activeReward == nullptr) {
        return;
    }

    const QString w = m_activeReward->word();

    notifyRewardExpired(w);

    m_activeReward = nullptr;
}

int SpaceWarModel::difficultyLevel() const
{
    return m_difficultyLevel;
}

void SpaceWarModel::setDifficultyLevel(int level)
{
    m_difficultyLevel = qBound(1, level, m_maxDifficultyLevel);
}

int SpaceWarModel::maxDifficultyLevel() const
{
    return m_maxDifficultyLevel;
}

bool SpaceWarModel::upgradeDifficulty()
{
    if (m_difficultyLevel >= m_maxDifficultyLevel) {
        return false;
    }

    ++m_difficultyLevel;
    notifyDifficultyUpgraded();
    return true;
}

int SpaceWarModel::maxEnemyCount() const
{
    return m_maxEnemyCount;
}

void SpaceWarModel::setMaxEnemyCount(int count)
{
    if (count < 1 || count > 10) {
        return;
    }

    if (m_maxEnemyCount == count) {
        return;
    }

    m_maxEnemyCount = count;
    config().setMaxTargetCount(count);
    notifySettingsChangedInternal(SettingKey::MaxAppleCount, count);
}

int SpaceWarModel::enemySpeedLevel() const
{
    return m_enemySpeedLevel;
}

void SpaceWarModel::setEnemySpeedLevel(int level)
{
    if (level < 1 || level > 10) {
        return;
    }

    if (m_enemySpeedLevel == level) {
        return;
    }

    m_enemySpeedLevel = level;
    config().setSpeedLevel(level);

    /*
     * 设置变更后立即同步当前在场敌机的下落速度。
     */
    syncActiveTargetSpeed();

    notifySettingsChangedInternal(SettingKey::SpeedLevel, level);
}

int SpaceWarModel::difficultyUpgradeIntervalSec() const
{
    return m_difficultyUpgradeIntervalSec;
}

void SpaceWarModel::setDifficultyUpgradeIntervalSec(int seconds)
{
    if (seconds < 1) {
        return;
    }

    m_difficultyUpgradeIntervalSec = seconds;
    notifySettingsChangedInternal(SettingKey::Unknown, seconds);
}

bool SpaceWarModel::rewardModeEnabled() const
{
    return m_rewardModeEnabled;
}

void SpaceWarModel::setRewardModeEnabled(bool enabled)
{
    m_rewardModeEnabled = enabled;

    if (!enabled) {
        expireReward();
    }

    notifySettingsChangedInternal(SettingKey::Unknown, enabled ? 1 : 0);
}

void SpaceWarModel::setSpaceWarSoundEnabled(bool enabled)
{
    m_spaceWarSoundEnabled = enabled;
    config().setSoundEnabled(enabled);
    notifySettingsChangedInternal(SettingKey::SoundEnabled, enabled ? 1 : 0);
}

bool SpaceWarModel::spaceWarSoundEnabled() const
{
    return m_spaceWarSoundEnabled;
}

int SpaceWarModel::currentEnemyMoveSpeed() const
{
    const auto& em = ConfigureSettings::instance().spacewar().enemyMove;
    const int byLevel =
        em.baseSpeed + (m_enemySpeedLevel - 1) * em.speedPerLevel;
    const int byDifficulty = (m_difficultyLevel - 1) * em.difficultySpeedBonus;
    return qMax(em.minVerticalSpeed, byLevel + byDifficulty);
}

int SpaceWarModel::currentTargetMoveSpeed() const
{
    /*
     * 必须与 currentEnemyMoveSpeed 一致：基类 syncActiveTargetSpeed 依赖 currentTargetMoveSpeed。
     * 若沿用 TypingGameConfig::currentMoveSpeed()（80+(speedLevel-1)*20），会在改敌机速度等级时
     * 把场上敌机竖直速度错误改成另一套公式，且竖直分量远小于 createEnemy 时的设定。
     */
    return currentEnemyMoveSpeed();
}

void SpaceWarModel::setSuppressAutoEnemySpawn(bool suppress)
{
    m_suppressAutoEnemySpawn = suppress;
}

bool SpaceWarModel::suppressAutoEnemySpawn() const
{
    return m_suppressAutoEnemySpawn;
}

int SpaceWarModel::currentEnemySpawnIntervalMs() const
{
    if (m_suppressAutoEnemySpawn) {
        return 2000000000;
    }

    const auto& sp = ConfigureSettings::instance().spacewar().spawn;
    const int delta = (m_difficultyLevel - 1) * sp.stepMs;
    return qMax(sp.minIntervalMs, sp.baseIntervalMs - delta);
}

int SpaceWarModel::score() const
{
    return stats().score();
}

void SpaceWarModel::initialize()
{
    TypingGameModelBase::initialize(); 

    config().setMaxTargetCount(m_maxEnemyCount);
    config().setSpeedLevel(m_enemySpeedLevel);
    config().setSoundEnabled(m_spaceWarSoundEnabled);

    /*
     * 飞机大战的退场线 = 1.0：敌机自然飞出底部时退场，但飞机大战会把退场重写为自然回收，
     * 不算 miss、不扣血。同时 maxMissCount 设置成一个永不可达的值，避免基类 shouldGameOver
     * 命中 miss 死亡条件。
     */
    config().setExitLineRatio(1.0);
    config().setMaxMissCount(99999);
    config().setTargetSuccessCount(99999);
    config().setSpawnIntervalMs(currentEnemySpawnIntervalMs());

    const auto& sw = ConfigureSettings::instance().spacewar();
    config().setTypingTargetLogicalWidth(sw.typingTarget.width);
    config().setTypingTargetLogicalHeight(sw.typingTarget.height);
    config().setPlayerCraftLogicalWidth(sw.playerCraft.width);
    config().setPlayerCraftLogicalHeight(sw.playerCraft.height);
    config().setPlayerCraftLogicalBottomMargin(sw.playerCraft.bottomMargin);
    config().setEnemySpriteSheetCols(sw.spriteSheets.enemyCols);
    config().setEnemySpriteSheetRows(sw.spriteSheets.enemyRows);
    config().setPlayerSpriteSheetCols(sw.spriteSheets.playerCols);
    config().setPlayerSpriteSheetRows(sw.spriteSheets.playerRows);
    config().setEnemySpriteAnimationFps(sw.spriteSheets.enemyAnimationFps);
    config().setPlayerSpriteAnimationFps(sw.spriteSheets.playerAnimationFps);

    m_difficultyLevel = 1;

    resetPlayerHealth();
    clearReward();
    clearBullets();
}

void SpaceWarModel::resetModel()
{
    TypingGameModelBase::resetModel();

    config().setMaxTargetCount(m_maxEnemyCount);
    config().setSpeedLevel(m_enemySpeedLevel);
    config().setSoundEnabled(m_spaceWarSoundEnabled);
    config().setSpawnIntervalMs(currentEnemySpawnIntervalMs());

    m_difficultyLevel = 1;

    if (m_player) {
        m_player->reset();
    }

    resetPlayerHealth();
    clearReward();
    clearBullets();

    notifyPlayerHpChanged();
}

void SpaceWarModel::update(int deltaMs)
{
    if (gameState() != GameState::Playing) {
        return;
    }

    const qreal ds = static_cast<qreal>(deltaMs) / 1000.0;

    if (m_player != nullptr && m_player->isActive() && ds > 0.0) {
        m_player->update(ds);
    }

    if (m_activeReward != nullptr && ds > 0.0) {
        m_activeReward->update(ds);

        if (m_activeReward->isExpired()) {
            expireReward();
        }
    }

    /*
     * 子弹推进与回收。
     */
    if (ds > 0.0) {
        for (BulletEntity* bullet : m_activeBullets) {
            if (bullet == nullptr) {
                continue;
            }
            if (!bullet->isActive() || !bullet->isAlive()) {
                continue;
            }
            bullet->update(ds);
        }
    }

    /*
     * 收集死亡子弹和被击中的敌机；交给基类处理敌机的退场与销毁通知。
     */
    QList<BulletEntity*> deadBullets;
    for (BulletEntity* bullet : m_activeBullets) {
        if (bullet == nullptr) {
            continue;
        }
        if (!bullet->isAlive() || !bullet->isActive()) {
            deadBullets.append(bullet);
        }
    }

    for (BulletEntity* bullet : deadBullets) {
        if (bullet == nullptr) {
            continue;
        }

        EnemyEntity* enemyHit = bullet->target();
        const bool wasHit = bullet->hasHitTarget();

        unregisterBullet(bullet);

        /*
         * BulletEntity::hitTarget 已调用 markHitByBullet，此时敌机 destroyReason
         * 已是 HitByBullet 但仍在 active 列表里。confirmTargetDestroyed 会再次
         * 标记（幂等）、记分并最终从 active 列表中移除。
         */
        if (wasHit && enemyHit != nullptr && activeTargets().contains(enemyHit)) {
            destroyEnemyByBullet(enemyHit);
        }
    }

    /*
     * 敌机自身位置驱动 → 触发退场或被外部标记销毁，由基类统一回收。
     */
    TypingGameModelBase::update(deltaMs);
}

void SpaceWarModel::onTargetMatched(TypingTargetEntity* target)
{
    /*
     * 飞机大战的输入命中 = 锁定敌机；不直接销毁敌机，留给子弹击中时再销毁。
     * 控制器会通过 tryLockEnemyByLetter / handleLetterInput 等接口直接锁定，
     * 这里仍然兼容基类调用路径：标记锁定但不销毁。
     */
    if (target == nullptr) {
        return;
    }

    EnemyEntity* enemy = static_cast<EnemyEntity*>(target);
    if (!enemy->isLocked()) {
        enemy->setLocked(true);
        notifyEnemyLocked(enemy->letter());
    }
}

void SpaceWarModel::onTargetExited(TypingTargetEntity* target)
{
    /*
     * 飞机大战的退场 = 敌机从下方飞出屏幕，扣 1 点血并移除敌机。
     */
    if (target == nullptr) {
        return;
    }

    if (!activeTargets().contains(target)) {
        return;
    }

    if (m_signalBridge != nullptr) {
        const QRectF enemyRect = target->boundingRect();
        emit m_signalBridge->enemyCollidedWithPlayer(
            enemyRect.center(),
            enemyRect.size(),
            EnemyPlayerCollisionReason::EnemyExitedBottom);
    }

    const QChar letter = target->letter();

    removeTargetInternal(target);
    notifyTargetRemoved(letter);

    applyPlayerDamage(1);
}

bool SpaceWarModel::shouldGameOver() const
{
    /*
     * 飞机大战的死亡条件 = 玩家 HP 归零，由 applyPlayerDamage → onGameOver 主动触发，
     * 不再依赖 miss count。
     */
    return false;
}

bool SpaceWarModel::shouldCompleteLevel() const
{
    /*
     * 飞机大战不存在“关卡完成”，只有死亡和退出。
     */
    return false;
}

void SpaceWarModel::resetPlayerHealth()
{
    if (m_player == nullptr) {
        return;
    }

    const int maxHp = ConfigureSettings::instance().spacewar().playerCraft.maxHealth;
    m_player->setMaxHealth(maxHp);
    m_player->setHealth(maxHp);
}

void SpaceWarModel::clearBullets()
{
    for (BulletEntity* bullet : m_activeBullets) {
        if (bullet == nullptr) {
            continue;
        }
        bullet->setActive(false);
        bullet->setVisible(false);
        bullet->setAlive(false);
    }
    m_activeBullets.clear();
}

void SpaceWarModel::clearReward()
{
    if (m_activeReward != nullptr) {
        m_activeReward->setActive(false);
        m_activeReward->setVisible(false);
        m_activeReward->setAlive(false);
        m_activeReward = nullptr;
    }
}

void SpaceWarModel::notifySettingsChangedInternal(SettingKey key, int value, int extraValue)
{
    notifySettingsChanged(key, value, extraValue);
}

void SpaceWarModel::notifyPlayerHpChanged()
{
    GameEvent event;
    event.type = GameEventType::HpChanged;
    event.state = gameState();
    event.value = playerHealth();
    event.extra_value = playerMaxHealth();
    event.score = score();
    event.level = difficultyLevel();
    event.hp = playerHealth();
    notifyObservers(event);
}

void SpaceWarModel::notifyDifficultyUpgraded()
{
    GameEvent event;
    event.type = GameEventType::LevelChanged;
    event.state = gameState();
    event.value = difficultyLevel();
    event.extra_value = maxDifficultyLevel();
    event.score = score();
    event.level = difficultyLevel();
    event.hp = playerHealth();
    notifyObservers(event);
}

void SpaceWarModel::notifyRewardSpawned(const QString& word)
{
    Q_UNUSED(word);

    GameEvent event;
    event.type = GameEventType::EntitySpawned;
    event.state = gameState();
    event.value = (m_activeReward != nullptr) ? m_activeReward->word().size() : 0;
    event.extra_value = -1; /* -1 用于区分这是 reward 实体而非敌机 */
    event.score = score();
    event.level = difficultyLevel();
    event.hp = playerHealth();
    notifyObservers(event);
}

void SpaceWarModel::notifyRewardCompleted(const QString& word)
{
    Q_UNUSED(word);

    GameEvent event;
    event.type = GameEventType::EntityHit;
    event.state = gameState();
    event.value = -1;            /* 标识 reward 完成 */
    event.extra_value = playerMaxHealth();
    event.score = score();
    event.level = difficultyLevel();
    event.hp = playerHealth();
    notifyObservers(event);
}

void SpaceWarModel::notifyRewardExpired(const QString& word)
{
    Q_UNUSED(word);

    GameEvent event;
    event.type = GameEventType::EntityRemoved;
    event.state = gameState();
    event.value = -1;            /* 标识 reward 移除 */
    event.extra_value = playerMaxHealth();
    event.score = score();
    event.level = difficultyLevel();
    event.hp = playerHealth();
    notifyObservers(event);
}

void SpaceWarModel::notifyRewardInputProgress(QChar letter, int typedCount, int wordLength)
{
    GameEvent event;
    event.type = GameEventType::InputAccepted;
    event.state = gameState();
    event.letter = letter;
    event.value = typedCount;
    event.extra_value = wordLength;
    event.score = score();
    event.level = difficultyLevel();
    event.hp = playerHealth();
    notifyObservers(event);
}

void SpaceWarModel::notifyEnemyLocked(QChar letter)
{
    GameEvent event;
    event.type = GameEventType::InputAccepted;
    event.state = gameState();
    event.letter = letter;
    event.value = 0;
    event.extra_value = 0;
    event.score = score();
    event.level = difficultyLevel();
    event.hp = playerHealth();
    notifyObservers(event);
}

void SpaceWarModel::notifyShotFired(QChar letter)
{
    GameEvent event;
    event.type = GameEventType::EntitySpawned;
    event.state = gameState();
    event.letter = letter;
    event.value = m_activeBullets.size();
    event.extra_value = 1; /* 1 = 子弹标识 */
    event.score = score();
    event.level = difficultyLevel();
    event.hp = playerHealth();
    notifyObservers(event);
}

void SpaceWarModel::notifyPlayerDamaged(int damage, int hpAfter)
{
    GameEvent event;
    event.type = GameEventType::EntityMissed;
    event.state = gameState();
    event.value = damage;
    event.extra_value = hpAfter;
    event.score = score();
    event.level = difficultyLevel();
    event.hp = hpAfter;
    notifyObservers(event);

}

void SpaceWarModel::notifyPlayerHealed(int heal, int hpAfter)
{
    GameEvent event;
    event.type = GameEventType::EntityHit;
    event.state = gameState();
    event.value = heal;
    event.extra_value = hpAfter;
    event.score = score();
    event.level = difficultyLevel();
    event.hp = hpAfter;
    notifyObservers(event);
}

void SpaceWarModel::notifyPlayerDied()
{
    GameEvent event;
    event.type = GameEventType::HpChanged;
    event.state = gameState();
    event.value = 0;
    event.extra_value = playerMaxHealth();
    event.score = score();
    event.level = difficultyLevel();
    event.hp = 0;
    notifyObservers(event);
}
