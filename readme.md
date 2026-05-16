# 1. 项目概述

## 1.1 项目背景

本项目是“打字通游戏”课程实践项目，目标是通过一个完整的 Qt/C++ 桌面游戏工程，综合训练 GUI 编程、游戏逻辑设计、多线程处理、单元测试、命令行测试、设计模式、工程构建以及 AI 能力接入等内容。

项目要求基于一个完整工程实现两个子游戏：“拯救苹果”和“太空大战”。两个游戏都围绕键盘字母输入展开，通过玩家输入对应字母来完成目标消除、得分、过关或生存等游戏行为。

## 1.2 项目实现范围

本项目主要实现以下内容：

- 游戏启动主界面，用于选择进入不同子游戏；
- “拯救苹果”游戏，包括苹果生成、字母匹配、下落失败、得分统计、难度调节和过关检测；
- “太空大战”游戏，包括敌机生成、字母匹配、子弹追踪、碰撞检测、生命系统、分数系统和难度升级；
- 奖励模式，通过 AI 接口或本地词库生成奖励单词；
- 配置系统，支持对游戏速度、数量、目标值、奖励模式等参数进行调整；
- 音频、日志、埋点等公共能力模块；
- 单元测试和命令行自动化测试能力；
- CMake 构建与 Windows 一键构建脚本支持。

## 1.3 技术选型

本项目主要使用以下技术：

- 开发语言：C++
- GUI 框架：Qt
- 构建工具：CMake
- 单元测试框架：Google Test
- 工程环境：Visual Studio / Windows
- 数据格式：JSON
- 架构模式：MVC 分层设计
- 扩展能力：AI 大模型接口、本地降级词库、多线程异步处理

Qt 负责窗口展示、事件响应、定时器、资源管理和跨平台接口封装；CMake 用于统一管理工程构建；Google Test 用于对核心逻辑进行自动化验证。

## 1.4 项目目标

本项目的核心目标不仅是完成两个可运行的打字游戏，还包括提升整体工程质量。具体目标包括：

- 保证游戏功能完整，满足作业需求中的核心验收点；
- 将界面层与游戏逻辑层解耦，方便后续维护和测试；
- 复用两个游戏之间相似的基础能力，减少重复代码；
- 支持图形界面运行和命令行测试运行两种模式；
- 通过单元测试和自动化测试提高代码可靠性；
- 通过日志、埋点和配置系统提升工程可维护性；
- 通过异步处理避免耗时任务阻塞主线程；
- 关注启动速度、运行流畅度、分辨率适配和 DPI 适配。

## 1.5 项目运行方式

项目支持两种主要运行方式。

### 1.5.1 图形界面运行

用户可以直接启动主程序：

```
typegame.exe
```

程序启动后进入游戏主界面，用户可以选择进入“拯救苹果”或“太空大战”，并在对应游戏中进行参数设置和游戏操作。

### 1.5.2 命令行测试运行

项目同时支持命令行测试模式：

typegame.exe <game_name> --test --input <config_file> --output <result_file>

其中：

- <game_name> 表示游戏名称；
    - apple：拯救苹果；
    - space：太空大战；
--test 表示以测试模式启动；
--input <config_file> 表示输入测试配置文件；
--output <result_file> 表示输出测试结果文件。

命令行测试模式主要用于自动化验证游戏逻辑，例如对象生成、字母匹配、分数统计、生命值扣减、配置加载等功能。

### 1.6 项目特点

- 本项目相比普通课程练习，更接近一个完整的小型桌面游戏工程，主要特点包括：
- 同一工程内实现两个子游戏，体现模块复用和扩展能力；
- 使用 MVC 思想拆分界面、数据和控制逻辑；
- 支持 GUI 模式和命令行测试模式；
- 引入单元测试，提升核心逻辑可靠性；
- 使用多线程处理音频播放、AI 请求等耗时任务；
- 太空大战奖励模式结合 AI 能力，并支持接口失败后的本地词库降级；
- 关注编码规范、工程构建、性能体验和兼容性适配。

---

# 2. 总体架构设计

## 2.1 总体架构概览

本项目采用“应用层 + 公共复用层 + 具体游戏层”的整体架构。应用层负责程序启动、主界面展示和游戏选择；公共复用层负责抽象通用接口、打字游戏基础逻辑、公共服务能力和通用视图组件；具体游戏层则分别实现“拯救苹果”和“飞机大战”的业务规则、界面表现和游戏流程。

项目整体结构如下：

```
app/
  application/
  factory/
  mainwindow/
  resources/

common/
  core/
  factory/
  pool/
  service/
    ai/
    audio/
    config/
    log/
    telemetry/
  test/
  typing/
    entity/
  view/

saveapple/
  controller/
  factory/
  model/
  service/
    audio/
    log/
    telemetry/
  test/
  view/

spacewar/
  controller/
  model/
    entity/
  service/
    audio/
    log/
    telemetry/
  test/
  view/
```

其中，`app` 层通过 `IGame` 接口统一管理不同游戏，不直接关心具体游戏内部的 Model、Controller、View 细节。`saveapple` 和 `spacewar` 作为两个独立游戏模块，对外都实现统一的游戏接口，对内则复用 `common` 中的公共能力。

整体架构可以概括为：

```text
                 ┌────────────────────────┐
                 │        app 应用层       │
                 │ MainWindow / GameWindow │
                 │      GameFactory        │
                 └───────────┬────────────┘
                             │ IGame
          ┌──────────────────┴──────────────────┐
          │                                     │
┌─────────▼─────────┐                 ┌─────────▼─────────┐
│   saveapple 模块   │                 │   spacewar 模块    │
│ Model/Controller  │                 │ Model/Controller  │
│ View/Observer     │                 │ View/Observer     │
└─────────┬─────────┘                 └─────────┬─────────┘
          │                                     │
          └──────────────────┬──────────────────┘
                             │
                 ┌───────────▼────────────┐
                 │       common 公共层      │
                 │ core / typing / service │
                 │ pool / view / factory   │
                 └────────────────────────┘
```

这种设计使两个游戏既能共享通用能力，又能保留各自独立的玩法扩展空间。

## 2.2 分层设计

项目整体可以划分为五个层次：

```text
1. 应用启动层：app
2. 核心抽象层：common/core
3. 打字游戏公共逻辑层：common/typing
4. 公共服务与工具层：common/service、common/pool、common/view
5. 具体游戏实现层：saveapple、spacewar
```

### 2.2.1 应用启动层 app

`app` 层负责程序启动、主界面展示、游戏选择和游戏窗口承载。它不直接实现任何具体游戏规则。

主要组成如下：

| 目录 | 作用 |
|---|---|
| `app/application` | 封装应用启动流程，创建并显示主窗口 |
| `app/mainwindow` | 实现主界面 `MainWindow` 和游戏承载窗口 `GameWindow` |
| `app/factory` | 通过 `GameFactory` 根据游戏类型创建具体游戏对象 |
| `app/resources` | 统一管理图片、音频、Qt 资源文件和资源路径 |

`MainWindow` 负责展示游戏入口卡片，例如“拯救苹果”和“飞机大战”。当用户点击某个游戏入口时，`MainWindow` 调用 `GameFactory::createGame()` 创建对应的游戏对象。

`GameFactory` 返回的是 `std::unique_ptr<IGame>`，因此主界面只依赖统一接口，不需要直接操作 `SaveAppleGame` 或 `SpaceWarGame` 的内部细节。

`GameWindow` 接收一个 `IGame` 对象，并通过 `gameWidget()` 获取具体游戏页面，将其设置为窗口中央控件：

```text
MainWindow
  → GameFactory::createGame()
  → std::unique_ptr<IGame>
  → GameWindow
  → game->gameWidget()
  → setCentralWidget()
```

这种方式使应用层和游戏层之间通过 `IGame` 解耦，后续如果新增其他打字游戏，也可以继续通过 `GameFactory` 接入。

### 2.2.2 核心抽象层 common/core

`common/core` 是项目最基础的抽象层，定义了多个游戏模块共同遵守的接口和事件结构。

主要文件包括：

```text
common/core/igame.h
common/core/igame_controller.h
common/core/igame_observer.h
common/core/game_event.h
common/core/game_event_subject_base.h
common/core/game_state.h
```

其中几个核心抽象如下：

| 抽象 | 作用 |
|---|---|
| `IGame` | 具体游戏对外暴露的统一接口 |
| `IGameObserver` | 游戏事件观察者接口 |
| `GameEvent` | 游戏事件数据结构 |
| `GameEventSubjectBase` | 事件主题基类，负责管理观察者并分发事件 |
| `GameState` | 游戏状态枚举，例如 Initial、Playing、Paused、End |

`IGame` 是应用层和具体游戏模块之间最重要的边界。`SaveAppleGame` 和 `SpaceWarGame` 都实现了该接口，对外提供统一的生命周期方法：

```text
initialize()
startGame()
pauseGame()
resumeGame()
stopGame()
restartGame()
update()
gameState()
gameWidget()
exitGame()
```

因此，`app` 层只需要面向 `IGame` 编程，而不用关心不同游戏的内部实现。

### 2.2.3 打字游戏公共逻辑层 common/typing

`common/typing` 是本项目中非常关键的游戏逻辑复用层。它并不是普通工具目录，而是抽象了“打字目标 + 字母输入 + 命中判断 + 统计更新”这一类游戏的共同规则。

主要文件包括：

```text
common/typing/typing_game_model_base.h
common/typing/typing_game_controller_base.h
common/typing/typing_target_entity.h
common/typing/typing_game_config.h
common/typing/typing_game_stats.h

common/typing/entity/game_object_base.h
common/typing/entity/game_entity.h
```

