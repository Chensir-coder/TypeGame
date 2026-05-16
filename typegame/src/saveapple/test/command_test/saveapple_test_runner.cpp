#include "saveapple_test_runner.h"

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

/* ------------------------------------------------------------------
 // 文件名     : saveapple_test_runner.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 拯救苹果命令行自动化测试运行器
------------------------------------------------------------------ */

#include "saveapple/controller/saveapple_controller.h"
#include "saveapple/factory/apple_factory.h"
#include "saveapple/model/apple_model.h"
#include "saveapple/model/saveapple_model.h"

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
        qDebug() << "[SaveAppleTestRunner] open input file failed:" << file_path;
        return false;
    }

    const QByteArray data = file.readAll();
    const QJsonDocument document = QJsonDocument::fromJson(data);

    if (!document.isObject())
    {
        qDebug() << "[SaveAppleTestRunner] invalid json file:" << file_path;
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
        qDebug() << "[SaveAppleTestRunner] create output directory failed:" << parent_dir;
        return false;
    }

    QFile file(file_path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qDebug() << "[SaveAppleTestRunner] open output file failed:" << file_path;
        return false;
    }

    const QJsonDocument document(object);
    file.write(document.toJson(QJsonDocument::Indented));
    return true;
}

AppleModel* createTestApple(
    AppleFactory* factory,
    SaveAppleModel* model,
    const QChar& letter,
    const int x,
    const int y)
{
    if (factory == nullptr || model == nullptr)
    {
        return nullptr;
    }

    AppleModel* apple = factory->create();
    if (apple == nullptr)
    {
        return nullptr;
    }

    apple->setLetter(letter);
    apple->setPosition(x, y);
    apple->setFallSpeed(model->currentAppleFallSpeed());

    if (!model->registerApple(apple))
    {
        factory->recycle(apple);
        return nullptr;
    }

    return apple;
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
    qDebug().noquote() << "Preparing SaveApple model/controller/factory...";


    const int speed_level = config.value("speedLevel").toInt(5);
    const int max_apple_count = config.value("maxAppleCount").toInt(3);
    const int target_success_count = config.value("targetSuccessCount").toInt(5);
    const int max_miss_count = config.value("maxMissCount").toInt(3);
    const int fail_line_y = config.value("failLineY").toInt(700);
    qDebug().noquote() << "Applied speed level:" << speed_level;
    qDebug().noquote() << "Applied max apple count:" << max_apple_count;
    qDebug().noquote() << "Applied target success count:" << target_success_count;
    qDebug().noquote() << "Applied fail line Y:" << fail_line_y;
    qDebug().noquote() << "Game started successfully";

    SaveAppleModel model;
    AppleFactory factory(32);
    SaveAppleController controller(&model, &factory);

    model.setSpeedLevel(speed_level);
    model.setMaxAppleCount(max_apple_count);
    model.setTargetSuccessCount(target_success_count);
    model.setMaxMissCount(max_miss_count);
    model.setFailLineY(fail_line_y);

    controller.initialize();
    controller.startGame();

    int executed_correct_rounds = 0;
    int executed_error_rounds = 0;

    for (int i = 0; i < correct_rounds && i < letters.size(); ++i)
    {
        const QChar letter = letters.at(i).toUpper();

        AppleModel* apple = createTestApple(&factory, &model, letter, 100, 0);
        if (apple == nullptr)
        {
            continue;
        }

        if (controller.handleLetterInput(letter))
        {
            ++executed_correct_rounds;
        }
    }

    for (int i = 0; i < error_rounds; ++i)
    {
        AppleModel* apple = createTestApple(&factory, &model, QChar('Z'), 100, fail_line_y);
        if (apple == nullptr)
        {
            continue;
        }

        model.update(1);

        if (!apple->isActive())
        {
            factory.recycle(apple);
            ++executed_error_rounds;
        }
    }

    const bool success_count_passed = model.successCount() >= qMin(correct_rounds, letters.size());
    const bool miss_count_passed = model.missCount() >= error_rounds;
    const bool target_passed = model.successCount() >= target_success_count;

    QJsonObject result;
    result.insert("name", case_name);
    result.insert("passed", success_count_passed && miss_count_passed);
    result.insert("targetPassed", target_passed);
    result.insert("successCount", model.successCount());
    result.insert("missCount", model.missCount());
    result.insert("score", model.score());
    result.insert("accuracy", model.accuracy());
    result.insert("expectedCorrectRounds", correct_rounds);
    result.insert("expectedErrorRounds", error_rounds);
    result.insert("executedCorrectRounds", executed_correct_rounds);
    result.insert("executedErrorRounds", executed_error_rounds);
    result.insert("gameState", static_cast<int>(model.gameState()));

    qDebug().noquote() << "Case result:";
    qDebug().noquote() << "  successCount:" << model.successCount();
    qDebug().noquote() << "  missCount   :" << model.missCount();
    qDebug().noquote() << "  score       :" << model.score();
    qDebug().noquote() << "  accuracy    :" << model.accuracy();
    qDebug().noquote() << "  passed      :" << result.value("passed").toBool();
    qDebug().noquote() << "Test case finished:" << case_name;

    return result;
}

} // namespace

