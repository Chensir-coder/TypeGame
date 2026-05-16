/* ------------------------------------------------------------------
 // 文件名     : spacewar_controller.cpp
------------------------------------------------------------------ */

#include "spacewar_controller.h"

#include <QRandomGenerator>
#include <QSizeF>
#include <QtMath>

#include "common/core/game_state.h"
#include "common/typing/typing_target_entity.h"
#include "spacewar/model/entity/bullet_entity.h"
#include "spacewar/model/entity/enemy_entity.h"
#include "spacewar/model/entity/player_entity.h"
#include "spacewar/model/entity/reward_word_entity.h"
#include "spacewar/model/spacewar_model.h"
#include "spacewar/model/spacewar_model_signal_bridge.h"
#include "spacewar/view/spacewar_view.h"
#include "common/service/config/configure_settings.h"

SpaceWarController::SpaceWarController(SpaceWarModel* model, QObject* parent)
    : QObject(parent)
    , TypingGameControllerBase(model)
    , m_spaceWarModel(model)
{
}

SpaceWarController::~SpaceWarController()
{
    unbindViewConnections();
    clearOwnedReward();
    clearOwnedBullets();
    clearOwnedTargets();
}

void SpaceWarController::initialize()
{
    TypingGameControllerBase::initialize();

    applyFixedLogicSceneGeometry();
    resetTimers();
    clearOwnedReward();
    clearOwnedBullets();
    clearOwnedTargets();
}

void SpaceWarController::startGame()
{
    TypingGameControllerBase::startGame();
    resetTimers();
}

void SpaceWarController::pauseGame()
{
    TypingGameControllerBase::pauseGame();
}

void SpaceWarController::resumeGame()
{
    TypingGameControllerBase::resumeGame();
}

void SpaceWarController::restartGame()
{
    resetTimers();
    clearOwnedReward();
    clearOwnedBullets();
    clearOwnedTargets();

    TypingGameControllerBase::restartGame();
    applyFixedLogicSceneGeometry();
}

void SpaceWarController::stopGame()
{
    resetTimers();
    TypingGameControllerBase::stopGame();
    clearOwnedReward();
    clearOwnedBullets();
    clearOwnedTargets();
}

void SpaceWarController::update(int delta_ms)
{
    /*
     * 暂停 / 结束状态下不推进任何游戏逻辑，保留画面。
     */
    if (m_spaceWarModel == nullptr) {
        return;
    }

    if (m_spaceWarModel->gameState() != GameState::Playing) {
        return;
    }

    /*
     * 玩家移动方向输入每帧应用一次，确保按住时持续移动。
     */
    applyPlayerMovementAxes();

    TypingGameControllerBase::update(delta_ms);
}

bool SpaceWarController::handleLetterInput(QChar input_letter)
{
    if (m_spaceWarModel == nullptr) {
        return false;
    }

    if (m_spaceWarModel->gameState() != GameState::Playing) {
        return false;
    }

    const QChar normalized = input_letter.toUpper();

    /* 1) 奖励单词优先处理 */
    if (m_spaceWarModel->activeReward() != nullptr) {
        if (m_spaceWarModel->advanceRewardInput(normalized)) {
            m_spaceWarModel->stats().recordTypingKeystroke(true);
            recycleConsumedReward();
            return true;
        }
        /*
         * 奖励单词不匹配时按规范不重置进度，继续走敌机锁定。
         */
    }

    /* 2) 锁定敌机 + 发射追踪子弹 */
    EnemyEntity* enemy = m_spaceWarModel->tryLockEnemyByLetter(normalized);
    if (enemy != nullptr) {
        m_spaceWarModel->stats().recordTypingKeystroke(true);
        spawnBulletForEnemy(enemy);
        return true;
    }

    /* 3) 没有匹配，忽略输入；不视作 miss，也不扣血 */
    m_spaceWarModel->stats().recordTypingKeystroke(false);
    return false;
}

