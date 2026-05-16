#include "spacewar_test_runner.h"

#include <QtMath>

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "common/core/game_state.h"
#include "spacewar/controller/spacewar_controller.h"
#include "spacewar/model/entity/enemy_entity.h"
#include "spacewar/model/spacewar_model.h"

namespace
{

bool readJsonFile(const QString& file_path, QJsonObject* object)
{
    if (object == nullptr)
    {
        return false;
    }

    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "[SpaceWarTestRunner] open input file failed:" << file_path;
        return false;
    }

    const QByteArray data = file.readAll();
    const QJsonDocument document = QJsonDocument::fromJson(data);

    if (!document.isObject())
    {
        qDebug() << "[SpaceWarTestRunner] invalid json file:" << file_path;
        return false;
    }

    *object = document.object();
    return true;
}

bool writeJsonFile(const QString& file_path, const QJsonObject& object)
{
    const QFileInfo output_info(file_path);
    const QString parent_dir = output_info.absolutePath();
    if (!parent_dir.isEmpty() && !QDir().mkpath(parent_dir))
    {
        qDebug() << "[SpaceWarTestRunner] create output directory failed:" << parent_dir;
        return false;
    }

    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "[SpaceWarTestRunner] open output file failed:" << file_path;
        return false;
    }

    const QJsonDocument document(object);
    file.write(document.toJson(QJsonDocument::Indented));
    return true;
}

void pumpBulletsUntilIdle(SpaceWarModel* model, SpaceWarController* controller, int max_steps)
{
    if (model == nullptr || controller == nullptr)
    {
        return;
    }

    for (int i = 0; i < max_steps && model->gameState() == GameState::Playing; ++i)
    {
        if (model->activeBullets().isEmpty())
        {
            return;
        }
        controller->update(16);
    }
}

EnemyEntity* createPlacedEnemy(SpaceWarModel* model,
                               SpaceWarController* controller,
                               const QChar& letter,
                               qreal pos_x,
                               qreal pos_y,
                               qreal vertical_speed)
{
    if (model == nullptr || controller == nullptr || letter.isNull())
    {
        return nullptr;
    }

    const auto& cfg = model->config();
    const qreal width = qMax<qreal>(80.0, cfg.typingTargetLogicalWidth());
    const qreal height = qMax<qreal>(80.0, cfg.typingTargetLogicalHeight());

    EnemyEntity* enemy = new EnemyEntity();
    enemy->setLetter(letter);
    enemy->setScoreValue(10);
    enemy->setSize(QSizeF(width, height));
    enemy->setLocked(false);

    const qreal sw = static_cast<qreal>(cfg.sceneWidth());
    const qreal sh = static_cast<qreal>(cfg.sceneHeight());
    enemy->setSceneRect(QRectF(0.0, 0.0, sw, sh));
    enemy->setPosition(QPointF(pos_x, pos_y));
    enemy->configureStraight(pos_x, vertical_speed);

    const int frame_count =
        qMax(1, cfg.enemySpriteSheetCols() * cfg.enemySpriteSheetRows());
    enemy->configureSpriteAnimation(frame_count, cfg.enemySpriteAnimationFps());

    if (!model->registerTarget(enemy))
    {
        delete enemy;
        return nullptr;
    }

    controller->adoptTestEnemy(enemy);
    return enemy;
}

