#ifndef __TYPEGAME_SPACEWAR_CONTROLLER_H__
#define __TYPEGAME_SPACEWAR_CONTROLLER_H__

#include <QChar>
#include <QList>
#include <QObject>

#include "common/typing/typing_game_controller_base.h"

/* ------------------------------------------------------------------
 // 文件名     : spacewar_controller.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 飞机大战主控制器（生成 / 移动 / 命中 / 奖励 / 难度）
------------------------------------------------------------------ */

class BulletEntity;
class EnemyEntity;
class RewardWordEntity;
class SpaceWarModel;
class SpaceWarView;
class TypingTargetEntity;

/**
 * 飞机大战主控制器。
 *
 * 职责：
 *   1. 主循环：tick 推进模型、生成敌机 / 子弹 / 奖励、检查碰撞、推进难度。
 *   2. 输入：玩家方向键移动、字母锁定敌机、字母推进奖励单词。
 *   3. UI 信号绑定：开始 / 暂停 / 停止 / 退出 / 字母输入。
 *
 * 边界：
 *   1. 不负责具体的绘制。
 *   2. 不负责服务层（日志 / 埋点 / 音频 / AI 单词）的初始化，仅通过 SpaceWarGame 连接。
 *   3. 不直接持有视图。
 */
class SpaceWarController final : public QObject, public TypingGameControllerBase
{
    Q_OBJECT

public:
    explicit SpaceWarController(SpaceWarModel* model, QObject* parent = nullptr);
    ~SpaceWarController() override;

    SpaceWarController(const SpaceWarController&) = delete;
    SpaceWarController& operator=(const SpaceWarController&) = delete;

    void initialize() override;
    void startGame() override;
    void pauseGame() override;
    void resumeGame() override;
    void restartGame() override;
    void stopGame() override;

    void update(int delta_ms) override;
    bool handleLetterInput(QChar input_letter) override;

    void attachView(SpaceWarView* view);

    /* 玩家移动键持续状态 */
    void setMoveLeftPressed(bool pressed);
    void setMoveRightPressed(bool pressed);
    void setMoveUpPressed(bool pressed);
    void setMoveDownPressed(bool pressed);

    /* 设置触发的应用：当 spawn / speed / reward 设置发生变化时立即生效 */
    void applySettingsToRuntime();

signals:
    void requestStartUpdateLoop();
    void requestStopUpdateLoop();
    void requestExitToLauncher();
    void requestRewardWord();

private slots:
    void onViewStartClicked();
    void onViewRetryClicked();
    void onViewPauseResumeClicked();
    void onViewStopClicked();
    void onViewExitClicked();
    void onViewSettingsClicked();
    void onViewLetterTyped(QChar letter);
    void onViewMoveKeyChanged(int hAxis, int vAxis);

protected:
    void onUpdate(int delta_ms) override;
    void onSceneSizeChanged(const QRectF& scene_rect) override;

public:
    /**
     * AI 单词服务异步回调进来后调用：把单词包装成奖励实体并注册。
     */
    void provideRewardWord(const QString& word);

    /**
     * 命令行测试：敌机已由 SpaceWarModel::registerTarget 注册后，
     * 纳入控制器内存管理（与 trySpawnEnemy 生成路径一致）。
     */
    void adoptTestEnemy(EnemyEntity* enemy);

private:
    void bindViewConnections();
    void unbindViewConnections();

    void applyFixedLogicSceneGeometry();
    void applyPlayerMovementAxes();
    void resetTimers();

    /* 敌机生成 */
    void updateEnemySpawn(int delta_ms);
    bool trySpawnEnemy();
    EnemyEntity* createEnemy();
    QChar generateRandomLetter() const;
    QPointF generateEnemySpawnTopLeft(qreal entityWidth) const;

    /* 子弹生成 */
    BulletEntity* spawnBulletForEnemy(EnemyEntity* enemy);

    /* 奖励 */
    void updateRewardSpawn(int delta_ms);
    void updateRewardLifecycle();
    bool isRewardOutOfScene(const RewardWordEntity* reward) const;

    /* 碰撞 */
    void updateCollision();

    /* 难度 */
    void updateDifficulty(int delta_ms);

    /* 清理 */
    void clearOwnedTargets();
    void clearOwnedBullets();
    void clearOwnedReward();
    void recycleInactiveOwnedTargets();
    void recycleInactiveOwnedBullets();
    void recycleConsumedReward();

    /* 安全：在删除已退场敌机前，先把指向它的子弹 target 置空 */
    void invalidateBulletsTargetingRemovedEnemies();

private:
    SpaceWarModel* m_spaceWarModel = nullptr;
    SpaceWarView* m_view = nullptr;
    bool m_viewBindingsConnected = false;

    int m_spawnElapsedMs = 0;
    int m_rewardElapsedMs = 0;
    int m_difficultyElapsedMs = 0;

    QList<EnemyEntity*> m_ownedEnemies;
    QList<BulletEntity*> m_ownedBullets;
    RewardWordEntity* m_ownedReward = nullptr;

    bool m_moveLeftPressed = false;
    bool m_moveRightPressed = false;
    bool m_moveUpPressed = false;
    bool m_moveDownPressed = false;

    bool m_rewardRequestInFlight = false;
};

#endif // __TYPEGAME_SPACEWAR_CONTROLLER_H__
