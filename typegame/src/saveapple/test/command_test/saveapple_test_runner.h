#ifndef __TYPEGAME_SAVEAPPLE_TEST_RUNNER_H__
#define __TYPEGAME_SAVEAPPLE_TEST_RUNNER_H__

#include <QString>

/* ------------------------------------------------------------------
 // 文件名     : saveapple_test_runner.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 拯救苹果命令行自动化测试运行器
------------------------------------------------------------------ */

class SaveAppleTestRunner final
{
public:
    explicit SaveAppleTestRunner() = default;

    SaveAppleTestRunner(const SaveAppleTestRunner&) = delete;
    SaveAppleTestRunner& operator=(const SaveAppleTestRunner&) = delete;

    bool run(const QString& input_file, const QString& output_file);
};

#endif // __TYPEGAME_SAVEAPPLE_TEST_RUNNER_H__