这一层主要负责：

- 游戏状态管理；
- 游戏配置管理；
- 分数、成功次数、失败次数、输入正确率等统计；
- 场上目标列表管理；
- 字母占用与释放；
- 字母输入匹配；
- 目标生成注册；
- 目标移除；
- 命中、漏掉、退场、销毁等通用语义；
- 游戏胜利和失败判断；
- 通用游戏事件通知。

`TypingGameModelBase` 是两个游戏共享的核心模型基类。它内部维护活跃目标列表 `activeTargets()` 和已占用字母集合，避免同一时刻出现重复字母目标。它还提供 `handleInput()`，用于根据玩家输入查找对应字母目标。

在默认逻辑中：

```text
输入字母
  → 查找对应 TypingTargetEntity
  → 命中目标
  → 记录成功次数和分数
  → 移除目标
  → 发送事件
```

“拯救苹果”基本使用这一套默认语义：玩家输入苹果上的字母后，苹果立即被消除。

但“飞机大战”的规则更复杂。输入字母后敌机不会立即销毁，而是先锁定敌机，再由控制器生成子弹，等待子弹击中后才真正销毁敌机。因此 `SpaceWarModel` 重写了 `TypingGameModelBase` 中的部分虚函数，例如 `onTargetMatched()` 和 `onTargetExited()`，将公共打字规则扩展为飞机大战自己的业务语义。

这说明 `common/typing` 的设计不是简单复用代码，而是提供了一套可扩展的打字游戏基础框架。

### 2.2.4 公共服务层 common/service

项目中的日志、音频、配置、AI、埋点等公共服务能力统一封装在 `common/service` 目录下，而不是直接散落在两个游戏模块内部。

目录结构如下：

```text
common/service/
  ai/
  audio/
  config/
  log/
  telemetry/
```

各子目录职责如下：

| 子目录 | 主要职责 |
|---|---|
| `common/service/ai` | 提供 AI 奖励单词生成能力，支持异步请求和本地词库降级 |
| `common/service/audio` | 封装音频播放接口和 Qt 异步音频实现 |
| `common/service/config` | 读取 JSON 配置文件，管理 Spacewar 可调参数 |
| `common/service/log` | 提供异步日志写入能力 |
| `common/service/telemetry` | 提供异步埋点写入和 JSON 格式化能力 |

需要注意的是，`saveapple/service` 和 `spacewar/service` 并不是重新实现底层服务，而是各自游戏的观察者适配层。例如：

```text
saveapple/service/audio/SaveAppleAudioObserver
saveapple/service/log/SaveAppleLoggerObserver
saveapple/service/telemetry/SaveAppleTelemetryObserver

spacewar/service/audio/SpaceWarAudioObserver
spacewar/service/log/SpaceWarLoggerObserver
spacewar/service/telemetry/SpaceWarTelemetryObserver
```

这些 Observer 负责监听模型发出的 `GameEvent`，然后调用 `common/service` 中的公共日志、音频、埋点服务完成实际处理。

也就是说，公共服务层和具体游戏层之间的关系是：

```text
GameEvent
  → 游戏专属 Observer
  → common/service 公共服务
  → 日志文件 / 音频播放 / 埋点文件
```

这种设计避免了模型直接依赖音频播放器、日志写入器或埋点写入器，降低了业务逻辑与外部服务之间的耦合。

### 2.2.5 具体游戏实现层 saveapple 与 spacewar

具体游戏层分为两个主要模块：

```text
saveapple/
spacewar/
```

两个模块都采用类似的 MVC 思路进行组织：

```text
model/
controller/
view/
service/
test/
```

但两个游戏的复杂度不同。

“拯救苹果”主要实现：

```text
苹果生成
苹果下落
字母匹配
命中消除
失败线判断
分数统计
过关和失败判断
对象池回收
```

“飞机大战”在打字匹配基础上进一步扩展：

```text
敌机生成
敌机轨迹移动
玩家飞机移动
字母锁定敌机
子弹追踪
子弹命中
敌机与玩家碰撞
生命值系统
奖励单词
难度升级
AI 单词生成
```

两个模块都依赖 `common/typing`，但扩展方式不同：

| 游戏 | 对 common/typing 的使用方式 |
|---|---|
| 拯救苹果 | 主要复用默认命中和退场逻辑 |
| 飞机大战 | 复用目标管理和输入匹配，但重写命中、退场、失败判断等语义 |

## 2.3 MVC 架构设计

本项目整体采用 MVC 思想组织游戏模块，但在实现上增加了 `Game` 作为模块装配入口，增加了 `Observer` 作为服务解耦机制。

每个具体游戏大致由以下部分组成：

```text
Game
  ├── Model
  ├── Controller
  ├── View
  ├── SettingsDialog
  ├── Service Observers
  └── Timer / Update Loop
```

### 2.3.1 Model：游戏状态与核心业务数据

Model 负责保存游戏核心状态和业务数据，不直接负责界面绘制。

以“拯救苹果”为例：

```text
SaveAppleModel
  继承 TypingGameModelBase
  管理苹果列表、分数、成功次数、失败次数、配置参数
```

以“飞机大战”为例：

```text
SpaceWarModel
  继承 TypingGameModelBase
  管理敌机、子弹、玩家飞机、奖励单词、生命值、难度等级、分数
```

Model 层的主要职责包括：

- 维护游戏状态；
- 保存游戏配置；
- 保存运行时统计；
- 管理活跃目标；
- 处理输入匹配后的状态变化；
- 判断游戏胜利或失败；
- 发出游戏事件。

Model 不直接播放音频、不直接写日志、不直接写埋点，也不直接控制窗口绘制。

### 2.3.2 Controller：流程控制与游戏循环驱动

Controller 负责协调游戏运行流程，是 Model 和 View 之间的调度中心。

两个游戏的 Controller 都继承或复用 `TypingGameControllerBase` 的公共流程：

```text
initialize()
startGame()
pauseGame()
resumeGame()
stopGame()
restartGame()
update()
handleLetterInput()
```

“拯救苹果”的 `SaveAppleController` 主要负责：

- 定时生成苹果；
- 为苹果分配随机可用字母；
- 设置苹果生成位置和下落速度；
- 调用 Model 更新；
- 回收失活苹果对象。

“飞机大战”的 `SpaceWarController` 负责的内容更多，包括：

- 生成敌机；
- 处理玩家移动；
- 根据字母输入锁定敌机；
- 生成追踪子弹；
- 更新子弹和敌机状态；
- 检测碰撞；
- 管理奖励单词；
- 推进难度等级；
- 清理失效实体；
- 连接 View 的 Qt 信号。

Controller 不直接保存核心业务统计数据，也不负责具体绘制，而是通过调用 Model 和 View 完成游戏流程。

### 2.3.3 View：界面显示与用户交互

View 负责 Qt 界面展示、绘制和用户输入事件，不直接承担核心业务规则。

“拯救苹果”的视图主要包括：

```text
SaveAppleView
SaveAppleSettingsDialog
```

“飞机大战”的视图主要包括：

```text
SpaceWarView
SpaceWarPlayfieldWidget
SpaceWarSettingsDialog
SpaceWarFailDialog
```

View 层主要负责：

- 绘制背景、角色、目标、按钮和状态栏；
- 接收键盘输入；
- 接收按钮点击；
- 显示设置弹窗；
- 刷新分数、生命值、难度等 HUD 信息；
- 根据 Model 状态更新界面表现。

其中，`SpaceWarPlayfieldWidget` 负责飞机大战主战斗区域的绘制，`SpaceWarView` 负责整体界面组合和 HUD 刷新。

### 2.3.4 Game：模块装配入口

每个游戏都有一个总入口类：

```text
SaveAppleGame
SpaceWarGame
```

它们都实现 `IGame` 接口，是应用层能够直接管理的游戏对象。

`Game` 类的职责不是实现具体玩法，而是负责组装模块：

- 创建 Model；
- 创建 Controller；
- 创建 View；
- 创建设置弹窗；
- 创建日志、埋点、音频等服务；
- 创建并注册 Observer；
- 创建定时器；
- 启动和停止游戏循环；
- 向 `GameWindow` 返回游戏主页面控件。

以 `SaveAppleGame` 为例，其内部会创建：

```text
SaveAppleModel
AppleFactory
SaveAppleController
SaveAppleView
SaveAppleSettingsDialog
AsyncLogger
AsyncTelemetryWriter
QtAudioService
SaveAppleLoggerObserver
SaveAppleTelemetryObserver
SaveAppleAudioObserver
QTimer
```

以 `SpaceWarGame` 为例，其内部会创建：

```text
SpaceWarModel
SpaceWarController
SpaceWarView
SpaceWarSettingsDialog
AsyncLogger
AsyncTelemetryWriter
QtAudioService
SpaceWarLoggerObserver
SpaceWarTelemetryObserver
SpaceWarAudioObserver
QTimer
```

因此，`Game` 类可以理解为每个游戏模块内部的“装配器”或“组合根”，负责把 MVC、服务层和更新循环连接起来。

## 2.4 事件驱动与观察者机制

项目使用观察者模式降低游戏核心逻辑与外部服务之间的耦合。

事件机制的核心类位于：

```text
common/core/game_event.h
common/core/game_event_subject_base.h
common/core/igame_observer.h
```

`TypingGameModelBase` 继承自 `GameEventSubjectBase`，因此 Model 可以作为事件源向外广播游戏事件。事件类型统一定义在 `GameEventType` 中，例如：

