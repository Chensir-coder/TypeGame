/* ------------------------------------------------------------------
// 文件名     : saveapple_model.cpp
// 创建者     : 3263297739@qq.com
// 创建时间   : 2026-04-14
// 功能描述   : 拯救苹果游戏核心业务模型实现
------------------------------------------------------------------ */

#include "saveapple_model.h"

#include "saveapple/model/apple_model.h"

SaveAppleModel::SaveAppleModel()
{
    initialize();
}

void SaveAppleModel::initialize()
{
    // 基类 initialize 会 resetToDefault() 配置 + 重置 stats + 清空目标
    TypingGameModelBase::initialize();

    // 苹果游戏专属默认配置（与旧接口保持一致）
    config().setSpeedLevel(5);
    config().setMaxTargetCount(3);
    config().setTargetSuccessCount(5);
    config().setMaxMissCount(3);
    config().setSoundEnabled(true);
    config().setExitLineRatio(0.7);  // failLineY = 700（sceneHeight 默认 1000）
}

void SaveAppleModel::resetModel()
{
    // 基类 resetModel：重置 stats、清空活动目标，但保留 config 设置
    TypingGameModelBase::resetModel();
}

bool SaveAppleModel::registerApple(AppleModel* apple)
{
    return registerTarget(apple);
}

bool SaveAppleModel::unregisterApple(AppleModel* apple)
{
    return unregisterTarget(apple);
}

bool SaveAppleModel::hitAppleByLetter(QChar input_letter)
{
    return handleInput(input_letter);
}

AppleModel* SaveAppleModel::findAppleByLetter(QChar letter) const
{
    TypingTargetEntity* target = findTargetByLetter(letter);
    return static_cast<AppleModel*>(target);
}

bool SaveAppleModel::canSpawnApple() const
{
    return canSpawnTarget();
}

bool SaveAppleModel::isLetterAvailable(QChar letter) const
{
    return TypingGameModelBase::isLetterAvailable(letter);
}

void SaveAppleModel::clearActiveApples()
{
    clearActiveTargets();
}

void SaveAppleModel::setSpeedLevel(int speed_level)
{
    if (!isValidSpeedLevel(speed_level))
    {
        return;
    }

    if (config().speedLevel() == speed_level)
    {
        return;
    }

    config().setSpeedLevel(speed_level);
    syncActiveTargetSpeed();
    notifySettingsChanged(SettingKey::SpeedLevel, speed_level);
}

void SaveAppleModel::setMaxAppleCount(int max_apple_count)
{
    if (!isValidMaxAppleCount(max_apple_count))
    {
        return;
    }

    if (config().maxTargetCount() == max_apple_count)
    {
        return;
    }

    config().setMaxTargetCount(max_apple_count);
    notifySettingsChanged(SettingKey::MaxAppleCount, max_apple_count);
}

void SaveAppleModel::setTargetSuccessCount(int target_success_count)
{
    if (!isValidTargetSuccessCount(target_success_count))
    {
        return;
    }

    if (config().targetSuccessCount() == target_success_count)
    {
        return;
    }

    config().setTargetSuccessCount(target_success_count);
    notifySettingsChanged(SettingKey::TargetSuccessCount, target_success_count);
}

void SaveAppleModel::setMaxMissCount(int max_miss_count)
{
    if (!isValidMaxMissCount(max_miss_count))
    {
        return;
    }

    if (config().maxMissCount() == max_miss_count)
    {
        return;
    }

    config().setMaxMissCount(max_miss_count);
    notifySettingsChanged(SettingKey::MaxMissCount, max_miss_count);
}

void SaveAppleModel::setSoundEnabled(bool sound_enabled)
{
    config().setSoundEnabled(sound_enabled);
    notifySettingsChanged(SettingKey::SoundEnabled, sound_enabled ? 1 : 0);
}

void SaveAppleModel::setFailLineY(int fail_line_y)
{
    if (fail_line_y <= 0)
    {
        return;
    }

    const int scene_height = config().sceneHeight();
    if (scene_height <= 0)
    {
        return;
    }

    const qreal new_ratio = static_cast<qreal>(fail_line_y) / static_cast<qreal>(scene_height);
    if (qFuzzyCompare(config().exitLineRatio(), new_ratio))
    {
        return;
    }

    config().setExitLineRatio(new_ratio);
    notifySettingsChanged(SettingKey::FailLineY, fail_line_y);
}

int SaveAppleModel::speedLevel() const
{
    return config().speedLevel();
}

int SaveAppleModel::maxAppleCount() const
{
    return config().maxTargetCount();
}

int SaveAppleModel::targetSuccessCount() const
{
    return config().targetSuccessCount();
}

int SaveAppleModel::maxMissCount() const
{
    return config().maxMissCount();
}

bool SaveAppleModel::soundEnabled() const
{
    return config().soundEnabled();
}

int SaveAppleModel::failLineY() const
{
    return static_cast<int>(config().exitLineRatio() * static_cast<qreal>(config().sceneHeight()));
}

int SaveAppleModel::score() const
{
    return stats().score();
}

int SaveAppleModel::successCount() const
{
    return stats().successCount();
}

int SaveAppleModel::missCount() const
{
    return stats().missCount();
}

int SaveAppleModel::accuracy() const
{
    return stats().accuracy();
}

bool SaveAppleModel::isLevelCompleted() const
{
    return stats().isTargetCompleted(config().targetSuccessCount());
}

QList<AppleModel*> SaveAppleModel::activeApples() const
{
    QList<AppleModel*> result;
    result.reserve(activeTargets().size());

    for (TypingTargetEntity* target : activeTargets())
    {
        result.append(static_cast<AppleModel*>(target));
    }

    return result;
}

int SaveAppleModel::currentAppleFallSpeed() const
{
    return currentTargetMoveSpeed();
}

int SaveAppleModel::hitScoreValue(const TypingTargetEntity* target) const
{
    Q_UNUSED(target);
    return 10 + (config().speedLevel() - 1) * 2;
}

bool SaveAppleModel::isValidSpeedLevel(int speed_level) const
{
    return speed_level >= 1 && speed_level <= 10;
}

bool SaveAppleModel::isValidMaxAppleCount(int max_apple_count) const
{
    return max_apple_count >= 1 && max_apple_count <= 5;
}

bool SaveAppleModel::isValidTargetSuccessCount(int target_success_count) const
{
    return target_success_count > 0;
}

bool SaveAppleModel::isValidMaxMissCount(int max_miss_count) const
{
    return max_miss_count > 0;
}
