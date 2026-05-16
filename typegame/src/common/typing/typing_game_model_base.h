#ifndef __TYPEGAME_TYPING_GAME_MODEL_BASE_H__
#define __TYPEGAME_TYPING_GAME_MODEL_BASE_H__

#include <QChar>
#include <QList>
#include <QSet>

#include "common/core/game_event.h"
#include "common/core/game_event_subject_base.h"
#include "common/core/game_state.h"
#include "typing_game_config.h"
#include "typing_game_stats.h"
#include "typing_target_entity.h"

/* ------------------------------------------------------------------
 // 文件名     : typing_game_model_base.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : typing_game_model_base 头文件声明
------------------------------------------------------------------ */

class TypingGameModelBase : public GameEventSubjectBase
{
public:
    TypingGameModelBase();
    ~TypingGameModelBase() override = default;

    TypingGameModelBase(const TypingGameModelBase&) = delete;
    TypingGameModelBase& operator=(const TypingGameModelBase&) = delete;

    virtual void initialize();
    virtual void resetModel();

    virtual void startGame();
    virtual void pauseGame();
    virtual void resumeGame();
    virtual void stopGame();

    virtual void update(int deltaMs);

    bool registerTarget(TypingTargetEntity* target);
    bool unregisterTarget(TypingTargetEntity* target);

    bool handleInput(QChar inputLetter);
    TypingTargetEntity* findTargetByLetter(QChar letter) const;

    bool canSpawnTarget() const;
    bool isLetterAvailable(QChar letter) const;
    void clearActiveTargets();

    GameState gameState() const;

    TypingGameConfig& config();
    const TypingGameConfig& config() const;

    TypingGameStats& stats();
    const TypingGameStats& stats() const;

    const QList<TypingTargetEntity*>& activeTargets() const;

    /**
     * 用于同步场上 TypingTarget 的竖直速度；接苹果等模式用 config 内建公式。
     * 飞机大战在 SpaceWarModel 中 override 为 currentEnemyMoveSpeed 的公式。
     */
    virtual int currentTargetMoveSpeed() const;

    /*
     * 新增统一销毁入口。
     *
     * 苹果：输入命中后直接调用这个。
     * 飞机：子弹命中敌机后再调用这个。
     */
    void confirmTargetDestroyed(TypingTargetEntity* target,
                                TypingTargetDestroyReason reason);

protected:
    /*
     * 默认实现：输入命中后直接销毁目标。
     * 苹果可以直接使用默认实现。
     * 飞机大战需要重写：输入命中只锁定敌机并发射子弹，不立刻销毁。
     */
    virtual void onTargetMatched(TypingTargetEntity* target);

    /*
     * 新语义：目标到达退场线。
     *
     * 默认实现：把退场当 miss。
     * 苹果可以直接使用默认实现。
     * 飞机大战需要重写：退场只自然回收，不算 miss，不扣血。
     */
    virtual void onTargetExited(TypingTargetEntity* target);

    virtual void onTargetMissed(TypingTargetEntity* target); // miss 只是退场事件的一种解释，不再由公共层硬编码。
    
    virtual void onTargetRemoved(TypingTargetEntity* target); // 目标被移除时调用。
    virtual void onLevelCompleted(); // 关卡完成时调用。
    virtual void onGameOver(); // 游戏结束时调用。

    virtual bool shouldCompleteLevel() const; // 判断是否完成关卡。
    virtual bool shouldGameOver() const; // 判断是否游戏结束。

    virtual int hitScoreValue(const TypingTargetEntity* target) const; // 命中目标得分。
    virtual int missPenaltyValue(const TypingTargetEntity* target) const; // 漏掉目标扣分。

    void removeTargetInternal(TypingTargetEntity* target); // 内部移除目标。
    
    void occupyLetter(QChar letter); // 占用字母。
    void releaseLetter(QChar letter); // 释放字母。

    void updateActiveTargets(int deltaMs); // 更新活跃目标。

    /*
     * 新名字：处理退场目标。
     */
    void processExitedTargets();

    /*
     * 兼容旧名字。
     */
    void processMissedTargets();

    void processDestroyedTargets();
    void finishCurrentRound();

    void syncActiveTargetSpeed();

protected:
    void notifyStateEvent(GameEventType eventType);
    void notifyScoreChanged();
    void notifyTargetSpawned(QChar letter);
    void notifyTargetRemoved(QChar letter);
    void notifyTargetHit(QChar letter);
    void notifyTargetMiss(QChar letter);
    void notifyLevelCompleted();
    void notifyGameOver();
    void notifySettingsChanged(SettingKey key, int value, int extraValue = 0);
    void notifyInputRejected(QChar letter);

protected:
    GameState m_gameState = GameState::Initial;

    TypingGameConfig m_config; // 游戏配置
    TypingGameStats m_stats; // 游戏统计

    QList<TypingTargetEntity*> m_activeTargets; // 活跃目标
    QSet<QChar> m_usedLetters; // 已使用字母
};

#endif // __TYPEGAME_TYPING_GAME_MODEL_BASE_H__