```text
GameInitialized
GameStarted
GamePaused
GameResumed
GameStopped
GameStateChanged
ScoreChanged
LevelChanged
HpChanged
EntitySpawned
EntityRemoved
EntityHit
EntityMissed
InputAccepted
InputRejected
SettingsChanged
GameOver
```

事件流如下：

```text
Model 状态变化
  → 构造 GameEvent
  → notifyObservers()
  → Observer 接收事件
  → 执行对应行为
```

不同 Observer 对同一个事件可以做不同处理：

| Observer | 处理内容 |
|---|---|
| View | 刷新界面显示 |
| AudioObserver | 播放音效或背景音乐 |
| LoggerObserver | 写入日志 |
| TelemetryObserver | 写入埋点数据 |
| SignalBridge | 将模型事件转换为 Qt 信号 |

这种机制的好处是 Model 不需要知道“谁在监听事件”，也不需要直接依赖音频、日志、埋点等具体实现。后续如果增加新的统计模块或调试模块，只需要新增 Observer 并注册即可。

## 2.5 公共服务接入方式

本项目将底层服务实现放在 `common/service`，将游戏相关的事件适配放在各自游戏目录的 `service` 下。

以日志为例，整体链路如下：

```text
SaveAppleModel / SpaceWarModel
  → 发出 GameEvent
  → SaveAppleLoggerObserver / SpaceWarLoggerObserver
  → AsyncLogger
  → logs/*.log
```

以埋点为例：

```text
SaveAppleModel / SpaceWarModel
  → 发出 GameEvent
  → SaveAppleTelemetryObserver / SpaceWarTelemetryObserver
  → AsyncTelemetryWriter
  → telemetry/*.jsonl
```

以音频为例：

```text
SaveAppleModel / SpaceWarModel
  → 发出 GameEvent
  → SaveAppleAudioObserver / SpaceWarAudioObserver
  → QtAudioService
  → Qt 音频播放
```

这样的设计把“事件解释逻辑”和“底层服务能力”分开：

- `common/service` 只关心如何完成服务能力；
- 游戏专属 Observer 只关心某个游戏事件应该触发什么服务行为；
- Model 只关心游戏状态变化，不关心外部服务实现。

## 2.6 游戏运行流程设计

### 2.6.1 GUI 模式启动流程

正常图形界面模式下，程序启动流程如下：

```text
main.cpp
  → QApplication
  → GameApplication::initialize()
  → 创建 MainWindow
  → MainWindow 显示游戏选择主页
  → 用户点击游戏卡片
  → GameFactory::createGame()
  → 创建 SaveAppleGame 或 SpaceWarGame
  → 创建 GameWindow
  → GameWindow 设置 game->gameWidget()
  → game->initialize()
  → 用户开始游戏
```

对应关系如下：

```text
main.cpp
 负责区分运行模式

GameApplication
 负责创建主窗口

MainWindow
 负责显示游戏入口

GameFactory
 负责创建具体游戏对象

GameWindow
 负责承载具体游戏界面

SaveAppleGame / SpaceWarGame
 负责具体游戏模块装配和生命周期管理
```

### 2.6.2 游戏主循环流程

游戏启动后，通过 `QTimer` 驱动更新循环。

“拯救苹果”中，定时器以固定间隔调用：

```text
SaveAppleGame::update(16)
```

其主要流程为：

```text
QTimer timeout
  → SaveAppleGame::update()
  → SaveAppleController::update()
  → 尝试生成苹果
  → SaveAppleModel::update()
  → 更新苹果位置
  → 判断命中、漏掉、过关、失败
  → 回收失活苹果
  → SaveAppleView 刷新界面
```

“飞机大战”中，定时器使用 `Qt::PreciseTimer`，并通过 `QElapsedTimer` 计算真实帧间隔：

```text
QTimer timeout
  → 计算 deltaMs
  → SpaceWarGame::update(deltaMs)
  → SpaceWarController::update(deltaMs)
  → 更新敌机、子弹、玩家、奖励、碰撞、难度
  → SpaceWarModel 更新状态和事件
  → SpaceWarView 刷新 HUD 和场景
```

飞机大战还记录更新耗时、最大 delta 和平均 update 时间，用于辅助发现主循环卡顿问题。

### 2.6.3 输入处理流程

两个游戏都围绕键盘输入展开，但输入后的语义不同。

“拯救苹果”的输入流程：

```text
玩家输入字母
  → SaveAppleView 捕获按键
  → SaveAppleController::handleLetterInput()
  → TypingGameModelBase::handleInput()
  → 查找对应 AppleModel
  → 命中后直接消除
  → 更新分数和成功次数
  → 发出 EntityHit / ScoreChanged 事件
  → 回收苹果对象
  → 刷新界面
```

“飞机大战”的输入流程：

```text
玩家输入字母
  → SpaceWarView 捕获按键
  → SpaceWarController::handleLetterInput()
  → 优先尝试推进奖励单词
  → 如果不是奖励输入，则查找对应敌机
  → SpaceWarModel::onTargetMatched()
  → 锁定敌机
  → Controller 生成子弹
  → 子弹追踪敌机
  → 子弹命中后 destroyEnemyByBullet()
  → 更新分数并移除敌机
  → 发出事件并刷新界面
```

这体现了公共打字逻辑和具体游戏逻辑之间的关系：公共层负责“字母和目标的匹配”，具体游戏负责解释“匹配之后发生什么”。

## 2.7 测试运行架构

项目在 `main.cpp` 中对运行模式进行了区分，支持三种入口：

```text
普通 GUI 模式
命令行测试模式 --test
Google Test 单元测试模式 --unittest
```

当检测到 `--test` 参数时，程序使用 `QCoreApplication` 启动，不进入 GUI，而是解析命令行参数并选择对应测试运行器：

```text
apple
  → SaveAppleTestRunner

spacewar
  → SpaceWarTestRunner
```

当检测到 `--unittest` 参数时，程序同样使用 `QCoreApplication` 启动，并调用 Google Test：

```text
::testing::InitGoogleTest()
RUN_ALL_TESTS()
```

测试相关代码主要分布在：

```text
saveapple/test/
spacewar/test/
common/test/
```

这种设计让游戏核心逻辑可以脱离 GUI 进行验证，符合 MVC 架构中“业务逻辑可测试”的设计目标。

## 2.8 架构设计特点

综合来看，本项目的总体架构具有以下特点：

### 2.8.1 应用层与游戏层通过 IGame 解耦

`app` 层只需要通过 `IGame` 调用游戏生命周期方法和获取游戏窗口控件，不需要知道具体游戏内部如何实现。

### 2.8.2 common/typing 抽象了打字类游戏公共逻辑

两个游戏都围绕“输入字母命中目标”展开，因此项目将目标管理、字母匹配、统计数据、状态切换等逻辑抽象到 `common/typing` 中，提高了复用程度。

### 2.8.3 common/service 集中管理公共服务

日志、音频、配置、AI 和埋点都统一封装在 `common/service` 下，避免每个游戏重复实现底层能力。

### 2.8.4 游戏专属 service 只做事件适配

`saveapple/service` 和 `spacewar/service` 中的类主要是 Observer，用于把通用 `GameEvent` 转换为具体的日志、音频和埋点行为，而不是重新实现公共服务。

### 2.8.5 Model 不直接依赖外部服务

Model 只负责游戏状态和业务规则，通过事件通知外部模块。音频、日志、埋点等能力通过 Observer 接入，降低了耦合度。

### 2.8.6 两个游戏共享框架但保留差异

“拯救苹果”复用公共打字命中的默认语义，而“飞机大战”通过重写公共模型中的部分虚函数，将“输入命中”扩展为“锁定敌机 + 发射子弹 + 子弹命中销毁”。这说明公共层既支持复用，也支持扩展。

### 2.8.7 支持 GUI、命令行测试和单元测试

项目不是单一 GUI 程序，而是在入口层就区分了 GUI 模式、命令行测试模式和单元测试模式，提升了工程可测试性和验收便利性。

---

# 3. 核心模块设计

本节重点说明项目中几个核心模块的内部设计。第二节已经介绍过整体架构、目录划分和 MVC 分层关系，因此本节不再重复整体结构，而是重点描述各模块内部如何组织数据、如何处理流程，以及为什么这样设计。

---

## 3.1 拯救苹果模块设计

“拯救苹果”模块负责实现苹果下落打字游戏。该模块的核心设计目标是：将苹果对象、游戏状态和输入处理逻辑分离，使苹果生成、下落、匹配、计分和扣血等逻辑能够被清晰管理和测试。

### 3.1.1 核心数据设计

拯救苹果模块围绕“苹果对象”和“游戏状态”两类数据展开。

苹果对象主要保存单个苹果在游戏中的状态：

```text
Apple
├── letter        # 苹果对应的字母
├── x             # 横坐标
├── y             # 纵坐标
├── speed         # 下落速度
└── active        # 是否仍然有效
```

游戏状态用于描述当前整局游戏的运行情况：

```text
SaveAppleGameState
├── apples        # 当前屏幕中的苹果列表
├── score         # 当前分数
├── lives         # 剩余生命值
├── targetScore   # 目标分数
├── status        # 游戏状态：运行、暂停、胜利、失败
└── difficulty    # 当前难度参数
```

这样设计的好处是，单个苹果的状态和整局游戏的状态被分开管理，避免所有数据混在控制逻辑中。

### 3.1.2 核心逻辑设计

拯救苹果模块的核心逻辑主要包括四部分：

```text
苹果生成逻辑
苹果下落逻辑
键盘匹配逻辑
游戏结果判断逻辑
```

游戏每一轮更新时，控制器会依次执行以下操作：

