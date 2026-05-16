#include "configure_settings.h"

#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

namespace
{
QString readAllUtf8(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    return QString::fromUtf8(f.readAll());
}

int jsonInt(const QJsonObject& o, const char* key, int fallback)
{
    if (!o.contains(QLatin1String(key))) {
        return fallback;
    }
    const QJsonValue v = o.value(QLatin1String(key));
    return v.isDouble() ? static_cast<int>(v.toDouble()) : v.toInt(fallback);
}

qreal jsonReal(const QJsonObject& o, const char* key, qreal fallback)
{
    if (!o.contains(QLatin1String(key))) {
        return fallback;
    }
    const QJsonValue v = o.value(QLatin1String(key));
    return v.isString() ? v.toString().toDouble() : v.toDouble(fallback);
}

QString jsonString(const QJsonObject& o, const char* key, const QString& fallback)
{
    if (!o.contains(QLatin1String(key))) {
        return fallback;
    }
    const QJsonValue v = o.value(QLatin1String(key));
    return v.toString(fallback);
}

void parseSpacewarObject(SpacewarConfigureSettings* out, const QJsonObject& obj)
{
    if (out == nullptr) {
        return;
    }

    if (obj.contains(QStringLiteral("scene"))) {
        const QJsonObject s = obj.value(QStringLiteral("scene")).toObject();
        out->scene.width = jsonInt(s, "width", out->scene.width);
        out->scene.height = jsonInt(s, "height", out->scene.height);
    }

    if (obj.contains(QStringLiteral("enemyMove"))) {
        const QJsonObject s = obj.value(QStringLiteral("enemyMove")).toObject();
        out->enemyMove.baseSpeed = jsonInt(s, "baseSpeed", out->enemyMove.baseSpeed);
        out->enemyMove.speedPerLevel =
            jsonInt(s, "speedPerLevel", out->enemyMove.speedPerLevel);
        out->enemyMove.difficultySpeedBonus =
            jsonInt(s, "difficultySpeedBonus", out->enemyMove.difficultySpeedBonus);
        out->enemyMove.minVerticalSpeed =
            jsonInt(s, "minVerticalSpeed", out->enemyMove.minVerticalSpeed);
    }

    if (obj.contains(QStringLiteral("spawn"))) {
        const QJsonObject s = obj.value(QStringLiteral("spawn")).toObject();
        out->spawn.baseIntervalMs =
            jsonInt(s, "baseIntervalMs", out->spawn.baseIntervalMs);
        out->spawn.minIntervalMs =
            jsonInt(s, "minIntervalMs", out->spawn.minIntervalMs);
        out->spawn.stepMs = jsonInt(s, "stepMs", out->spawn.stepMs);
    }

    if (obj.contains(QStringLiteral("typingTarget"))) {
        const QJsonObject s = obj.value(QStringLiteral("typingTarget")).toObject();
        out->typingTarget.width =
            jsonReal(s, "width", out->typingTarget.width);
        out->typingTarget.height =
            jsonReal(s, "height", out->typingTarget.height);
    }

    if (obj.contains(QStringLiteral("playerCraft"))) {
        const QJsonObject s = obj.value(QStringLiteral("playerCraft")).toObject();
        out->playerCraft.width =
            jsonReal(s, "width", out->playerCraft.width);
        out->playerCraft.height =
            jsonReal(s, "height", out->playerCraft.height);
        out->playerCraft.bottomMargin =
            jsonReal(s, "bottomMargin", out->playerCraft.bottomMargin);
        out->playerCraft.maxHealth =
            jsonInt(s, "maxHealth", out->playerCraft.maxHealth);
        out->playerCraft.moveSpeed =
            jsonReal(s, "moveSpeed", out->playerCraft.moveSpeed);
    }

    if (obj.contains(QStringLiteral("spriteSheets"))) {
        const QJsonObject s = obj.value(QStringLiteral("spriteSheets")).toObject();
        out->spriteSheets.enemyCols =
            jsonInt(s, "enemyCols", out->spriteSheets.enemyCols);
        out->spriteSheets.enemyRows =
            jsonInt(s, "enemyRows", out->spriteSheets.enemyRows);
        out->spriteSheets.playerCols =
            jsonInt(s, "playerCols", out->spriteSheets.playerCols);
        out->spriteSheets.playerRows =
            jsonInt(s, "playerRows", out->spriteSheets.playerRows);
        out->spriteSheets.enemyAnimationFps =
            jsonReal(s, "enemyAnimationFps", out->spriteSheets.enemyAnimationFps);
        out->spriteSheets.playerAnimationFps =
            jsonReal(s, "playerAnimationFps", out->spriteSheets.playerAnimationFps);
        out->spriteSheets.rewardCols =
            jsonInt(s, "rewardCols", out->spriteSheets.rewardCols);
        out->spriteSheets.rewardRows =
            jsonInt(s, "rewardRows", out->spriteSheets.rewardRows);
    }

    if (obj.contains(QStringLiteral("bullet"))) {
        const QJsonObject s = obj.value(QStringLiteral("bullet")).toObject();
        out->bullet.width = jsonReal(s, "width", out->bullet.width);
        out->bullet.height = jsonReal(s, "height", out->bullet.height);
        out->bullet.speed = jsonReal(s, "speed", out->bullet.speed);
        out->bullet.hitRadiusMin =
            jsonReal(s, "hitRadiusMin", out->bullet.hitRadiusMin);
    }

    if (obj.contains(QStringLiteral("reward"))) {
        const QJsonObject s = obj.value(QStringLiteral("reward")).toObject();
        out->reward.width = jsonReal(s, "width", out->reward.width);
        out->reward.height = jsonReal(s, "height", out->reward.height);
        out->reward.horizontalSpeed =
            jsonReal(s, "horizontalSpeed", out->reward.horizontalSpeed);
        out->reward.healValue = jsonInt(s, "healValue", out->reward.healValue);
        out->reward.scoreValue =
            jsonInt(s, "scoreValue", out->reward.scoreValue);
        out->reward.spawnIntervalMs =
            jsonInt(s, "spawnIntervalMs", out->reward.spawnIntervalMs);
    }

    if (obj.contains(QStringLiteral("enemySpawn"))) {
        const QJsonObject s = obj.value(QStringLiteral("enemySpawn")).toObject();
        out->enemySpawn.horizontalPadding =
            jsonReal(s, "horizontalPadding", out->enemySpawn.horizontalPadding);
        out->enemySpawn.yOffset =
            jsonReal(s, "yOffset", out->enemySpawn.yOffset);
    }

    if (obj.contains(QStringLiteral("trajectory"))) {
        const QJsonObject s = obj.value(QStringLiteral("trajectory")).toObject();
        out->trajectory.sineAmplitudeMin =
            jsonReal(s, "sineAmplitudeMin", out->trajectory.sineAmplitudeMin);
        out->trajectory.sineAmplitudeMax =
            jsonReal(s, "sineAmplitudeMax", out->trajectory.sineAmplitudeMax);
        out->trajectory.sineAngularSpeed =
            jsonReal(s, "sineAngularSpeed", out->trajectory.sineAngularSpeed);
        out->trajectory.bounceHorizontalBase =
            jsonReal(s, "bounceHorizontalBase", out->trajectory.bounceHorizontalBase);
        out->trajectory.bounceHorizontalRandom =
            jsonReal(s, "bounceHorizontalRandom", out->trajectory.bounceHorizontalRandom);
    }

    if (obj.contains(QStringLiteral("rewardWordAi"))) {
        const QJsonObject s = obj.value(QStringLiteral("rewardWordAi")).toObject();
        out->rewardWordAi.minLength =
            jsonInt(s, "minLength", out->rewardWordAi.minLength);
        out->rewardWordAi.maxLength =
            jsonInt(s, "maxLength", out->rewardWordAi.maxLength);
        out->rewardWordAi.topic =
            jsonString(s, "topic", out->rewardWordAi.topic);
        out->rewardWordAi.arkChatUrl =
            jsonString(s, "arkChatUrl", out->rewardWordAi.arkChatUrl);
        out->rewardWordAi.arkModelEndpoint =
            jsonString(s, "arkModelEndpoint", out->rewardWordAi.arkModelEndpoint);
    }

    if (obj.contains(QStringLiteral("ui"))) {
        const QJsonObject s = obj.value(QStringLiteral("ui")).toObject();
        out->ui.barMinHeight = jsonInt(s, "barMinHeight", out->ui.barMinHeight);
        out->ui.barHMargin = jsonInt(s, "barHMargin", out->ui.barHMargin);
        out->ui.barVMargin = jsonInt(s, "barVMargin", out->ui.barVMargin);
        out->ui.playfieldMinHeight =
            jsonInt(s, "playfieldMinHeight", out->ui.playfieldMinHeight);
    }
}
}