void SpaceWarController::attachView(SpaceWarView* view)
{
    if (m_view == view) {
        return;
    }

    unbindViewConnections();
    m_view = view;

    if (m_view != nullptr) {
        bindViewConnections();
    }
}

void SpaceWarController::setMoveLeftPressed(bool pressed)
{
    m_moveLeftPressed = pressed;
}

void SpaceWarController::setMoveRightPressed(bool pressed)
{
    m_moveRightPressed = pressed;
}

void SpaceWarController::setMoveUpPressed(bool pressed)
{
    m_moveUpPressed = pressed;
}

void SpaceWarController::setMoveDownPressed(bool pressed)
{
    m_moveDownPressed = pressed;
}

void SpaceWarController::applySettingsToRuntime()
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    /*
     * 重新写回到 base config：MaxTarget / SpeedLevel / Sound 已在 setter 内同步。
     * 这里再保险地刷一遍 spawnInterval，并同步当前敌机速度。
     */
    m_spaceWarModel->config().setSpawnIntervalMs(
        m_spaceWarModel->currentEnemySpawnIntervalMs());
}

void SpaceWarController::onUpdate(int delta_ms)
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    if (m_spaceWarModel->gameState() != GameState::Playing) {
        return;
    }

    /*
     * 按顺序：
     *   1. 难度计时（在生成之前升级，让生成立即受新难度影响）
     *   2. 敌机生成
     *   3. 奖励生成
     *   4. 碰撞检测
     *   5. 资源回收
     */
    updateDifficulty(delta_ms);
    updateEnemySpawn(delta_ms);
    updateRewardSpawn(delta_ms);
    updateRewardLifecycle();
    updateCollision();

    /*
     * 在删除敌机之前，先把指向已退出 active 列表的敌机的子弹 target 置空，
     * 避免下一帧 bullet.update() 解引用悬空指针。
     */
    invalidateBulletsTargetingRemovedEnemies();

    recycleInactiveOwnedTargets();
    recycleInactiveOwnedBullets();
    recycleConsumedReward();
}

void SpaceWarController::onSceneSizeChanged(const QRectF& scene_rect)
{
    Q_UNUSED(scene_rect);

    if (m_spaceWarModel != nullptr) {
        m_spaceWarModel->preparePlayerCraftAfterSceneFix();
    }
}

void SpaceWarController::adoptTestEnemy(EnemyEntity* enemy)
{
    if (enemy == nullptr || m_spaceWarModel == nullptr) {
        return;
    }

    if (!m_spaceWarModel->activeTargets().contains(enemy)) {
        return;
    }

    if (m_ownedEnemies.contains(enemy)) {
        return;
    }

    m_ownedEnemies.append(enemy);
}

void SpaceWarController::provideRewardWord(const QString& word)
{
    m_rewardRequestInFlight = false;

    if (m_spaceWarModel == nullptr) {
        return;
    }

    if (m_spaceWarModel->gameState() != GameState::Playing) {
        return;
    }

    if (!m_spaceWarModel->rewardModeEnabled()) {
        return;
    }

    if (m_spaceWarModel->activeReward() != nullptr) {
        return;
    }

    if (word.trimmed().isEmpty()) {
        return;
    }

    const QString normalized = word.trimmed().toUpper();

    const auto& rw = ConfigureSettings::instance().spacewar().reward;

    RewardWordEntity* reward = new RewardWordEntity();
    reward->setWord(normalized);
    reward->setSize(QSizeF(rw.width, rw.height));
    reward->setHealValue(rw.healValue);
    reward->setScoreValue(rw.scoreValue);

    const qreal sceneW = static_cast<qreal>(m_spaceWarModel->config().sceneWidth());
    const qreal sceneH = static_cast<qreal>(m_spaceWarModel->config().sceneHeight());
    reward->setSceneRect(QRectF(0.0, 0.0, sceneW, sceneH));

    /* 随机从左侧或右侧出现 */
    const bool fromLeft = QRandomGenerator::global()->bounded(2) == 0;
    const qreal yLow = sceneH * 0.18;
    const qreal yHigh = sceneH * 0.45;
    const qreal y =
        yLow + (yHigh - yLow) * QRandomGenerator::global()->generateDouble();
    if (fromLeft) {
        reward->setPosition(QPointF(-rw.width, y));
        reward->setVelocity(QPointF(rw.horizontalSpeed, 0.0));
    } else {
        reward->setPosition(QPointF(sceneW, y));
        reward->setVelocity(QPointF(-rw.horizontalSpeed, 0.0));
    }
    reward->setOutMargin(rw.width);

    if (!m_spaceWarModel->registerReward(reward)) {
        delete reward;
        return;
    }

    if (m_ownedReward != nullptr) {
        delete m_ownedReward;
    }
    m_ownedReward = reward;
}

