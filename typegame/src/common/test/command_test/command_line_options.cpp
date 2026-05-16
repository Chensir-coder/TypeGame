#include "command_line_options.h"

#include <QDebug>

/* ------------------------------------------------------------------
 // 文件名     : command_line_options.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 命令行测试参数解析
------------------------------------------------------------------ */

bool CommandLineOptions::parse(const QStringList& args, CommandLineOptions* options)
{
    if (options == nullptr)
    {
        qDebug() << "[CommandLineOptions] options is nullptr.";
        return false;
    }

    if (args.size() < 6)
    {
        qDebug() << "[CommandLineOptions] invalid argument count:" << args;
        return false;
    }

    options->game_name = args.at(1);

    const int input_index = args.indexOf("--input");
    const int output_index = args.indexOf("--output");

    if (input_index < 0 || input_index + 1 >= args.size())
    {
        qDebug() << "[CommandLineOptions] missing --input.";
        return false;
    }

    if (output_index < 0 || output_index + 1 >= args.size())
    {
        qDebug() << "[CommandLineOptions] missing --output.";
        return false;
    }

    options->input_file = args.at(input_index + 1);
    options->output_file = args.at(output_index + 1);

    if (options->game_name.isEmpty()
        || options->input_file.isEmpty()
        || options->output_file.isEmpty())
    {
        qDebug() << "[CommandLineOptions] empty argument.";
        return false;
    }

    return true;
}
