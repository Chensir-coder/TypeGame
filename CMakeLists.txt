cmake_minimum_required(VERSION 3.10)  #cmake版本最低3.10

if(WIN32)
    set(CMAKE_SYSTEM_VERSION "10.0.18362.0")
    # 设置UTF-8编码以避免乱码
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

SET(CMAKE_TOOLCHAIN_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/../thirdparty_install/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(TypeGame.sln)  #添加当前项目名称

# 生成 compile_commands.json，供 clangd / IDE「转到定义」解析翻译单元与包含路径
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17) #设置C++标准
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin") # 设置输出目录为顶层 bin 文件夹

add_subdirectory(typegame)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT typegame)  #设置启动项