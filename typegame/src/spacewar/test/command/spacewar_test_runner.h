#ifndef __TYPEGAME_SPACEWAR_TEST_RUNNER_H__
#define __TYPEGAME_SPACEWAR_TEST_RUNNER_H__

#include <QString>

/* ------------------------------------------------------------------
 // 文件名     : spacewar_test_runner.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 太空大战（飞机）命令行自动化测试运行器
------------------------------------------------------------------ */

class SpaceWarTestRunner final
{
public:
    explicit SpaceWarTestRunner() = default;

    SpaceWarTestRunner(const SpaceWarTestRunner&) = delete;
    SpaceWarTestRunner& operator=(const SpaceWarTestRunner&) = delete;

    bool run(const QString& input_file, const QString& output_file);
};

#endif // __TYPEGAME_SPACEWAR_TEST_RUNNER_H__