void SpaceWarController::onViewStartClicked()
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    const GameState s = m_spaceWarModel->gameState();
    if (s == GameState::Playing) {
        return;
    }

    if (s == GameState::End) {
        restartGame();
    } else if (s == GameState::Initial) {
        startGame();
    } else {
        return;
    }

    if (m_spaceWarModel->gameState() == GameState::Playing) {
        emit requestStartUpdateLoop();
    }
}

void SpaceWarController::onViewRetryClicked()
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    /*
     * 失败弹窗里的“再来一局”应直接重开，不走开始/暂停切换分支。
     */
    restartGame();
    if (m_spaceWarModel->gameState() == GameState::Playing) {
        emit requestStartUpdateLoop();
    }
}

void SpaceWarController::onViewPauseResumeClicked()
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    if (m_spaceWarModel->gameState() == GameState::Playing) {
        pauseGame();
        emit requestStopUpdateLoop();
        return;
    }

    if (m_spaceWarModel->gameState() == GameState::Paused) {
        resumeGame();
        if (m_spaceWarModel->gameState() == GameState::Playing) {
            emit requestStartUpdateLoop();
        }
    }
}

void SpaceWarController::onViewStopClicked()
{
    stopGame();
    emit requestStopUpdateLoop();
}

void SpaceWarController::onViewExitClicked()
{
    emit requestExitToLauncher();
}

void SpaceWarController::onViewSettingsClicked()
{
    /*
     * 设置完成后由 SpaceWarGame 调用 applySettingsToRuntime()。
     * 控制器自身不负责弹窗。
     */
    applySettingsToRuntime();
}

void SpaceWarController::onViewLetterTyped(QChar letter)
{
    handleLetterInput(letter);
}

void SpaceWarController::onViewMoveKeyChanged(int hAxis, int vAxis)
{
    m_moveLeftPressed = hAxis < 0;
    m_moveRightPressed = hAxis > 0;
    m_moveUpPressed = vAxis < 0;
    m_moveDownPressed = vAxis > 0;
    applyPlayerMovementAxes();
}