```text
更新苹果位置
→ 检查苹果是否落地
→ 处理落地苹果并扣除生命值
→ 检查是否需要生成新苹果
→ 判断游戏是否胜利或失败
→ 通知界面刷新
```

玩家输入字母时，模块会执行匹配逻辑：

```text
接收输入字母
→ 在苹果列表中查找匹配字母
→ 如果匹配成功，移除苹果并增加分数
→ 如果匹配失败，记录错误输入
→ 更新游戏状态
```

### 3.1.3 设计要点

拯救苹果模块的设计重点是“简单逻辑可测试”。

苹果生成、字母匹配、分数变化和生命值扣减都属于核心规则，因此这些逻辑不直接写在界面绘制代码中，而是由控制逻辑统一处理。

这样可以保证：

- 界面只负责显示苹果和分数；
- 游戏规则集中在控制层中；
- 测试时可以直接构造苹果列表并模拟输入；
- 后续调整苹果速度、生成数量或计分规则时，不需要修改界面代码。

---

## 3.2 太空大战模块设计

“太空大战”模块负责实现敌机打字射击游戏。相比“拯救苹果”，该模块的对象更多，流程也更复杂，因此设计上将敌机、子弹、玩家状态和奖励状态分别管理。

### 3.2.1 核心数据设计

太空大战模块主要包含四类核心对象。

第一类是敌机对象：

```text
Enemy
├── letter        # 敌机对应的字母
├── x             # 横坐标
├── y             # 纵坐标
├── speed         # 移动速度
├── active        # 是否仍然有效
└── locked        # 是否已被子弹锁定
```

第二类是子弹对象：

```text
Bullet
├── x             # 当前横坐标
├── y             # 当前纵坐标
├── target        # 目标敌机
├── speed         # 子弹速度
└── active        # 是否仍然有效
```

第三类是玩家状态：

```text
PlayerState
├── lives         # 剩余生命值
├── score         # 当前分数
└── position      # 玩家飞船位置
```

第四类是奖励状态：

```text
RewardState
├── enabled       # 是否处于奖励模式
├── word          # 当前奖励单词
├── input         # 玩家已输入内容
└── timeout       # 奖励剩余时间
```

通过将这些对象拆分管理，可以避免太空大战模块中敌机移动、子弹追踪、奖励输入等逻辑互相干扰。

### 3.2.2 敌机与输入匹配设计

太空大战中的输入不是直接消除敌机，而是先根据输入字母找到目标敌机，再创建子弹攻击目标。

流程如下：

```text
玩家输入字母
→ 查找对应字母的敌机
→ 判断敌机是否存在且未被锁定
→ 创建子弹对象
→ 将子弹目标设置为该敌机
→ 标记敌机为已锁定
```

这样设计可以避免同一个敌机被多个输入重复攻击，也让“输入正确”和“敌机被消灭”之间形成一个完整的射击反馈过程。

### 3.2.3 子弹追踪与碰撞设计

子弹创建后，不是立即消灭敌机，而是在每一帧向目标敌机移动。

子弹更新流程如下：

```text
遍历所有子弹
→ 获取子弹目标敌机
→ 根据目标位置计算移动方向
→ 更新子弹坐标
→ 判断是否与敌机碰撞
→ 命中后移除子弹和敌机
→ 增加分数并触发爆炸效果
```

这种设计增强了游戏表现力，也使玩家能够看到“输入字母 → 发射子弹 → 命中敌机”的完整过程。

碰撞检测可以采用简化的距离判断或矩形区域判断。由于本项目不是复杂物理游戏，因此碰撞检测不需要引入复杂物理引擎，只要保证视觉效果和判定结果一致即可。

### 3.2.4 奖励模式设计

太空大战中的奖励模式是该模块的扩展玩法。奖励模式不直接写入普通敌机逻辑中，而是通过独立的奖励状态进行管理。

奖励模式流程如下：

```text
检测奖励触发条件
→ 进入奖励模式
→ 请求奖励单词
→ 显示奖励单词
→ 接收玩家输入
→ 判断输入是否完整匹配
→ 发放奖励
→ 退出奖励模式
```

奖励模式与普通战斗流程之间保持边界清晰：

- 普通模式处理敌机、子弹和生命值；
- 奖励模式处理单词显示、完整输入和奖励发放；
- 奖励单词由 AI 单词服务模块提供；
- AI 不可用时使用本地词库，不影响游戏继续运行。

### 3.2.5 设计要点

太空大战模块的设计重点是“复杂对象分离管理”。

敌机、子弹、玩家和奖励状态分别建模，可以让每类对象只关心自己的状态变化。控制逻辑负责协调它们之间的关系，例如输入后创建子弹、子弹命中后更新分数、敌机越界后扣除生命值等。

这样设计可以保证：

- 子弹逻辑不会和敌机生成逻辑混在一起；
- 奖励模式不会破坏普通战斗流程；
- 碰撞检测可以单独测试；
- 后续增加新的敌机类型或奖励效果时，修改范围较小。

---

## 3.3 配置模块设计

配置模块用于统一管理游戏参数。该模块的设计重点不是简单读取文件，而是将“参数来源”和“游戏逻辑”分离。

### 3.3.1 配置数据设计

配置模块将游戏运行参数封装为配置对象，例如：

```text
GameConfig
├── appleConfig       # 拯救苹果相关参数
├── spaceConfig       # 太空大战相关参数
├── audioConfig       # 音频相关参数
├── aiConfig          # AI 服务相关参数
└── testConfig        # 测试模式相关参数
```

其中，两个游戏只读取自己需要的配置内容，不直接关心配置文件的读取方式。

### 3.3.2 配置加载流程

配置加载流程如下：

```text
程序启动
→ 读取配置文件
→ 解析 JSON 内容
→ 校验关键字段
→ 生成配置对象
→ 如果配置缺失，使用默认值补全
→ 将配置对象提供给游戏模块
```

### 3.3.3 设计要点

配置模块采用“默认值兜底”的设计。即使配置文件不存在、字段缺失或部分字段格式错误，程序也应尽量使用默认参数继续运行。

这样设计可以提高程序稳定性，也方便命令行测试通过不同输入配置覆盖不同场景。

---

## 3.4 音频模块设计

音频模块负责统一处理游戏音效。它的设计目标是让游戏模块只关心“什么时候播放什么音效”，而不关心音效资源如何加载和播放。

### 3.4.1 音频事件设计

项目中可以将音效抽象为不同事件：

```text
KeyHit          # 输入命中
KeyMiss         # 输入错误
AppleSaved      # 苹果被消除
EnemyHit        # 敌机被击中
Explosion       # 爆炸
Reward          # 奖励触发
GameOver        # 游戏结束
```

游戏模块只需要根据事件调用音频接口：

```text
游戏事件发生
→ 调用音频模块播放对应事件音效
→ 音频模块查找资源并播放
```

### 3.4.2 设计要点

音频模块不参与游戏规则判断。如果音频播放失败，只记录错误或忽略该次播放，不影响游戏继续运行。

这样设计可以保证音效是体验增强功能，而不是游戏主流程的依赖项。

---

## 3.5 日志与埋点模块设计

日志模块和埋点模块都用于记录运行信息，但二者关注点不同。

日志主要用于开发调试和问题定位；埋点主要用于统计玩家行为和游戏过程数据。

### 3.5.1 日志模块设计

日志模块记录程序运行过程中的关键事件，例如：

```text
程序启动
配置加载成功或失败
游戏开始
游戏暂停
游戏结束
AI 请求失败
异常情况
```

日志记录通常包含：

```text
时间
日志级别
模块名称
事件描述
错误信息
```

### 3.5.2 埋点模块设计

埋点模块记录更偏统计意义的事件，例如：

```text
game_start
game_over
key_input
key_match_success
key_match_failed
score_changed
life_changed
reward_triggered
```

这些事件可以用于统计：

- 玩家输入次数；
- 正确率；
- 平均得分；
- 游戏时长；
- 奖励模式触发次数。

### 3.5.3 设计要点

日志和埋点不直接影响游戏状态。它们都属于旁路模块，由 Controller 在关键事件发生时调用。

这样可以避免统计逻辑侵入核心游戏逻辑，也方便后续关闭、替换或扩展记录方式。

---

## 3.6 AI 单词服务模块设计

AI 单词服务模块主要服务于太空大战的奖励模式。该模块的设计目标是：优先使用 AI 生成奖励单词，同时保证 AI 不可用时游戏仍然可以继续运行。

### 3.6.1 单词生成流程

AI 单词服务的调用流程如下：

```text
奖励模式触发
→ Controller 请求奖励单词
→ AI 单词服务尝试调用外部接口
→ 判断调用结果
    → 成功：返回 AI 生成的单词
    → 失败：从本地词库选择单词
→ 返回最终奖励单词
```

### 3.6.2 降级设计

AI 服务存在网络失败、接口超时、返回格式异常等情况，因此模块需要提供本地降级能力。

降级逻辑如下：

```text
AI 请求失败
→ 记录失败日志
→ 读取本地词库
→ 随机或按规则选择一个单词
→ 返回给奖励模式
```

### 3.6.3 设计要点

AI 模块是游戏的增强能力，不是核心依赖。因此它需要满足三个要求：

- AI 可用时提升游戏趣味性；
- AI 不可用时自动降级；
- AI 请求失败不能导致游戏崩溃或卡死。

通过独立封装 AI 单词服务，太空大战模块只需要请求“一个可用单词”，不需要关心单词来自 AI 还是本地词库。

---

## 3.7 命令行测试模块设计

命令行测试模块用于在不启动完整 GUI 的情况下验证核心游戏逻辑。该模块的设计重点是让测试能够直接驱动游戏逻辑，而不是依赖人工操作界面。