ConfigureSettings& ConfigureSettings::instance()
{
    static ConfigureSettings inst;
    return inst;
}

ConfigureSettings::ConfigureSettings()
{
    applySpacewarDefaults();
}

void ConfigureSettings::applySpacewarDefaults()
{
    m_spacewar = SpacewarConfigureSettings();
}

bool ConfigureSettings::load(const QString& path)
{
    applySpacewarDefaults();

    QString jsonText;

    if (!path.isEmpty()) {
        jsonText = readAllUtf8(path);
    }

    if (jsonText.isEmpty() && qApp != nullptr) {
        const QString appDirPath =
            QCoreApplication::applicationDirPath() + QStringLiteral("/configureSettings.json");
        jsonText = readAllUtf8(appDirPath);
    }

    if (jsonText.isEmpty()) {
        QFile rf(QStringLiteral(":/config/configureSettings.json"));
        if (rf.open(QIODevice::ReadOnly | QIODevice::Text)) {
            jsonText = QString::fromUtf8(rf.readAll());
        }
    }

    if (jsonText.isEmpty()) {
        m_loadedFromFile = false;
        return false;
    }

    QJsonParseError err{};
    const QJsonDocument doc = QJsonDocument::fromJson(jsonText.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) {
        m_loadedFromFile = false;
        return false;
    }

    const QJsonObject root = doc.object();
    if (root.contains(QStringLiteral("spacewar"))) {
        parseSpacewarObject(&m_spacewar, root.value(QStringLiteral("spacewar")).toObject());
    }

    m_loadedFromFile = true;
    return true;
}
