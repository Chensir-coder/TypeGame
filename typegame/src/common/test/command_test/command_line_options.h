#ifndef __TYPEGAME_COMMAND_LINE_OPTIONS_H__
#define __TYPEGAME_COMMAND_LINE_OPTIONS_H__

#include <QString>
#include <QStringList>

/* ------------------------------------------------------------------
 // 文件名     : command_line_options.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 命令行测试参数解析
------------------------------------------------------------------ */

class CommandLineOptions final
{
public:
    CommandLineOptions() = default;

    CommandLineOptions(const CommandLineOptions&) = delete;
    CommandLineOptions& operator=(const CommandLineOptions&) = delete;

    QString game_name;
    QString input_file;
    QString output_file;

    static bool parse(const QStringList& args, CommandLineOptions* options);
};

#endif // __TYPEGAME_COMMAND_LINE_OPTIONS_H__
