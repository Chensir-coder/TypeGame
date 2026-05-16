/* ------------------------------------------------------------------
 // 文件名     : main.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 程序入口
------------------------------------------------------------------ */

#include <QApplication>
#include <QCoreApplication>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <locale>

#include <gtest/gtest.h>

#include "app/application/game_application.h"
#include "common/test/command_test/command_line_options.h"
#include "saveapple/test/command_test/saveapple_test_runner.h"
#include "spacewar/test/command/spacewar_test_runner.h"

namespace
{

bool isTestMode(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i)
    {
        if (QString::fromLocal8Bit(argv[i]) == "--test")
        {
            return true;
        }
    }

    return false;
}

bool isUnittestMode(int argc, char* argv[])
{
    for (int i = 0; i < argc; ++i)
    {
        if (QString::fromLocal8Bit(argv[i]) == "--unittest")
        {
            return true;
        }
    }

    return false;
}

} // namespace

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "zh_CN.UTF-8");

    if (isUnittestMode(argc, argv))
    {
        QCoreApplication app(argc, argv);

        qDebug() << "[main] Running in unit test mode (GoogleTest).";

        ::testing::InitGoogleTest(&argc, argv);

        return RUN_ALL_TESTS();
    }

    if (isTestMode(argc, argv))
    {
        QCoreApplication app(argc, argv);

        const QStringList args = QCoreApplication::arguments();
        qDebug() << "[main] Running in test mode:" << args;

        CommandLineOptions options;
        if (!CommandLineOptions::parse(args, &options))
        {
            qDebug() << "[main] parse command line failed.";
            return 1;
        }

        if (options.game_name == "apple")
        {
            SaveAppleTestRunner runner;
            return runner.run(options.input_file, options.output_file) ? 0 : 2;
        }

        if (options.game_name == "space")
        {
            SpaceWarTestRunner runner;
            return runner.run(options.input_file, options.output_file) ? 0 : 2;
        }

        qDebug() << "[main] unsupported game:" << options.game_name;
        return 3;
    }

    QApplication application(argc, argv);

    GameApplication game_application;
    game_application.initialize();

    const int init_result = game_application.run();
    if (init_result != 0)
    {
        return init_result;
    }

    return application.exec();
}



// #include <QDebug>
// #include <QChar>
// #include <QPointF>
// #include <QtGlobal>

// #include "spacewar/model/spacewar_model.h"
// #include "spacewar/controller/spacewar_controller.h"
// #include "common/core/game_state.h"
// #include "common/typing/typing_target_entity.h"

// #define CHECK_TRUE(expr)                                                   \
//     do {                                                                   \
//         if (!(expr)) {                                                     \
//             qFatal("CHECK_TRUE failed: %s, line %d", #expr, __LINE__);     \
//         }                                                                  \
//     } while (false)

// #define CHECK_EQ(actual, expected)                                         \
//     do {                                                                   \
//         if (!((actual) == (expected))) {                                    \
//             qFatal("CHECK_EQ failed: %s != %s, line %d",                   \
//                    #actual, #expected, __LINE__);                          \
//         }                                                                  \
//     } while (false)

// static TypingTargetEntity* firstTarget(SpaceWarModel& model)
// {
//     CHECK_TRUE(!model.activeTargets().isEmpty());
//     CHECK_TRUE(model.activeTargets().first() != nullptr);
//     return model.activeTargets().first();
// }

// void testSpaceWarControllerDrivesTypingModel()
// {
//     qDebug() << "========== testSpaceWarControllerDrivesTypingModel begin ==========";

//     SpaceWarModel model;
//     SpaceWarController controller(&model);

//     // 1. 初始化前：设置真实场景大小
//     controller.setSceneSize(800, 1000);
//     controller.initialize();

//     CHECK_EQ(model.gameState(), GameState::Initial);
//     CHECK_EQ(model.config().sceneWidth(), 800);
//     CHECK_EQ(model.config().sceneHeight(), 1000);
//     CHECK_EQ(model.activeTargets().size(), 0);
//     CHECK_EQ(model.stats().score(), 0);
//     CHECK_EQ(model.stats().successCount(), 0);
//     CHECK_EQ(model.stats().missCount(), 0);

//     qDebug() << "[OK] initialize";

//     // 2. 启动游戏
//     controller.startGame();

//     CHECK_EQ(model.gameState(), GameState::Playing);

//     qDebug() << "[OK] startGame";

//     // 3. update 不足生成间隔，不应该生成目标
//     controller.update(100);

//     CHECK_EQ(model.activeTargets().size(), 0);

//     qDebug() << "[OK] update before spawn interval";

//     // 4. update 到达生成间隔，应该生成一个目标
//     controller.update(model.config().spawnIntervalMs());

//     CHECK_EQ(model.activeTargets().size(), 1);

//     TypingTargetEntity* target = firstTarget(model);