void SpaceWarController::bindViewConnections()
{
    if (m_view == nullptr || m_spaceWarModel == nullptr || m_viewBindingsConnected) {
        return;
    }

    SpaceWarModelSignalBridge* hub = m_spaceWarModel->signalBridge();
    if (hub == nullptr) {
        return;
    }

    QObject::connect(hub, &SpaceWarModelSignalBridge::modelChanged,
                     m_view, &SpaceWarView::onModelSignalBridgeChanged);

    QObject::connect(hub, &SpaceWarModelSignalBridge::dialogGameOverRequested,
                     m_view, &SpaceWarView::onDialogGameOverRequested);

    QObject::connect(hub, &SpaceWarModelSignalBridge::dialogLevelCompletedRequested,
                     m_view, &SpaceWarView::onDialogLevelCompletedRequested);

    QObject::connect(hub, &SpaceWarModelSignalBridge::enemyHit,
                     m_view, &SpaceWarView::onEnemyHitByBullet);

    QObject::connect(hub, &SpaceWarModelSignalBridge::rewardCompleted,
                     m_view, &SpaceWarView::onRewardCompleted);

    QObject::connect(hub, &SpaceWarModelSignalBridge::rewardExpired,
                     m_view, &SpaceWarView::onRewardExpired);

    QObject::connect(hub, &SpaceWarModelSignalBridge::difficultyUpgraded,
                     m_view, &SpaceWarView::onDifficultyUpgraded);

    QObject::connect(hub, &SpaceWarModelSignalBridge::enemyCollidedWithPlayer,
                     m_view, &SpaceWarView::onEnemyCollidedWithPlayer);

    QObject::connect(m_view, &SpaceWarView::startClicked,
                     this, &SpaceWarController::onViewStartClicked);

    QObject::connect(m_view, &SpaceWarView::retryClicked,
                     this, &SpaceWarController::onViewRetryClicked);

    QObject::connect(m_view, &SpaceWarView::pauseResumeClicked,
                     this, &SpaceWarController::onViewPauseResumeClicked);

    QObject::connect(m_view, &SpaceWarView::stopClicked,
                     this, &SpaceWarController::onViewStopClicked);

    QObject::connect(m_view, &SpaceWarView::exitClicked,
                     this, &SpaceWarController::onViewExitClicked);

    QObject::connect(m_view, &SpaceWarView::letterTyped,
                     this, &SpaceWarController::onViewLetterTyped);

    QObject::connect(m_view, &SpaceWarView::moveKeyChanged,
                     this, &SpaceWarController::onViewMoveKeyChanged);

    m_viewBindingsConnected = true;
}

void SpaceWarController::unbindViewConnections()
{
    if (m_view != nullptr && m_spaceWarModel != nullptr
        && m_spaceWarModel->signalBridge() != nullptr)
    {
        SpaceWarModelSignalBridge* hub = m_spaceWarModel->signalBridge();
        QObject::disconnect(hub, nullptr, m_view, nullptr);
    }

    if (m_view != nullptr) {
        QObject::disconnect(m_view, nullptr, this, nullptr);
        QObject::disconnect(this, nullptr, m_view, nullptr);
    }

    m_view = nullptr;
    m_viewBindingsConnected = false;
}

void SpaceWarController::applyFixedLogicSceneGeometry()
{
    const auto& scene = ConfigureSettings::instance().spacewar().scene;
    setSceneSize(scene.width, scene.height);
}

void SpaceWarController::applyPlayerMovementAxes()
{
    if (m_spaceWarModel == nullptr || m_spaceWarModel->player() == nullptr) {
        return;
    }

    const int h = (m_moveLeftPressed ? -1 : 0) + (m_moveRightPressed ? 1 : 0);
    const int v = (m_moveUpPressed ? -1 : 0) + (m_moveDownPressed ? 1 : 0);

    m_spaceWarModel->player()->setHorizontalAxis(h);
    m_spaceWarModel->player()->setVerticalAxis(v);
}

void SpaceWarController::resetTimers()
{
    m_spawnElapsedMs = 0;
    m_rewardElapsedMs = 0;
    m_difficultyElapsedMs = 0;
    m_rewardRequestInFlight = false;
}

void SpaceWarController::updateEnemySpawn(int delta_ms)
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    if (m_spaceWarModel->suppressAutoEnemySpawn()) {
        return;
    }

    m_spawnElapsedMs += delta_ms;

    const int spawnInterval = m_spaceWarModel->currentEnemySpawnIntervalMs();
    if (m_spawnElapsedMs < spawnInterval) {
        return;
    }

    m_spawnElapsedMs = 0;
    trySpawnEnemy();
}

bool SpaceWarController::trySpawnEnemy()
{
    if (m_spaceWarModel == nullptr) {
        return false;
    }

    if (!m_spaceWarModel->canSpawnTarget()) {
        return false;
    }

    EnemyEntity* enemy = createEnemy();
    if (enemy == nullptr) {
        return false;
    }

    const bool registered = m_spaceWarModel->registerTarget(enemy);
    if (!registered) {
        delete enemy;
        return false;
    }

    m_ownedEnemies.append(enemy);
    return true;
}

