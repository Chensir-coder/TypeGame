#ifndef __TYPEGAME_CONFIGURE_SETTINGS_H__
#define __TYPEGAME_CONFIGURE_SETTINGS_H__

#include <QString>

#include <QtGlobal>

/* ------------------------------------------------------------------
 // 文件名     : configure_settings.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : configure_settings 头文件声明
------------------------------------------------------------------ */

/**
 * 从 configureSettings.json 加载的全局可调参数（当前主要为太空战）。
 * 在创建 SpaceWar 模型之前应调用 ConfigureSettings::instance().load()。
 */
struct SpacewarSceneSettings
{
    int width = 1920;
    int height = 1080;
};

struct SpacewarEnemyMoveSettings
{
    int baseSpeed = 120;
    int speedPerLevel = 27;
    int difficultySpeedBonus = 18;
    int minVerticalSpeed = 60;
};

struct SpacewarSpawnSettings
{
    int baseIntervalMs = 1000;
    int minIntervalMs = 100;
    int stepMs = 80;
};

struct SpacewarTypingTargetSettings
{
    qreal width = 100.0;
    qreal height = 84.0;
};

struct SpacewarPlayerCraftSettings
{
    qreal width = 120.0;
    qreal height = 156.0;
    qreal bottomMargin = 56.0;
    int maxHealth = 18;
    qreal moveSpeed = 260.0;
};

struct SpacewarSpriteSheetSettings
{
    int enemyCols = 3;
    int enemyRows = 4;
    int playerCols = 3;
    int playerRows = 4;
    qreal enemyAnimationFps = 10.0;
    qreal playerAnimationFps = 6.0;
    int rewardCols = 3;
    int rewardRows = 3;
};

struct SpacewarBulletSettings
{
    qreal width = 30.0;
    qreal height = 30.0;
    qreal speed = 720.0;
    qreal hitRadiusMin = 16.0;
};

struct SpacewarRewardSpawnSettings
{
    qreal width = 165.0;
    qreal height = 125.0;
    qreal horizontalSpeed = 170.0;
    int healValue = 1;
    int scoreValue = 0;
    int spawnIntervalMs = 10000;
};

struct SpacewarEnemySpawnSettings
{
    qreal horizontalPadding = 48.0;
    qreal yOffset = -100.0;
};

struct SpacewarTrajectorySettings
{
    qreal sineAmplitudeMin = 32.0;
    qreal sineAmplitudeMax = 165.0;
    qreal sineAngularSpeed = 1.6;
    qreal bounceHorizontalBase = 105.0;
    qreal bounceHorizontalRandom = 60.0;
};

struct SpacewarRewardWordAiSettings
{
    int minLength = 4;
    int maxLength = 8;
    QString topic = QStringLiteral("space war");
    /** Volcengine Ark Chat Completions URL; empty = AiWordService builtin default. */
    QString arkChatUrl;
    /** Inference endpoint id (ep-...); sent as JSON field "model"; empty = AiWordService builtin. */
    QString arkModelEndpoint;
};

struct SpacewarUiSettings
{
    int barMinHeight = 118;
    int barHMargin = 14;
    int barVMargin = 10;
    int playfieldMinHeight = 720;
};

struct SpacewarConfigureSettings
{
    SpacewarSceneSettings scene;
    SpacewarEnemyMoveSettings enemyMove;
    SpacewarSpawnSettings spawn;
    SpacewarTypingTargetSettings typingTarget;
    SpacewarPlayerCraftSettings playerCraft;
    SpacewarSpriteSheetSettings spriteSheets;
    SpacewarBulletSettings bullet;
    SpacewarRewardSpawnSettings reward;
    SpacewarEnemySpawnSettings enemySpawn;
    SpacewarTrajectorySettings trajectory;
    SpacewarRewardWordAiSettings rewardWordAi;
    SpacewarUiSettings ui;
};

class ConfigureSettings
{
public:
    static ConfigureSettings& instance();

    /**
     * 尝试顺序：path（若非空）→ 可执行目录下 configureSettings.json → Qt 资源 :/config/configureSettings.json。
     * @return 是否成功解析出至少默认 spacewar 段（失败时仍保留内置默认值）。
     */
    bool load(const QString& path = QString());

    const SpacewarConfigureSettings& spacewar() const { return m_spacewar; }

    /** 最后一次 load 是否从文件成功读取 JSON（资源文件也算成功）。 */
    bool loadedFromFile() const { return m_loadedFromFile; }

private:
    ConfigureSettings();
    void applySpacewarDefaults();

    SpacewarConfigureSettings m_spacewar;
    bool m_loadedFromFile = false;
};

#endif // __TYPEGAME_CONFIGURE_SETTINGS_H__