### 3.7.1 测试输入设计

命令行测试通过输入文件描述测试场景，例如：

```text
测试游戏类型
初始生命值
初始分数
初始对象列表
输入序列
期望分数
期望生命值
期望游戏状态
```

### 3.7.2 测试执行流程

测试执行流程如下：

```text
读取命令行参数
→ 加载测试输入文件
→ 创建对应游戏状态
→ 按输入序列模拟玩家操作
→ 执行游戏逻辑
→ 得到最终状态
→ 与期望结果比较
→ 输出测试结果
```

### 3.7.3 设计要点

命令行测试模块依赖的是游戏核心逻辑，而不是界面显示。

因此，苹果生成、字母匹配、敌机命中、分数变化、生命值扣减等逻辑必须从界面中抽离出来。这样测试程序才能直接调用这些逻辑，验证结果是否正确。

---

## 3.8 核心模块设计小结

本项目的核心模块设计重点可以概括为以下几点：

- “拯救苹果”模块以苹果对象和游戏状态为核心，重点保证下落、匹配、计分和扣血逻辑清晰；
- “太空大战”模块以敌机、子弹、玩家和奖励状态为核心，重点解决对象较多时的状态管理问题；
- 配置模块将可变参数从代码中抽离，提高测试和调参便利性；
- 音频、日志、埋点模块作为公共辅助能力，不直接影响游戏核心状态；
- AI 单词服务通过本地降级机制保证奖励模式稳定；
- 命令行测试模块直接驱动核心逻辑，提高项目可测试性。

通过以上设计，项目的核心逻辑能够保持相对独立，既便于图形界面运行，也便于自动化测试和后续功能扩展。

---

# 4. 关键功能实现

# 4. 关键功能实现

本节重点说明项目中几个关键功能的实现方式。与前面的架构设计和模块设计不同，本节会结合项目中的核心接口和方法，说明这些功能在代码层面是如何组织和实现的。

---

## 4.1 游戏生命周期统一接口实现

项目中两个子游戏都通过统一的 `IGame` 接口对外暴露能力。这样 `app` 层不需要直接依赖“拯救苹果”或“太空大战”的具体实现，只需要通过统一接口控制游戏生命周期。

核心接口如下：

```cpp
class IGame
{
public:
    virtual ~IGame() = default;

    virtual void initialize() = 0;
    virtual void startGame() = 0;
    virtual void pauseGame() = 0;
    virtual void resumeGame() = 0;
    virtual void stopGame() = 0;
    virtual void restartGame() = 0;

    virtual void update(int delta_ms) = 0;

    virtual GameState gameState() const = 0;
    virtual QWidget* gameWidget() = 0;

    virtual void exitGame() = 0;
};
```

该接口主要解决两个问题：

1. 统一不同游戏模块的启动、暂停、恢复、停止和更新流程；
2. 让主程序可以通过 `gameWidget()` 获取当前游戏页面，并挂载到主窗口中。

因此，`SaveAppleGame` 和 `SpaceWarGame` 虽然内部玩法不同，但对外都表现为一个可以被启动、暂停、更新和退出的游戏模块。

---

## 4.2 通用打字目标实体实现

两个游戏的共同点是：屏幕上会出现带有字母或单词的目标，玩家通过键盘输入完成匹配。因此项目抽象出了通用的 `TypingTargetEntity`，用于表示“可被输入命中的目标”。

核心接口如下：

```cpp
class TypingTargetEntity : public GameEntity
{
public:
    QString text() const;
    void setText(const QString& text);

    QChar letter() const;
    void setLetter(QChar letter);

    bool matches(QChar input) const;
    bool matches(const QString& input) const;

    int scoreValue() const;
    void setScoreValue(int value);

    QRectF sceneRect() const;
    void setSceneRect(const QRectF& rect);

    qreal exitLineRatio() const;
    void setExitLineRatio(qreal ratio);

    bool hasReachedExitLine() const;

    TypingTargetDestroyReason destroyReason() const;
    bool isDestroyed() const;

    void markMatched();
    void markReachedExitLine();
    void markHitByBullet();
    void markCollisionWithPlayer();
    void markOutOfScene();
};
```

这个类的作用是将“输入匹配”和“目标状态”统一起来。

在“拯救苹果”中，苹果可以看作一种 `TypingTargetEntity`，玩家输入匹配字母后，苹果直接被标记为 `Matched`。

在“太空大战”中，敌机也可以看作一种 `TypingTargetEntity`，但输入匹配后不会立即销毁，而是先锁定敌机，再由子弹命中后标记为 `HitByBullet`。

这种设计减少了两个游戏中重复的字母匹配代码，也让不同游戏可以在相同基础模型上实现不同玩法。

---

## 4.3 拯救苹果关键功能实现

“拯救苹果”的核心逻辑主要由 `SaveAppleModel` 和 `SaveAppleController` 完成。

其中，`SaveAppleModel` 负责维护苹果列表、分数、命中次数、失败次数等游戏状态；`SaveAppleController` 负责苹果生成、输入处理和状态更新。

### 4.3.1 苹果模型接口

`SaveAppleModel` 中与苹果管理和输入命中相关的核心接口如下：

```cpp
class SaveAppleModel final : public TypingGameModelBase
{
public:
    void initialize() override;
    void resetModel() override;

    bool registerApple(AppleModel* apple);
    bool unregisterApple(AppleModel* apple);

    bool hitAppleByLetter(QChar input_letter);
    AppleModel* findAppleByLetter(QChar letter) const;

    bool canSpawnApple() const;
    bool isLetterAvailable(QChar letter) const;
    void clearActiveApples();

    int score() const;
    int successCount() const;
    int missCount() const;
    int accuracy() const;
    bool isLevelCompleted() const;

    QList<AppleModel*> activeApples() const;

    int currentAppleFallSpeed() const;

protected:
    int hitScoreValue(const TypingTargetEntity* target) const override;
};
```

其中几个关键方法的作用如下：

- `registerApple()`：将新生成的苹果加入当前活动目标列表；
- `hitAppleByLetter()`：根据玩家输入字母查找并命中苹果；
- `findAppleByLetter()`：查找指定字母对应的苹果对象；
- `canSpawnApple()`：判断当前是否还能继续生成苹果；
- `isLetterAvailable()`：避免生成重复字母的苹果；
- `currentAppleFallSpeed()`：根据速度等级计算当前苹果下落速度；
- `hitScoreValue()`：重写命中得分计算逻辑，使苹果得分可以结合速度奖励。

### 4.3.2 苹果控制器接口

`SaveAppleController` 中负责驱动苹果游戏运行的核心接口如下：

```cpp
class SaveAppleController final : public TypingGameControllerBase
{
public:
    explicit SaveAppleController(SaveAppleModel* model,
                                 AppleFactory* apple_factory);

    void initialize() override;
    void stopGame() override;
    void restartGame() override;

    void update(int delta_ms) override;
    bool handleLetterInput(QChar input_letter) override;

    void setSceneWidth(int scene_width);
    int sceneWidth() const;

    void setSpawnY(int spawn_y);
    int spawnY() const;

    void setSpawnIntervalMs(int spawn_interval_ms);
    int spawnIntervalMs() const;

private:
    void resetSpawnTimer();
    void trySpawnApple();
    QChar generateAvailableLetter() const;
    int generateSpawnX() const;

    void recycleInactiveApples(const QList<AppleModel*>& previous_apples);
};
```

其中，`update()` 是拯救苹果游戏的主循环入口。每次更新时，控制器会推进苹果状态，并判断是否需要生成新的苹果。

核心流程如下：

```text
update(delta_ms)
→ 累加生成计时器
→ 调用 trySpawnApple() 尝试生成苹果
→ 更新已有苹果位置
→ 判断苹果是否到达退场线
→ 回收失效苹果
→ 判断胜利或失败
→ 通知界面刷新
```

玩家输入字母时，会进入 `handleLetterInput()`：

```text
handleLetterInput(input_letter)
→ 调用 SaveAppleModel::hitAppleByLetter()
→ 如果命中，更新分数和成功次数
→ 如果未命中，记录错误输入
→ 回收已经失效的苹果
→ 通知界面刷新
```

这种实现方式将“生成苹果”和“命中苹果”都放在控制器中调度，而真正的状态数据由模型保存，符合 MVC 分层思路。

---

## 4.4 太空大战关键功能实现

“太空大战”的核心逻辑比“拯救苹果”更复杂，主要包括敌机、子弹、玩家血量、奖励单词和难度升级等状态。

这些状态主要由 `SpaceWarModel` 管理，游戏流程由 `SpaceWarController` 推进。

### 4.4.1 太空大战模型接口

`SpaceWarModel` 中的核心接口如下：

```cpp
class SpaceWarModel final : public TypingGameModelBase
{
public:
    void initialize() override;
    void resetModel() override;
    void update(int deltaMs) override;

    PlayerEntity* player() const;

    int playerHealth() const;
    int playerMaxHealth() const;

    int applyPlayerDamage(int amount = 1);
    int applyPlayerHeal(int amount = 1);

    EnemyEntity* tryLockEnemyByLetter(QChar letter);

    void destroyEnemyByBullet(EnemyEntity* enemy);
    void destroyEnemyByCollision(EnemyEntity* enemy);

    const QList<BulletEntity*>& activeBullets() const;
    bool registerBullet(BulletEntity* bullet);
    bool unregisterBullet(BulletEntity* bullet);

    RewardWordEntity* activeReward() const;
    bool registerReward(RewardWordEntity* reward);
    bool advanceRewardInput(QChar input);
    void completeReward();
    void expireReward();

    int difficultyLevel() const;
    void setDifficultyLevel(int level);
    bool upgradeDifficulty();

    int maxEnemyCount() const;
    void setMaxEnemyCount(int count);

    int enemySpeedLevel() const;
    void setEnemySpeedLevel(int level);

    bool rewardModeEnabled() const;
    void setRewardModeEnabled(bool enabled);

    int currentEnemyMoveSpeed() const;
    int currentEnemySpawnIntervalMs() const;

    int score() const;

protected:
    void onTargetMatched(TypingTargetEntity* target) override;
    void onTargetExited(TypingTargetEntity* target) override;

    bool shouldGameOver() const override;
    bool shouldCompleteLevel() const override;
};
```