EnemyEntity* SpaceWarController::createEnemy()
{
    if (m_spaceWarModel == nullptr) {
        return nullptr;
    }

    const QChar letter = generateRandomLetter();
    if (letter.isNull()) {
        return nullptr;
    }

    const auto& cfg = m_spaceWarModel->config();
    const qreal width = qMax<qreal>(80.0, cfg.typingTargetLogicalWidth());
    const qreal height = qMax<qreal>(80.0, cfg.typingTargetLogicalHeight());
    const qreal verticalSpeed = static_cast<qreal>(m_spaceWarModel->currentEnemyMoveSpeed());
    const auto& tr = ConfigureSettings::instance().spacewar().trajectory;

    EnemyEntity* enemy = new EnemyEntity();
    enemy->setLetter(letter);
    enemy->setScoreValue(10);
    enemy->setSize(QSizeF(width, height));
    enemy->setLocked(false);

    const qreal sw = static_cast<qreal>(cfg.sceneWidth());
    const qreal sh = static_cast<qreal>(cfg.sceneHeight());
    enemy->setSceneRect(QRectF(0.0, 0.0, sw, sh));

    /* 随机选择敌机外观与轨迹 */
    enemy->setVariant(QRandomGenerator::global()->bounded(2) == 0
                          ? EnemyEntity::Variant::Type0
                          : EnemyEntity::Variant::Type4);

    const QPointF spawnTopLeft = generateEnemySpawnTopLeft(width);
    enemy->setPosition(spawnTopLeft);

    const int trajectoryRoll = QRandomGenerator::global()->bounded(3);
    const qreal startX = spawnTopLeft.x();

    if (trajectoryRoll == 0) {
        enemy->configureStraight(startX, verticalSpeed);
    } else if (trajectoryRoll == 1) {
        const qreal pad = ConfigureSettings::instance().spacewar().enemySpawn.horizontalPadding;
        const qreal availableHalfWidth =
            qMax<qreal>(tr.sineAmplitudeMin,
                        qMin(startX - pad, sw - (startX + width) - pad));
        const qreal amplitude =
            qMin<qreal>(tr.sineAmplitudeMax,
                        qMax<qreal>(tr.sineAmplitudeMin, availableHalfWidth));
        const qreal angularSpeed = tr.sineAngularSpeed;
        const qreal phase = QRandomGenerator::global()->generateDouble() * (2.0 * M_PI);
        enemy->configureSine(startX, verticalSpeed, amplitude, angularSpeed, phase);
    } else {
        const qreal hSpeed =
            (QRandomGenerator::global()->bounded(2) == 0 ? -1.0 : 1.0)
            * (tr.bounceHorizontalBase
               + static_cast<qreal>(QRandomGenerator::global()->bounded(
                   static_cast<int>(tr.bounceHorizontalRandom + 0.5))));
        enemy->configureBounce(startX, verticalSpeed, hSpeed);
    }

    const int frameCount =
        qMax(1, cfg.enemySpriteSheetCols() * cfg.enemySpriteSheetRows());
    enemy->configureSpriteAnimation(frameCount, cfg.enemySpriteAnimationFps());

    return enemy;
}

QChar SpaceWarController::generateRandomLetter() const
{
    if (m_spaceWarModel == nullptr) {
        return QChar();
    }

    QList<QChar> available;
    for (char c = 'A'; c <= 'Z'; ++c) {
        const QChar letter(c);
        if (m_spaceWarModel->isLetterAvailable(letter)) {
            available.append(letter);
        }
    }

    if (available.isEmpty()) {
        return QChar();
    }

    const int idx = QRandomGenerator::global()->bounded(available.size());
    return available.at(idx);
}