bool SaveAppleTestRunner::run(const QString& input_file, const QString& output_file)
{
    qDebug().noquote() << "========================================";
    qDebug().noquote() << "        Apple Game Test Runner";
    qDebug().noquote() << "========================================";
    qDebug().noquote() << "Launching Apple game in test mode...";
    qDebug().noquote() << "Input config file :" << input_file;
    qDebug().noquote() << "Output result file:" << output_file;
    qDebug().noquote() << "";


    QJsonObject input_root;
    if (!readJsonFile(input_file, &input_root))
    {
        return false;
    }


    const QString game_name = input_root.value("game").toString("apple");
    if (game_name != "apple")
    {
        qDebug() << "[SaveAppleTestRunner] unsupported game:" << game_name;
        return false;
    }

    const QJsonObject config = input_root.value("config").toObject();
    const QJsonArray test_cases = input_root.value("testCases").toArray();

    if (test_cases.isEmpty())
    {
        qDebug() << "[SaveAppleTestRunner] testCases is empty.";
        return false;
    }

    qDebug().noquote() << "Loading apple test configuration...";
    qDebug().noquote() << "Loaded game:" << game_name;
    qDebug().noquote() << "Loaded speedLevel:" << config.value("speedLevel").toInt(5);
    qDebug().noquote() << "Loaded maxAppleCount:" << config.value("maxAppleCount").toInt(3);
    qDebug().noquote() << "Loaded targetSuccessCount:" << config.value("targetSuccessCount").toInt(5);
    qDebug().noquote() << "Loaded maxMissCount:" << config.value("maxMissCount").toInt(3);
    qDebug().noquote() << "Loaded failLineY:" << config.value("failLineY").toInt(700);
    qDebug().noquote() << "Loaded test cases:" << test_cases.size();
    qDebug().noquote() << "Settings applied successfully";
    qDebug().noquote() << "";


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
    output_root.insert("game", "apple");
    output_root.insert("passed", passed_cases == result_cases.size());
    output_root.insert("totalCases", result_cases.size());
    output_root.insert("passedCases", passed_cases);
    output_root.insert("failedCases", result_cases.size() - passed_cases);
    output_root.insert("timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
    output_root.insert("cases", result_cases);

    return writeJsonFile(output_file, output_root);

    qDebug().noquote() << "";
    qDebug().noquote() << "========================================";
    qDebug().noquote() << "        Apple Test Summary";
    qDebug().noquote() << "========================================";
    qDebug().noquote() << "Total cases :" << result_cases.size();
    qDebug().noquote() << "Passed cases:" << passed_cases;
    qDebug().noquote() << "Failed cases:" << result_cases.size() - passed_cases;
    qDebug().noquote() << "Result file :" << output_file;
    qDebug().noquote() << "========================================";
}