//     CHECK_TRUE(target->isActive());
//     CHECK_TRUE(target->isVisible());
//     CHECK_TRUE(!target->letter().isNull());
//     CHECK_EQ(target->sceneRect().width(), 800);
//     CHECK_EQ(target->sceneRect().height(), 1000);
//     CHECK_EQ(target->exitLineRatio(), model.config().exitLineRatio());

//     qDebug() << "[OK] spawn target"
//              << "letter =" << target->letter()
//              << "pos =" << target->position()
//              << "velocity =" << target->velocity();

//     // 5. 输入错误字母：不应该命中，应该记录 rejected input
//     QChar wrongLetter = 'A';
//     if (wrongLetter.toUpper() == target->letter().toUpper()) {
//         wrongLetter = 'B';
//     }
//     if (wrongLetter.toUpper() == target->letter().toUpper()) {
//         wrongLetter = 'C';
//     }

//     const bool wrongHandled = controller.handleLetterInput(wrongLetter);

//     CHECK_TRUE(!wrongHandled);
//     CHECK_EQ(model.activeTargets().size(), 1);
//     CHECK_EQ(model.stats().successCount(), 0);
//     CHECK_EQ(model.stats().missCount(), 0);
//     CHECK_EQ(model.stats().rejectedInputCount(), 1);
//     CHECK_EQ(model.stats().inputCount(), 1);

//     qDebug() << "[OK] rejected input";

//     // 6. 输入正确字母：目标应该被模型移除，success +1，score 增加
//     const QChar rightLetter = target->letter();

//     const bool rightHandled = controller.handleLetterInput(rightLetter);

//     CHECK_TRUE(rightHandled);
//     CHECK_EQ(model.activeTargets().size(), 0);
//     CHECK_EQ(model.stats().successCount(), 1);
//     CHECK_EQ(model.stats().missCount(), 0);
//     CHECK_TRUE(model.stats().score() > 0);
//     CHECK_EQ(model.stats().inputCount(), 2);

//     const int scoreAfterHit = model.stats().score();

//     qDebug() << "[OK] correct input"
//              << "success =" << model.stats().successCount()
//              << "score =" << model.stats().score();

//     // 7. 再 update 一帧，让 controller 清理它自己持有但已经 inactive 的 target
//     controller.update(16);

//     CHECK_EQ(model.activeTargets().size(), 0);

//     qDebug() << "[OK] inactive target cleanup frame";

//     // 8. 测试退场 miss：生成一个目标，然后强制放到退场线
//     controller.update(model.config().spawnIntervalMs());

//     CHECK_EQ(model.activeTargets().size(), 1);

//     TypingTargetEntity* missTarget = firstTarget(model);

//     missTarget->setPosition(QPointF(missTarget->position().x(), missTarget->exitLineY()));

//     controller.update(16);

//     CHECK_EQ(model.activeTargets().size(), 0);
//     CHECK_EQ(model.stats().successCount(), 1);
//     CHECK_EQ(model.stats().missCount(), 1);
//     CHECK_TRUE(model.stats().score() <= scoreAfterHit);

//     qDebug() << "[OK] target exit becomes miss"
//              << "miss =" << model.stats().missCount()
//              << "score =" << model.stats().score();

//     // 9. 测试 miss 达到上限后 game over
//     while (model.gameState() == GameState::Playing &&
//            model.stats().missCount() < model.config().maxMissCount())
//     {
//         controller.update(model.config().spawnIntervalMs());

//         if (model.activeTargets().isEmpty()) {
//             continue;
//         }

//         TypingTargetEntity* t = firstTarget(model);
//         t->setPosition(QPointF(t->position().x(), t->exitLineY()));

//         controller.update(16);
//     }

//     CHECK_EQ(model.stats().missCount(), model.config().maxMissCount());
//     CHECK_EQ(model.gameState(), GameState::End);

//     qDebug() << "[OK] game over by miss limit";

//     // 10. 测试 restart：状态、分数、目标都应该清空并重新开始
//     controller.restartGame();

//     CHECK_EQ(model.gameState(), GameState::Playing);
//     CHECK_EQ(model.activeTargets().size(), 0);
//     CHECK_EQ(model.stats().score(), 0);
//     CHECK_EQ(model.stats().successCount(), 0);
//     CHECK_EQ(model.stats().missCount(), 0);
//     CHECK_EQ(model.stats().inputCount(), 0);
//     CHECK_EQ(model.stats().rejectedInputCount(), 0);

//     qDebug() << "[OK] restartGame";

//     // 11. restart 后仍然可以继续生成目标
//     controller.update(model.config().spawnIntervalMs());

//     CHECK_EQ(model.activeTargets().size(), 1);

//     qDebug() << "[OK] spawn after restart";

//     // 12. stop 后目标应该清空，状态变 End
//     controller.stopGame();

//     CHECK_EQ(model.gameState(), GameState::End);
//     CHECK_EQ(model.activeTargets().size(), 0);

//     qDebug() << "[OK] stopGame";

//     qDebug() << "========== testSpaceWarControllerDrivesTypingModel passed ==========";
// }

// int main(int argc, char* argv[])
// {
//     testSpaceWarControllerDrivesTypingModel();

//     return 0;
// }