QPointF SpaceWarController::generateEnemySpawnTopLeft(qreal entityWidth) const
{
    if (m_spaceWarModel == nullptr) {
        return QPointF(0.0, 0.0);
    }

    const QRectF rect = sceneRect();
    const qreal sw = rect.width();

    const qreal pad = ConfigureSettings::instance().spacewar().enemySpawn.horizontalPadding;
    const qreal minLeft = pad;
    const qreal maxLeft = qMax(minLeft, sw - pad - entityWidth);
    const qreal x = (maxLeft <= minLeft)
        ? minLeft
        : (minLeft + QRandomGenerator::global()->generateDouble() * (maxLeft - minLeft));

    const qreal y = ConfigureSettings::instance().spacewar().enemySpawn.yOffset;
    return QPointF(x, y);
}

BulletEntity* SpaceWarController::spawnBulletForEnemy(EnemyEntity* enemy)
{
    if (enemy == nullptr || m_spaceWarModel == nullptr) {
        return nullptr;
    }

    PlayerEntity* player = m_spaceWarModel->player();
    if (player == nullptr) {
        return nullptr;
    }

    const auto& bul = ConfigureSettings::instance().spacewar().bullet;

    BulletEntity* bullet = new BulletEntity();
    bullet->setSize(QSizeF(bul.width, bul.height));

    const QPointF muzzle = player->gunMuzzlePosition();
    bullet->setPosition(QPointF(muzzle.x() - bul.width / 2.0,
                                muzzle.y() - bul.height / 2.0));
    bullet->setSpeed(bul.speed);
    bullet->setHitRadius(qMax<qreal>(bul.hitRadiusMin, bul.width / 2.0));
    bullet->setTarget(enemy);

    if (!m_spaceWarModel->registerBullet(bullet)) {
        delete bullet;
        return nullptr;
    }

    m_ownedBullets.append(bullet);

    m_spaceWarModel->notifyShotFired(enemy->letter());
    return bullet;
}

void SpaceWarController::updateRewardSpawn(int delta_ms)
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    if (!m_spaceWarModel->rewardModeEnabled()) {
        return;
    }

    if (m_spaceWarModel->activeReward() != nullptr) {
        return;
    }

    m_rewardElapsedMs += delta_ms;
    const int rewardIntervalMs =
        ConfigureSettings::instance().spacewar().reward.spawnIntervalMs;
    if (m_rewardElapsedMs < rewardIntervalMs) {
        return;
    }

    /*
     * 计时已满，请求一个奖励单词；若上一次请求还没回来则等待。
     */
    if (!m_rewardRequestInFlight) {
        m_rewardElapsedMs = 0;
        m_rewardRequestInFlight = true;
        emit requestRewardWord();
    }
}

void SpaceWarController::updateRewardLifecycle()
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    if (m_ownedReward == nullptr) {
        return;
    }

    /* 模型在 update() 时已检测 expired，奖励飞出后会清掉模型引用 */
    if (m_spaceWarModel->activeReward() == nullptr) {
        delete m_ownedReward;
        m_ownedReward = nullptr;
    }
}

bool SpaceWarController::isRewardOutOfScene(const RewardWordEntity* reward) const
{
    if (reward == nullptr) {
        return true;
    }
    return reward->isExpired();
}

void SpaceWarController::updateCollision()
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    PlayerEntity* player = m_spaceWarModel->player();
    if (player == nullptr || !player->isActive() || player->isDead()) {
        return;
    }

    const QRectF playerRect = player->boundingRect();

    QList<EnemyEntity*> collided;
    for (TypingTargetEntity* t : m_spaceWarModel->activeTargets()) {
        if (t == nullptr) {
            continue;
        }
        EnemyEntity* enemy = static_cast<EnemyEntity*>(t);
        if (!enemy->isActive() || enemy->isDestroyed()) {
            continue;
        }
        if (enemy->boundingRect().intersects(playerRect)) {
            collided.append(enemy);
        }
    }

    SpaceWarModelSignalBridge* hub = m_spaceWarModel->signalBridge();
    for (EnemyEntity* enemy : collided) {
        if (hub != nullptr && enemy != nullptr) {
            const QRectF enemyRect = enemy->boundingRect();
            emit hub->enemyCollidedWithPlayer(enemyRect.center(),
                                              enemyRect.size(),
                                              EnemyPlayerCollisionReason::BoundingOverlap);
        }
        m_spaceWarModel->applyPlayerDamage(1);
        m_spaceWarModel->destroyEnemyByCollision(enemy);

        if (m_spaceWarModel->gameState() != GameState::Playing) {
            break;
        }
    }
}