其中几个关键方法体现了太空大战和拯救苹果的区别：

- `tryLockEnemyByLetter()`：输入字母后只锁定敌机，不直接销毁；
- `destroyEnemyByBullet()`：子弹命中后才真正销毁敌机并加分；
- `destroyEnemyByCollision()`：敌机撞到玩家时扣除玩家生命值；
- `registerBullet()` / `unregisterBullet()`：维护当前活动子弹列表；
- `registerReward()` / `advanceRewardInput()` / `completeReward()`：维护奖励单词输入流程；
- `upgradeDifficulty()`：负责难度递增。

这种实现将“输入命中”和“敌机销毁”拆成两个阶段，使太空大战具有更完整的射击反馈。

### 4.4.2 太空大战控制器接口

`SpaceWarController` 负责生成敌机、创建子弹、检测碰撞、触发奖励和推进难度。

核心接口如下：

```cpp
class SpaceWarController final : public QObject,
                                 public TypingGameControllerBase
{
    Q_OBJECT

public:
    explicit SpaceWarController(SpaceWarModel* model,
                                QObject* parent = nullptr);

    void initialize() override;
    void startGame() override;
    void pauseGame() override;
    void resumeGame() override;
    void restartGame() override;
    void stopGame() override;

    void update(int delta_ms) override;
    bool handleLetterInput(QChar input_letter) override;

    void attachView(SpaceWarView* view);

    void setMoveLeftPressed(bool pressed);
    void setMoveRightPressed(bool pressed);
    void setMoveUpPressed(bool pressed);
    void setMoveDownPressed(bool pressed);

    void applySettingsToRuntime();

    void provideRewardWord(const QString& word);
    void adoptTestEnemy(EnemyEntity* enemy);

private:
    void updateEnemySpawn(int delta_ms);
    bool trySpawnEnemy();
    EnemyEntity* createEnemy();

    BulletEntity* spawnBulletForEnemy(EnemyEntity* enemy);

    void updateRewardSpawn(int delta_ms);
    void updateRewardLifecycle();
    bool isRewardOutOfScene(const RewardWordEntity* reward) const;

    void updateCollision();
    void updateDifficulty(int delta_ms);

    void clearOwnedTargets();
    void clearOwnedBullets();
    void clearOwnedReward();

    void recycleInactiveOwnedTargets();
    void recycleInactiveOwnedBullets();
    void recycleConsumedReward();

    void invalidateBulletsTargetingRemovedEnemies();
};
```

太空大战的 `update()` 方法会在每一帧推进多个子流程：

```text
update(delta_ms)
→ 更新玩家移动
→ 更新敌机生成
→ 更新敌机位置
→ 更新子弹位置
→ 检测子弹与敌机碰撞
→ 检测敌机与玩家碰撞
→ 更新奖励单词生命周期
→ 更新难度
→ 回收失效敌机、子弹和奖励对象
→ 通知界面刷新
```

玩家输入字母时，`handleLetterInput()` 会优先处理奖励单词；如果当前没有奖励输入被消费，则尝试锁定敌机并创建子弹：

```text
handleLetterInput(input_letter)
→ 如果奖励模式存在，尝试推进奖励输入
→ 如果奖励输入未消费，则查找匹配敌机
→ 调用 tryLockEnemyByLetter() 锁定敌机
→ 调用 spawnBulletForEnemy() 创建子弹
→ 注册子弹到模型
→ 通知视图和服务模块
```

这种实现方式让奖励输入和普通敌机输入共用键盘事件，但在逻辑上保持了优先级和边界。

---

## 4.5 子弹追踪与碰撞实现

太空大战中，玩家输入正确字母后不会直接消灭敌机，而是发射一颗子弹。子弹由 `BulletEntity` 表示，它保存目标敌机指针，并在更新过程中向目标移动。

核心接口如下：

```cpp
class BulletEntity : public GameEntity
{
public:
    BulletEntity();
    explicit BulletEntity(int id);

    void update(qreal deltaTime) override;
    void reset() override;

    EnemyEntity* target() const;
    void setTarget(EnemyEntity* target);

    qreal speed() const;
    void setSpeed(qreal speed);

    qreal hitRadius() const;
    void setHitRadius(qreal radius);

    bool hasHitTarget() const;

protected:
    void onUpdate(qreal deltaTime) override;
    void onReset() override;

private:
    QPointF centerPosition() const;
    QPointF targetCenterPosition() const;

    bool isTargetValid() const;
    bool shouldHitTarget(qreal distance) const;

    void moveToTarget(qreal deltaTime);
    void hitTarget();
    void destroySelf();
};
```

子弹追踪流程如下：

```text
创建子弹
→ setTarget(enemy)
→ 每一帧调用 update(deltaTime)
→ 判断目标是否仍然有效
→ 计算子弹当前位置到敌机中心点的方向
→ 按 speed 移动
→ 判断距离是否小于 hitRadius
→ 命中后标记 hasHitTarget
→ 控制器回收子弹并销毁敌机
```

这里的关键点是：子弹自己负责“如何移动到目标”，控制器负责“命中后如何改变游戏状态”。  
这样可以避免控制器中堆积过多几何移动计算逻辑。

---

## 4.6 奖励模式与 AI 单词实现

太空大战中的奖励模式由 `RewardWordEntity` 和 `AiWordService` 配合实现。

`RewardWordEntity` 负责记录奖励单词、玩家已输入内容和奖励状态；`AiWordService` 负责生成奖励单词，并在 AI 请求失败时提供本地词库降级。

### 4.6.1 奖励单词实体

核心接口如下：

```cpp
class RewardWordEntity : public GameEntity
{
public:
    RewardWordEntity();
    explicit RewardWordEntity(int id);

    void update(qreal deltaTime) override;
    void reset() override;

    QString word() const;
    void setWord(const QString& word);

    int inputIndex() const;
    QString typedPart() const;
    QString remainingPart() const;

    bool handleInput(QChar ch);

    bool isCompleted() const;
    bool isExpired() const;

    int healValue() const;
    void setHealValue(int value);

    int scoreValue() const;
    void setScoreValue(int value);

    QRectF sceneRect() const;
    void setSceneRect(const QRectF& sceneRect);

protected:
    void onUpdate(qreal deltaTime) override;
    void onReset() override;

private:
    bool isOutOfScene() const;
    void complete();
    void expire();
};
```

奖励输入流程如下：

```text
奖励模式触发
→ 创建 RewardWordEntity
→ 设置奖励单词 word
→ 玩家输入字符
→ 调用 handleInput(ch)
→ 如果字符匹配当前位置，inputIndex 后移
→ 如果完整输入成功，标记 completed
→ SpaceWarModel::completeReward() 发放回血或加分奖励
```

通过 `typedPart()` 和 `remainingPart()`，界面可以分别显示已输入部分和剩余部分，从而给玩家更清晰的输入反馈。

### 4.6.2 AI 单词服务

AI 单词服务核心接口如下：

```cpp
class AiWordService : public QObject
{
    Q_OBJECT

public:
    using WordCallback =
        std::function<void(const QString& word, bool fromLocal)>;

    static AiWordService& instance();

    void requestRandomWordAsync(const QString& topic,
                                int minLength,
                                int maxLength,
                                WordCallback callback);

private:
    QString requestWordFromDoubao(const QString& topic,
                                  int minLength,
                                  int maxLength,
                                  const QStringList& avoidWords,
                                  int attemptIndex,
                                  QString* failureReasonOut = nullptr) const;

    QString requestWordFromLocal(int minLength,
                                 int maxLength) const;

    QString buildPrompt(const QString& topic,
                        int minLength,
                        int maxLength,
                        const QStringList& avoidWords,
                        int variationHint) const;

    QString parseWordFromResponse(const QByteArray& responseBody,
                                  QString* failureReasonOut = nullptr) const;

    bool isValidWord(const QString& word,
                     int minLength,
                     int maxLength) const;
};
```

AI 单词请求流程如下：

```text
SpaceWarController 触发奖励模式
→ 发出 requestRewardWord 信号
→ 调用 AiWordService::requestRandomWordAsync()
→ 优先请求 AI 接口生成单词
→ 校验返回单词是否合法
→ 成功时通过 callback 返回 AI 单词
→ 失败时调用 requestWordFromLocal()
→ 通过 callback 返回本地词库单词
→ SpaceWarController::provideRewardWord() 创建奖励实体
```

这里使用异步回调的好处是：AI 请求不会阻塞游戏主线程。即使网络请求失败，也可以通过本地词库继续游戏流程。

---

## 4.7 命令行测试功能实现

项目支持命令行测试模式，主要通过 `CommandLineOptions` 解析参数，再由不同游戏的 TestRunner 执行测试。

命令行参数解析接口如下：

```cpp
class CommandLineOptions final
{
public:
    QString game_name;
    QString input_file;
    QString output_file;

    static bool parse(const QStringList& args,
                      CommandLineOptions* options);
};
```

两个游戏分别提供独立的测试运行器：

