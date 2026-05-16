#ifndef __TYPEGAME_SAVEAPPLE_MODEL_H__
#define __TYPEGAME_SAVEAPPLE_MODEL_H__

#include <QChar>
#include <QList>

#include "common/typing/typing_game_model_base.h"

/* ------------------------------------------------------------------
// 文件名     : saveapple_model.h
// 创建者     : 3263297739@qq.com
// 创建时间   : 2026-04-14
// 功能描述   : 拯救苹果游戏核心业务模型
------------------------------------------------------------------ */

class AppleModel;

/**
 * @brief 拯救苹果游戏核心业务模型。
 *
 * 继承 TypingGameModelBase，复用公共 typing 游戏逻辑：
 *   - 游戏状态、分数、统计由基类 TypingGameStats / TypingGameConfig 统一管理
 *   - 目标注册 / 移除 / 命中 / 漏失由基类实现
 *   - activeTargets() 存储 TypingTargetEntity*（实际类型为 AppleModel*）
 *
 * 对外保留原有公共 API，内部转调基类能力。
 *
 * 苹果语义（使用公共默认实现，无需重写）：
 *   - 输入命中 → 直接消除 → success + score（onTargetMatched 默认逻辑）
 *   - 到达 70% 退场线 → miss（onTargetExited 默认逻辑）
 *   - success 达标 → 胜利（shouldCompleteLevel 默认逻辑）
 *   - miss 超上限 → 失败（shouldGameOver 默认逻辑）
 */
class SaveAppleModel final : public TypingGameModelBase
{
public:
    SaveAppleModel();
    ~SaveAppleModel() override = default;

    SaveAppleModel(const SaveAppleModel&) = delete;
    SaveAppleModel& operator=(const SaveAppleModel&) = delete;

    void initialize() override;
    void resetModel() override;

    /* ---- 苹果目标管理（兼容旧 API，内部转调 registerTarget） ---- */
    bool registerApple(AppleModel* apple);
    bool unregisterApple(AppleModel* apple);

    bool hitAppleByLetter(QChar input_letter);
    AppleModel* findAppleByLetter(QChar letter) const;

    bool canSpawnApple() const;
    bool isLetterAvailable(QChar letter) const;
    void clearActiveApples();

    /* ---- 配置 setter（带旧验证规则） ---- */
    void setSpeedLevel(int speed_level);
    void setMaxAppleCount(int max_apple_count);
    void setTargetSuccessCount(int target_success_count);
    void setMaxMissCount(int max_miss_count);
    void setSoundEnabled(bool sound_enabled);
    void setFailLineY(int fail_line_y);

    /* ---- 配置 getter ---- */
    int speedLevel() const;
    int maxAppleCount() const;
    int targetSuccessCount() const;
    int maxMissCount() const;
    bool soundEnabled() const;
    int failLineY() const;

    /* ---- 运行时统计 ---- */
    int score() const;
    int successCount() const;
    int missCount() const;
    int accuracy() const;
    bool isLevelCompleted() const;

    /* ---- 活动苹果列表（由 activeTargets() 动态转换） ---- */
    QList<AppleModel*> activeApples() const;

    /* ---- 当前苹果下落速度（与旧接口公式相同） ---- */
    int currentAppleFallSpeed() const;

protected:
    /* ---- 覆盖得分公式，使用苹果专属速度奖励计算 ---- */
    int hitScoreValue(const TypingTargetEntity* target) const override;

private:
    /* ---- 配置合法性校验（保留旧约束边界） ---- */
    bool isValidSpeedLevel(int speed_level) const;
    bool isValidMaxAppleCount(int max_apple_count) const;
    bool isValidTargetSuccessCount(int target_success_count) const;
    bool isValidMaxMissCount(int max_miss_count) const;
};

#endif // __TYPEGAME_SAVEAPPLE_MODEL_H__