void SpaceWarController::updateDifficulty(int delta_ms)
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    const int interval =
        qMax(1, m_spaceWarModel->difficultyUpgradeIntervalSec()) * 1000;

    m_difficultyElapsedMs += delta_ms;
    if (m_difficultyElapsedMs < interval) {
        return;
    }

    m_difficultyElapsedMs = 0;
    m_spaceWarModel->upgradeDifficulty();
    m_spaceWarModel->config().setSpawnIntervalMs(
        m_spaceWarModel->currentEnemySpawnIntervalMs());
}

void SpaceWarController::clearOwnedTargets()
{
    if (m_spaceWarModel != nullptr) {
        m_spaceWarModel->clearActiveTargets();
    }
    for (EnemyEntity* enemy : m_ownedEnemies) {
        delete enemy;
    }
    m_ownedEnemies.clear();
}

void SpaceWarController::clearOwnedBullets()
{
    if (m_spaceWarModel != nullptr) {
        m_spaceWarModel->clearBullets();
    }
    for (BulletEntity* bullet : m_ownedBullets) {
        delete bullet;
    }
    m_ownedBullets.clear();
}

void SpaceWarController::clearOwnedReward()
{
    if (m_spaceWarModel != nullptr) {
        m_spaceWarModel->clearReward();
    }
    if (m_ownedReward != nullptr) {
        delete m_ownedReward;
        m_ownedReward = nullptr;
    }
    m_rewardRequestInFlight = false;
}

void SpaceWarController::recycleInactiveOwnedTargets()
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    for (int i = m_ownedEnemies.size() - 1; i >= 0; --i) {
        EnemyEntity* enemy = m_ownedEnemies.at(i);
        if (enemy == nullptr) {
            m_ownedEnemies.removeAt(i);
            continue;
        }

        /* 仍在模型 active 列表里就保留 */
        if (m_spaceWarModel->activeTargets().contains(enemy)) {
            continue;
        }

        delete enemy;
        m_ownedEnemies.removeAt(i);
    }
}

void SpaceWarController::recycleInactiveOwnedBullets()
{
    if (m_spaceWarModel == nullptr) {
        return;
    }

    for (int i = m_ownedBullets.size() - 1; i >= 0; --i) {
        BulletEntity* bullet = m_ownedBullets.at(i);
        if (bullet == nullptr) {
            m_ownedBullets.removeAt(i);
            continue;
        }

        if (m_spaceWarModel->activeBullets().contains(bullet)) {
            continue;
        }

        delete bullet;
        m_ownedBullets.removeAt(i);
    }
}

void SpaceWarController::recycleConsumedReward()
{
    if (m_ownedReward == nullptr || m_spaceWarModel == nullptr) {
        return;
    }

    if (m_spaceWarModel->activeReward() == m_ownedReward) {
        return;
    }

    delete m_ownedReward;
    m_ownedReward = nullptr;
}

void SpaceWarController::invalidateBulletsTargetingRemovedEnemies()
{
    if (m_spaceWarModel == nullptr) {
        return;
    }
    for (BulletEntity* bullet : m_ownedBullets) {
        if (bullet == nullptr) {
            continue;
        }
        EnemyEntity* tgt = bullet->target();
        if (tgt == nullptr) {
            continue;
        }
        if (!m_spaceWarModel->activeTargets().contains(tgt)) {
            bullet->setTarget(nullptr);
        }
    }
}
