cmake_minimum_required(VERSION 3.10)

if(WIN32)
    set(CMAKE_SYSTEM_VERSION "10.0.18362.0")
endif()

project(typegame)

# 生成 compile_commands.json，供 clangd / IDE 使用（需 Ninja 等支持的生成器）
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

enable_testing()

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Multimedia Network)
find_package(GTest REQUIRED)

if(WIN32)
    set(CMAKE_SYSTEM_VERSION "10.0.18362.0")
    # 设置UTF-8编码以避免乱码
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /utf-8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

# ==============================================
# 自动遍历 src 目录下所有 .cpp .h .qrc 文件
# ==============================================
file(GLOB_RECURSE TYPEGAME_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

file(GLOB_RECURSE TYPEGAME_HEADERS CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

list(FILTER TYPEGAME_SOURCES EXCLUDE REGEX ".*[\\/]single_test[\\/].*")
list(FILTER TYPEGAME_HEADERS EXCLUDE REGEX ".*[\\/]gtest[\\/].*")
list(FILTER TYPEGAME_HEADERS EXCLUDE REGEX ".*[\\/]single_test[\\/].*")

file(GLOB_RECURSE TYPEGAME_QRC CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.qrc
)

set(TYPEGAME_QRC
    src/app/resources/typegame_assets.qrc

)

add_executable(typegame
    ${TYPEGAME_SOURCES}
    ${TYPEGAME_HEADERS}
    ${TYPEGAME_QRC}
)

target_include_directories(typegame
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(typegame
    PRIVATE
        Qt5::Widgets
        Qt5::Gui
        Qt5::Core
        Qt5::Network
        Qt5::Multimedia
        GTest::gmock 
        GTest::gtest 
)

# 可选：便于 VS 工程中按目录查看
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
    ${TYPEGAME_SOURCES}
    ${TYPEGAME_HEADERS}
)

# GoogleTest：SaveApple / SpaceWar 等与主程序共用 src/app/main.cpp，运行时加 --unittest
add_test(NAME typegame_unittest COMMAND $<TARGET_FILE:typegame> --unittest)


# # AI 相关测试
# add_executable(ai_word_service_demo
#     src/common/service/ai/ai_word_service.cpp
#     src/saveapple/test/single_test/ai_word_service_demo.cpp
# )

# target_include_directories(ai_word_service_demo PRIVATE
#     src/common/service/ai
# )

# target_link_libraries(ai_word_service_demo PRIVATE
#     Qt${QT_VERSION_MAJOR}::Core
#     Qt${QT_VERSION_MAJOR}::Network
# )