QJsonObject runSingleCase(const QJsonObject& case_object, const QJsonObject& config)
{
    const QString case_name = case_object.value("name").toString("unnamed_case");
    const QString letters = case_object.value("letters").toString("ABCDE");
    const int correct_rounds = case_object.value("correctRounds").toInt(0);
    const int error_rounds = case_object.value("errorRounds").toInt(0);

    qDebug().noquote() << "----------------------------------------";
    qDebug().noquote() << "Running test case:" << case_name;
    qDebug().noquote() << "Loaded letters sequence:" << letters;
    qDebug().noquote() << "Loaded correctRounds:" << correct_rounds;
    qDebug().noquote() << "Loaded errorRounds:" << error_rounds;

    const int speed_level = config.value("speedLevel").toInt(5);
    const int max_enemy_count = config.value("maxAppleCount").toInt(3);
    const int target_success_count = config.value("targetSuccessCount").toInt(5);

    SpaceWarModel model;
    model.setEnemySpeedLevel(speed_level);
    model.setMaxEnemyCount(max_enemy_count);
    model.setRewardModeEnabled(false);
    model.setDifficultyUpgradeIntervalSec(86400);

    SpaceWarController controller(&model);

    controller.initialize();
    /* currentEnemySpawnIntervalMs() 来自 ConfigureSettings，与 config().spawnIntervalMs 无关 */
    model.setSuppressAutoEnemySpawn(true);
    model.config().setTargetSuccessCount(target_success_count);

    controller.startGame();

    const qreal scene_w = static_cast<qreal>(model.config().sceneWidth());
    const qreal scene_h = static_cast<qreal>(model.config().sceneHeight());
    const qreal target_h = qMax<qreal>(80.0, model.config().typingTargetLogicalHeight());
    const qreal exit_y = scene_h - target_h;

    int executed_correct_rounds = 0;

    const int effective_correct = qMin(correct_rounds, letters.size());
    for (int i = 0; i < effective_correct && model.gameState() == GameState::Playing; ++i)
    {
        const QChar letter = letters.at(i).toUpper();
        const qreal x =
            qBound<qreal>(0.0, scene_w / 2.0 - model.config().typingTargetLogicalWidth() / 2.0,
                          qMax<qreal>(0.0, scene_w - model.config().typingTargetLogicalWidth()));

        EnemyEntity* enemy =
            createPlacedEnemy(&model, &controller, letter, x, scene_h * 0.25, 0.0);
        if (enemy == nullptr)
        {
            continue;
        }

        const int successes_before = model.stats().successCount();
        if (controller.handleLetterInput(letter))
        {
            pumpBulletsUntilIdle(&model, &controller, 10000);
        }
        if (model.stats().successCount() > successes_before)
        {
            ++executed_correct_rounds;
        }
    }

    const int hp_before_errors = model.playerHealth();

    int executed_error_rounds = 0;
    for (int i = 0; i < error_rounds && model.gameState() == GameState::Playing; ++i)
    {
        const QChar letter('Z');
        if (!model.isLetterAvailable(letter))
        {
            continue;
        }

        /* 置于退场线以下，下一帧按飞出底部结算并扣血（missCount 不增加） */
        EnemyEntity* enemy =
            createPlacedEnemy(&model, &controller, letter, scene_w * 0.1, exit_y, 0.0);
        if (enemy == nullptr)
        {
            continue;
        }

        const int hp_before_step = model.playerHealth();
        controller.update(16);
        const int hp_after_step = model.playerHealth();

        if (hp_before_step > hp_after_step)
        {
            ++executed_error_rounds;
        }
    }

    const int health_lost_to_exit = qMax(0, hp_before_errors - model.playerHealth());

    const bool success_count_passed = model.stats().successCount() >= effective_correct;
    const bool error_damage_passed =
        (error_rounds == 0) ? (health_lost_to_exit == 0)
                            : (health_lost_to_exit >= error_rounds);
    const bool target_passed = model.stats().successCount() >= target_success_count;

    QJsonObject result;
    result.insert("name", case_name);
    result.insert("passed", success_count_passed && error_damage_passed);
    result.insert("targetPassed", target_passed);
    result.insert("successCount", model.stats().successCount());
    result.insert("missCount", model.stats().missCount());
    result.insert("score", model.stats().score());
    result.insert("accuracy", model.stats().accuracy());
    result.insert("healthLostToBottomExit", health_lost_to_exit);
    result.insert("playerHealthAfter", model.playerHealth());
    result.insert("playerMaxHealth", model.playerMaxHealth());
    result.insert("expectedCorrectRounds", correct_rounds);
    result.insert("expectedErrorRounds", error_rounds);
    result.insert("executedCorrectRounds", executed_correct_rounds);
    result.insert("executedErrorRounds", executed_error_rounds);
    result.insert("gameState", static_cast<int>(model.gameState()));

    qDebug().noquote() << "Case result:";
    qDebug().noquote() << "  successCount           :" << model.stats().successCount();
    qDebug().noquote() << "  missCount              :" << model.stats().missCount();
    qDebug().noquote() << "  score                  :" << model.stats().score();
    qDebug().noquote() << "  healthLostToBottomExit :" << health_lost_to_exit;
    qDebug().noquote() << "  passed                 :" << result.value("passed").toBool();
    qDebug().noquote() << "Test case finished:" << case_name;

    return result;
}

} // namespace

bool SpaceWarTestRunner::run(const QString& input_file, const QString& output_file)
{
    qDebug().noquote() << "========================================";
    qDebug().noquote() << "        SpaceWar Game Test Runner";
    qDebug().noquote() << "========================================";
    qDebug().noquote() << "Input config file :" << input_file;
    qDebug().noquote() << "Output result file:" << output_file;

    QJsonObject input_root;
    if (!readJsonFile(input_file, &input_root))
    {
        return false;
    }

    const QString game_name = input_root.value("game").toString("spacewar");
    if (game_name != "spacewar")
    {
        qDebug() << "[SpaceWarTestRunner] unsupported game:" << game_name;
        return false;
    }

    const QJsonObject config = input_root.value("config").toObject();
    const QJsonArray test_cases = input_root.value("testCases").toArray();

    if (test_cases.isEmpty())
    {
        qDebug() << "[SpaceWarTestRunner] testCases is empty.";
        return false;
    }

    qDebug().noquote() << "Loaded test cases:" << test_cases.size();

    QJsonArray result_cases;
    int passed_cases = 0;

    for (const QJsonValue& value : test_cases)
    {
        if (!value.isObject())
        {
            continue;
        }

        const QJsonObject case_object = value.toObject();
        const QJsonObject case_result = runSingleCase(case_object, config);

        if (case_result.value("passed").toBool())
        {
            ++passed_cases;
        }

        result_cases.append(case_result);
    }

    QJsonObject output_root;
    output_root.insert("game", "spacewar");
    output_root.insert("passed", passed_cases == result_cases.size());
    output_root.insert("totalCases", result_cases.size());
    output_root.insert("passedCases", passed_cases);
    output_root.insert("failedCases", result_cases.size() - passed_cases);
    output_root.insert("timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
    output_root.insert("cases", result_cases);

    const bool ok = writeJsonFile(output_file, output_root);

    qDebug().noquote() << "========================================";
    qDebug().noquote() << "        SpaceWar Test Summary";
    qDebug().noquote() << "========================================";
    qDebug().noquote() << "Total cases :" << result_cases.size();
    qDebug().noquote() << "Passed cases:" << passed_cases;
    qDebug().noquote() << "Failed cases:" << result_cases.size() - passed_cases;
    qDebug().noquote() << "Result file :" << output_file;
    qDebug().noquote() << "========================================";

    return ok;
}
