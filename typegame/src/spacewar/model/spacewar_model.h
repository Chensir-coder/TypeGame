#ifndef __TYPEGAME_SPACEWAR_MODEL_H__
#define __TYPEGAME_SPACEWAR_MODEL_H__

#include <memory>

#include <QList>
#include <QString>

#include "common/typing/typing_game_model_base.h"

/* ------------------------------------------------------------------
 // 文件名     : spacewar_model.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 飞机大战业务模型
------------------------------------------------------------------ */

class BulletEntity;
class EnemyEntity;
class PlayerEntity;
class RewardWordEntity;
class SpaceWarModelSignalBridge;

/**
 * 飞机大战核心业务模型。
 *
 * 在通用 TypingGameModelBase 之上扩展：
 * 1. 玩家飞机生命值、最大生命值（默认 18）。
 * 2. 子弹列表与奖励单词实体。
 * 3. 难度等级（独立于 speedLevel，由控制器定时升级）。
 * 4. spacewar 专用设置：
 *    - 最大同屏敌机数量（1-10）
 *    - 敌机速度等级（1-10）
 *    - 难度升级间隔（秒）
 *    - 奖励模式开关
 *    - 音效开关（与基类 soundEnabled 同步）
 * 5. 自定义命中规则：
 *    - 输入命中后只锁定敌机，不直接销毁；由控制器负责发射子弹。
 *    - 退场不算 miss，直接安全回收，不扣血。
 */
class SpaceWarModel final : public TypingGameModelBase
{
public:
    SpaceWarModel();
    ~SpaceWarModel() override;

    SpaceWarModel(const SpaceWarModel&) = delete;
    SpaceWarModel& operator=(const SpaceWarModel&) = delete;

    SpaceWarModelSignalBridge* signalBridge() const;

    void initialize() override;
    void resetModel() override;
    void update(int deltaMs) override;

    /* ---------------- player ---------------- */
    PlayerEntity* player() const;
    void preparePlayerCraftAfterSceneFix();

    int playerHealth() const;
    int playerMaxHealth() const;

    /**
     * 玩家受到一次撞击伤害。
     * @return 实际扣除的血量
     */
    int applyPlayerDamage(int amount = 1);
    int applyPlayerHeal(int amount = 1);

    /* ---------------- enemies ---------------- */

    /**
     * 查找未锁定且字母匹配的敌机，找到则锁定后返回。
     * 同一字母只能锁定一次。
     */
    EnemyEntity* tryLockEnemyByLetter(QChar letter);

    /**
     * 把敌机标记为被子弹击中并立即销毁，记录得分与事件。
     */
    void destroyEnemyByBullet(EnemyEntity* enemy);

    /**
     * 敌机与玩家发生碰撞：扣血、销毁敌机、记录事件。
     */
    void destroyEnemyByCollision(EnemyEntity* enemy);

    /* ---------------- bullets ---------------- */
    const QList<BulletEntity*>& activeBullets() const;
    bool registerBullet(BulletEntity* bullet);
    bool unregisterBullet(BulletEntity* bullet);

    /* ---------------- reward word ---------------- */
    RewardWordEntity* activeReward() const;

    /**
     * 注册一个新的奖励单词实体；同时只允许存在一个。
     */
    bool registerReward(RewardWordEntity* reward);

    /**
     * 接收键盘输入并尝试推进奖励单词。
     * @return 是否被消费（输入字符与下一字母匹配）
     */
    bool advanceRewardInput(QChar input);

    /**
     * 奖励完成时的回血与事件通知。
     */
    void completeReward();

    /**
     * 奖励飞出 / 主动取消。
     */
    void expireReward();

    /* ---------------- difficulty / settings ---------------- */
    int difficultyLevel() const;
    void setDifficultyLevel(int level);
    int maxDifficultyLevel() const;

    /**
     * 推进一级难度并广播事件。
     * @return 是否真正升级（达到上限时返回 false）
     */
    bool upgradeDifficulty();

    /* spacewar 专属设置项 */
    int maxEnemyCount() const;
    void setMaxEnemyCount(int count);

    int enemySpeedLevel() const;
    void setEnemySpeedLevel(int level);

    int difficultyUpgradeIntervalSec() const;
    void setDifficultyUpgradeIntervalSec(int seconds);

    bool rewardModeEnabled() const;
    void setRewardModeEnabled(bool enabled);

    /**
     * 同步 spacewar 自身保存的音效开关到基类 config().soundEnabled。
     */
    void setSpaceWarSoundEnabled(bool enabled);
    bool spaceWarSoundEnabled() const;

    /* ---------------- runtime queries ---------------- */
    int currentEnemyMoveSpeed() const;
    int currentEnemySpawnIntervalMs() const;

    /**
     * 命令行 / 自动化测试：为 true 时控制器不再按难度表自动生成敌机，
     * 避免与手动注入的目标抢占字母或干扰碰撞。（不影响 registerTarget）
     */
    void setSuppressAutoEnemySpawn(bool suppress);
    bool suppressAutoEnemySpawn() const;

    int currentTargetMoveSpeed() const override;

    int score() const;

    /* spacewar 自定义事件类型，借用 GameEvent.value */
    void notifyPlayerHpChanged();
    void notifyDifficultyUpgraded();
    void notifyRewardSpawned(const QString& word);
    void notifyRewardCompleted(const QString& word);
    void notifyRewardExpired(const QString& word);
    void notifyRewardInputProgress(QChar letter, int typedCount, int wordLength);
    void notifyEnemyLocked(QChar letter);
    void notifyShotFired(QChar letter);
    void notifyPlayerDamaged(int damage, int hpAfter);
    void notifyPlayerHealed(int heal, int hpAfter);
    void notifyPlayerDied();

    /**
     * 仅清空模型侧登记与列表，不释放实体内存；控制器在 delete 子弹/奖励前应调用，
     * 避免视图与模型仍遍历已释放指针。
     */
    void clearBullets();
    void clearReward();

protected:
    /**
     * 输入命中后只锁定敌机；真正销毁交给子弹命中流程。
     */
    void onTargetMatched(TypingTargetEntity* target) override;

    /**
     * 飞机大战中“退场”等于自然飞出场景，不算 miss。
     */
    void onTargetExited(TypingTargetEntity* target) override;

    bool shouldGameOver() const override;
    bool shouldCompleteLevel() const override;

private:
    void resetPlayerHealth();
    void notifySettingsChangedInternal(SettingKey key, int value, int extraValue = 0);

private:
    std::unique_ptr<SpaceWarModelSignalBridge> m_signalBridge;
    std::unique_ptr<PlayerEntity> m_player;

    QList<BulletEntity*> m_activeBullets;
    RewardWordEntity* m_activeReward = nullptr;

    int m_difficultyLevel = 1; // 难度等级
    int m_maxDifficultyLevel = 10; // 最大难度等级

    int m_maxEnemyCount = 3; // 最大敌机数量
    int m_enemySpeedLevel = 1; // 敌机速度等级（与设置面板默认一致；1 时仅用 baseSpeed，在 tall 场景下会很慢）
    int m_difficultyUpgradeIntervalSec = 10; // 难度升级间隔
    bool m_rewardModeEnabled = true; // 奖励模式开关
    bool m_spaceWarSoundEnabled = true; // 音效开关
    bool m_suppressAutoEnemySpawn = false;
};

#endif // __TYPEGAME_SPACEWAR_MODEL_H__