```cpp
class SaveAppleTestRunner final
{
public:
    bool run(const QString& input_file,
             const QString& output_file);
};
```

```cpp
class SpaceWarTestRunner final
{
public:
    bool run(const QString& input_file,
             const QString& output_file);
};
```

测试模式的执行流程如下：

```text
程序启动
→ CommandLineOptions::parse() 解析参数
→ 根据 game_name 判断测试目标
→ apple 调用 SaveAppleTestRunner::run()
→ space 调用 SpaceWarTestRunner::run()
→ 读取 input_file 中的测试配置
→ 初始化对应游戏模型和控制器
→ 模拟输入或对象状态
→ 执行核心逻辑
→ 将最终结果写入 output_file
```

命令行测试的关键意义在于：测试程序不需要依赖完整 GUI，而是直接调用模型和控制器中的核心逻辑。  
这说明项目中的苹果命中、敌机锁定、子弹命中、奖励输入、分数变化和生命值变化等逻辑已经从界面代码中抽离出来，具备自动化验证能力。

---

## 4.8 关键功能实现小结

本项目的关键功能实现主要体现为以下几点：

1. 使用 `IGame` 统一两个子游戏的生命周期接口，使 `app` 层可以用同一套方式管理不同游戏；
2. 使用 `TypingTargetEntity` 抽象通用打字目标，复用字母匹配、退场和销毁状态逻辑；
3. “拯救苹果”通过 `SaveAppleModel` 和 `SaveAppleController` 实现苹果生成、下落、命中、计分和失败判断；
4. “太空大战”通过 `SpaceWarModel` 和 `SpaceWarController` 实现敌机生成、锁定、子弹攻击、碰撞检测、奖励模式和难度升级；
5. 使用 `BulletEntity` 将子弹追踪逻辑封装到实体内部，降低控制器复杂度；
6. 使用 `RewardWordEntity` 和 `AiWordService` 实现奖励单词输入，并通过本地词库保证 AI 失败时可以降级；
7. 使用 `CommandLineOptions` 和两个 TestRunner 实现命令行测试，使核心逻辑可以脱离 GUI 自动验证。

通过这些接口和方法的组织，项目不仅完成了游戏功能，也体现了较清晰的工程化实现方式。

---

# 5. 质量保障

# 5. 质量保障

本节主要说明项目在开发过程中采用的质量保障措施。项目质量保障的重点不是单纯依赖人工试玩，而是通过单元测试、命令行测试、边界条件处理、日志埋点和异常降级机制，保证核心逻辑的正确性和程序运行的稳定性。

---

## 5.1 单元测试

项目使用 Google Test 对核心逻辑进行单元测试。测试重点放在 Model 和 Controller 层，而不是界面绘制层。

由于项目采用 MVC 分层设计，苹果生成、字母匹配、分数计算、生命值扣减、敌机锁定、子弹命中等逻辑没有直接写在 View 中，因此可以在不启动完整 GUI 的情况下进行测试。

单元测试主要覆盖以下内容：

```text
配置加载逻辑
苹果生成逻辑
苹果字母匹配逻辑
苹果落地扣血逻辑
敌机字母匹配逻辑
敌机锁定逻辑
子弹命中判断逻辑
奖励单词输入逻辑
AI 失败降级逻辑
游戏胜利和失败判断逻辑
```

典型测试流程如下：

```text
构造测试对象
→ 设置初始状态
→ 调用待测试方法
→ 获取实际结果
→ 与期望结果比较
→ 输出测试结果
```

例如，在“拯救苹果”中，可以构造一个带有指定字母的苹果对象，然后模拟玩家输入该字母，检查苹果是否被移除、分数是否增加。  
在“太空大战”中，可以构造敌机和子弹对象，检查敌机是否能被正确锁定，子弹命中后敌机是否被销毁。

---

## 5.2 命令行测试

除单元测试外，项目还支持命令行测试模式，用于验证较完整的游戏流程。

命令行测试可以通过输入文件描述测试场景，并将运行结果输出到结果文件中。

示例命令如下：

```bash
typegame.exe apple --test --input input.json --output result.json
typegame.exe space --test --input input.json --output result.json
```

命令行测试主要验证以下内容：

```text
输入序列执行后分数是否正确
生命值扣减是否正确
苹果或敌机是否正确生成和移除
敌机锁定和子弹命中是否正确
奖励模式是否能正常触发
AI 不可用时是否能使用本地词库
最终游戏状态是否符合预期
```

命令行测试的优势是可以脱离 GUI 执行，适合批量运行和回归测试。  
每次修改核心逻辑后，可以重新运行测试用例，检查已有功能是否被破坏。

---

## 5.3 边界条件处理

游戏运行中存在多种边界情况，如果处理不当，容易导致程序异常或游戏状态错误。因此项目对核心边界场景进行了处理。

重点边界场景包括：

```text
苹果数量为 0
苹果数量达到上限
敌机数量达到上限
玩家输入不存在的字母
同一敌机被重复输入
子弹目标已经失效
生命值减少到 0
分数达到目标值
配置文件不存在
配置字段缺失
AI 请求超时或失败
本地词库兜底
```

边界处理的目标是保证程序在异常输入或非理想状态下仍然能够稳定运行。

例如：

```text
输入不存在的字母
→ 不改变当前游戏对象状态
→ 记录错误输入
→ 游戏继续运行

子弹目标敌机已经失效
→ 子弹标记为无效
→ 下一轮更新中统一回收

生命值减少到 0
→ 游戏进入失败状态
→ 停止继续生成新对象
```

这些处理可以减少崩溃和状态错乱的风险。

---

## 5.4 异常处理与降级机制

项目中部分功能依赖外部资源或外部服务，例如配置文件、音频资源和 AI 单词服务。为了避免这些功能失败后影响主游戏流程，项目设计了异常处理和降级机制。

主要处理方式如下：

```text
配置加载失败
→ 使用默认配置继续运行

音频播放失败
→ 记录日志
→ 不影响游戏主流程

AI 请求失败
→ 记录失败原因
→ 使用本地词库生成奖励单词

非法输入
→ 不修改核心游戏状态
→ 游戏继续运行

无效游戏对象
→ 标记为失效
→ 在下一轮更新中统一回收
```

其中，AI 单词服务是最典型的降级场景。

AI 降级流程如下：

```text
请求 AI 奖励单词
→ 如果请求成功，使用 AI 返回结果
→ 如果请求失败，记录日志
→ 从本地词库中选择单词
→ 返回给奖励模式继续使用
```

通过这种方式，AI 服务可以作为游戏亮点存在，但不会成为影响游戏稳定性的强依赖。

---

## 5.5 日志保障

日志模块用于记录程序运行过程中的关键事件，方便开发者定位问题。

日志主要记录以下内容：

```text
程序启动和退出
配置加载结果
游戏开始、暂停、恢复和结束
玩家输入
分数变化
生命值变化
敌机锁定和子弹命中
奖励模式触发
AI 请求失败
异常情况
```

日志的作用主要体现在：

```text
帮助定位程序异常
帮助检查配置是否正确加载
帮助分析游戏状态变化
帮助排查 AI 或资源加载失败问题
```

相比临时使用调试输出，统一日志模块可以让运行信息更加规范，也方便后续维护。

---

## 5.6 埋点保障

埋点模块用于记录游戏过程中的统计事件，主要服务于行为分析和体验优化。

项目中可以记录的核心埋点事件包括：

```text
game_start
game_over
key_input
key_match_success
key_match_failed
score_changed
life_changed
reward_triggered
ai_request_failed
```

通过这些埋点，可以分析以下信息：

```text
玩家输入次数
输入正确率
平均游戏时长
平均得分
失败原因
奖励模式触发频率
AI 服务失败次数
```

日志更偏向问题定位，埋点更偏向数据统计。二者结合可以提高项目的可观察性。

---

## 5.7 构建与运行保障

项目使用 CMake 管理构建流程，并支持 Debug、Release 以及测试目标的统一构建。

构建保障主要体现在：

```text
统一管理源码文件
统一管理 Qt 依赖
统一管理 Google Test 测试目标
支持图形界面程序构建
支持命令行测试运行
支持 Windows 环境下构建脚本
```

统一构建流程可以减少手动配置工程带来的错误，提高项目在不同环境下的可复现性。

---

## 5.8 质量保障小结

本项目的质量保障主要体现在以下几个方面：

- 使用单元测试验证核心方法和核心状态变化；
- 使用命令行测试模拟完整输入流程；
- 针对对象数量、生命值、分数、配置和 AI 请求等边界场景进行处理；
- 对配置、音频和 AI 等外部依赖提供异常处理或降级方案；
- 使用日志记录运行过程，便于问题定位；
- 使用埋点记录关键行为，便于后续分析；
- 使用 CMake 统一构建流程，提高项目可复现性。

通过这些措施，项目不仅能够完成基本功能，也具备一定的稳定性、可测试性和可维护性。

---

# 6. 项目亮点

本项目不仅完成了“拯救苹果”和“太空大战”两个打字游戏的基本功能，还在架构设计、测试能力、AI 扩展、异常处理和工程化方面进行了较完整的设计。相比普通的界面程序，本项目更强调代码结构、可维护性和可测试性。

---

## 6.1 MVC 分层清晰，核心逻辑可独立测试

项目采用 MVC 分层思想，将游戏数据、界面展示和控制逻辑进行拆分。

其中：

- Model 负责保存苹果、敌机、子弹、分数、生命值等核心状态；
- View 负责界面绘制和用户输入接收；
- Controller 负责游戏规则处理、状态更新和模块调度。

这种设计避免了将所有逻辑都写在窗口类中，使核心游戏逻辑可以脱离界面单独测试。

例如，苹果匹配、敌机锁定、子弹命中、分数变化和生命值扣减等逻辑，都可以通过 Model 和 Controller 直接验证，而不需要人工操作 GUI。

这是项目工程化程度较高的一个重要体现。

---

## 6.2 两个子游戏共用公共能力，减少重复代码

项目虽然包含“拯救苹果”和“太空大战”两个玩法不同的游戏，但二者都复用了公共基础模块。

公共模块主要包括：

- 配置模块；
- 音频模块；
- 日志模块；
- 埋点模块；
- AI 单词服务模块；
- 通用打字目标实体。

例如，两个游戏都需要处理字母匹配、分数变化、音效播放和日志记录。如果每个游戏都单独实现这些功能，会造成大量重复代码。

通过公共模块复用，项目在实现两个游戏的同时，保持了较好的代码一致性和可维护性。

---

## 6.3 支持 GUI 模式和命令行测试模式

项目同时支持图形界面运行和命令行测试运行。

GUI 模式用于正常游戏体验，玩家可以通过主界面选择不同游戏并进行操作。

命令行测试模式用于自动化验证核心逻辑，例如：

- 输入指定字母后分数是否增加；
- 苹果落地后生命值是否减少；
- 敌机被锁定后是否生成子弹；
- 子弹命中后敌机是否销毁；
- AI 请求失败时是否能使用本地词库；
- 游戏最终状态是否符合预期。

这种双模式设计使项目不仅能够运行展示，也能够进行自动化测试，减少对人工试玩的依赖。

---

## 6.4 太空大战引入子弹追踪机制，增强游戏反馈

在“太空大战”中，玩家输入正确字母后，敌机不会立即消失，而是先创建子弹，再由子弹追踪并击中敌机。

完整反馈流程如下：

- 玩家输入字母；
- 系统查找并锁定对应敌机；
- 飞船发射子弹；
- 子弹向敌机移动；
- 子弹命中敌机；
- 敌机爆炸并加分。

这种设计相比“输入即消除”的简单方式，游戏表现力更强，也让玩家能够看到更明确的操作反馈。

同时，子弹追踪逻辑被封装在独立实体中，避免控制器承担过多计算细节。

---

## 6.5 AI 奖励单词与本地降级机制结合

太空大战中的奖励模式接入了 AI 单词服务。玩家达到奖励触发条件后，系统可以通过 AI 生成奖励单词，增加游戏的新鲜感和扩展性。

同时，项目没有将 AI 服务设计成强依赖，而是提供了本地词库降级机制。

当 AI 请求失败、网络异常或返回结果不可用时，程序会自动从本地词库中选择单词，保证奖励模式仍然可以继续运行。

这种设计体现了两个优点：

- AI 可用时提升游戏趣味性；
- AI 不可用时不影响游戏主流程稳定性。

因此，AI 模块既是项目亮点，也不会成为影响程序稳定性的风险点。

---

## 6.6 配置化设计提升调试和扩展能力

项目将游戏速度、对象数量、生命值、目标分数、奖励模式等参数抽离到配置系统中管理。

通过配置化设计，可以在不修改源码的情况下调整游戏行为，例如：

- 修改苹果下落速度；
- 修改敌机生成数量；
- 修改子弹速度；
- 修改玩家生命值；
- 修改奖励模式开关；
- 修改测试输入参数。

这种方式方便调试不同难度，也方便命令行测试构造不同场景。

相比将参数直接写死在代码中，配置化设计更灵活，也更符合工程化开发习惯。

---

## 6.7 日志与埋点提高项目可观察性

项目中引入了日志和埋点能力，用于记录程序运行过程和玩家行为数据。

日志主要用于问题定位，例如记录：

- 配置加载结果；
- 游戏开始和结束；
- 玩家输入；
- 分数和生命值变化；
- AI 请求失败；
- 异常情况。

埋点主要用于行为统计，例如记录：

- 游戏开始次数；
- 输入次数；
- 输入正确率；
- 游戏时长；
- 奖励模式触发次数；
- AI 失败次数。

日志和埋点使项目不只是“能运行”，还能够在运行后分析状态和定位问题，提高了项目的可维护性。

---

## 6.8 异常处理和兜底机制较完善

项目针对多种异常情况设计了处理方式，例如：

- 配置文件加载失败时使用默认值；
- 音频播放失败时不中断游戏；
- AI 请求失败时使用本地词库；
- 子弹目标失效时自动回收子弹；
- 输入不存在的字母时不改变核心状态；
- 游戏对象失效后统一回收。

这些处理能够减少程序崩溃和状态错误的风险。

尤其是在 AI、音频、配置等外部依赖上，项目都尽量保证“外部能力失败不影响核心游戏继续运行”。

---

## 6.9 项目亮点小结

本项目的主要亮点可以概括为以下几点：

- 使用 MVC 分层设计，使核心逻辑和界面展示解耦；
- 两个子游戏复用公共模块，减少重复实现；
- 支持 GUI 运行和命令行测试两种模式；
- 太空大战通过子弹追踪和爆炸效果增强操作反馈；
- 奖励模式结合 AI 单词生成，并支持本地词库降级；
- 配置化设计提高调试和扩展能力；
- 日志与埋点提高项目可观察性；
- 异常处理和兜底机制提升程序稳定性。

整体来看，本项目不仅完成了游戏功能，还在架构、测试、扩展和稳定性方面进行了较完整的设计，体现了一个小型 Qt/C++ 桌面游戏项目的工程化实现思路。

---

# 7. 当前问题与改进方向

本项目已经完成两个子游戏的核心功能，并具备 MVC 分层、命令行测试、日志埋点、AI 奖励单词等能力。但从项目完善度来看，仍有一些可以继续优化的地方。

---

## 7.1 界面表现仍可提升

当前游戏界面已经能够完成基本展示，包括苹果下落、敌机移动、子弹追踪和爆炸效果等。但整体视觉效果仍然偏基础，动画过渡和特效表现还可以进一步增强。

后续可以优化：

- 增加更平滑的动画效果；
- 优化爆炸、命中、奖励触发等反馈；
- 改进游戏开始、暂停、胜利和失败界面；
- 继续完善不同分辨率下的界面适配。

---

## 7.2 AI 服务稳定性仍受外部接口影响

太空大战中的奖励模式使用了 AI 单词服务，这是项目的一个亮点。但 AI 服务依赖网络和外部接口，可能出现请求失败、响应较慢或返回格式异常等问题。

目前项目已经使用本地词库作为降级方案，保证 AI 失败时游戏仍能继续运行。

后续可以优化：

- 增加更严格的 AI 返回结果校验；
- 优化请求超时处理；
- 扩展本地词库内容；
- 在界面上提示当前单词来自 AI 还是本地词库。

---

## 7.3 测试覆盖还可以继续扩展

项目已经支持单元测试和命令行测试，能够验证核心游戏逻辑。但目前测试主要覆盖基础场景，复杂场景和边界场景还可以继续补充。

后续可以增加：

- 连续错误输入测试；
- 高难度下大量对象生成测试；
- 暂停、恢复、重新开始测试；
- 奖励模式超时测试；
- 多轮游戏连续运行测试；
- 异常配置文件测试。

这样可以进一步提高项目的稳定性和可靠性。

---

## 7.4 配置系统可以更加细化

当前配置系统已经支持部分游戏参数调整，例如速度、数量、生命值、目标分数和奖励模式等。但仍有一些参数可以继续抽离到配置文件中。

后续可以优化：

- 为不同难度提供独立配置；
- 增加配置合法性校验；
- 增加配置错误提示；
- 支持更多游戏参数配置化。

这样可以减少代码修改，提高调试和扩展效率。

---

## 7.5 性能优化仍有提升空间

当前项目已经通过对象数量控制、失效对象回收、异步 AI 请求等方式减少卡顿风险。但在高难度或大量对象场景下，仍然可以进一步优化。

后续可以考虑：

- 优化碰撞检测逻辑；
- 减少不必要的界面重绘；
- 控制高频日志和埋点写入。

这些优化可以提升游戏在复杂场景下的运行流畅度。

---

## 7.6 改进方向小结

总体来看，项目当前功能已经基本完整，但后续仍可以从以下几个方面继续改进：

- 提升界面动画和视觉反馈；
- 优化 AI 服务稳定性；
- 扩展测试覆盖范围；
- 细化配置系统；
- 优化高难度场景下的性能；
- 继续重构复杂逻辑，提高代码维护性。

这些问题不会影响当前项目的基本运行，但可以作为后续进一步完善项目质量的方向。


---

# 8. 总结

本项目基于 Qt/C++ 实现了“拯救苹果”和“太空大战”两个打字游戏，完成了从界面交互、游戏逻辑、资源管理到测试验证的完整开发流程。

在项目设计上，采用 MVC 分层思想，将界面展示、游戏状态和控制逻辑进行拆分，提高了代码的可维护性和可测试性。两个子游戏复用了配置、音频、日志、埋点和 AI 单词服务等公共模块，减少了重复代码。

在功能实现上，项目完成了苹果下落与字母匹配、敌机生成与子弹追踪、碰撞检测、奖励模式、分数与生命值管理等核心功能。同时，项目支持命令行测试和单元测试，能够对核心逻辑进行自动化验证。

总体来看，本项目不仅完成了课程作业要求中的基本游戏功能，也在架构设计、工程化实现、测试保障和扩展能力方面进行了实践。通过本项目的开发，我对 Qt GUI 开发、C++ 面向对象设计、MVC 架构、自动化测试和项目工程化流程有了更系统的理解。



