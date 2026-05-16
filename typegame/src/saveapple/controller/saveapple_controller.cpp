/* ------------------------------------------------------------------
// 文件名     : saveapple_controller.cpp
// 创建者     : 3263297739@qq.com
// 创建时间   : 2026-04-13
// 功能描述   : 拯救苹果游戏主控制器实现
------------------------------------------------------------------ */

#include "saveapple_controller.h"

#include <QRandomGenerator>
#include <QString>

#include "saveapple/model/saveapple_model.h"
#include "saveapple/model/apple_model.h"
#include "saveapple/factory/apple_factory.h"

#include <algorithm>

SaveAppleController::SaveAppleController(SaveAppleModel* model, AppleFactory* apple_factory)
    : TypingGameControllerBase(model)
    , m_apple_factory(apple_factory)
{
}

void SaveAppleController::initialize()
{
    TypingGameControllerBase::initialize();
    resetSpawnTimer();
}

void SaveAppleController::stopGame()
{
    auto* sam = saveAppleModel();
    if (sam == nullptr)
    {
        return;
    }

    const QList<AppleModel*> previous_apples = sam->activeApples();

    TypingGameControllerBase::stopGame();

    recycleInactiveApples(previous_apples);
    resetSpawnTimer();
}

void SaveAppleController::restartGame()
{
    auto* sam = saveAppleModel();
    if (sam == nullptr)
    {
        return;
    }

    const QList<AppleModel*> previous_apples = sam->activeApples();

    TypingGameControllerBase::restartGame();

    recycleInactiveApples(previous_apples);
    resetSpawnTimer();
}

void SaveAppleController::update(int delta_ms)
{
    auto* sam = saveAppleModel();
    if (sam == nullptr || m_apple_factory == nullptr)
    {
        return;
    }

    if (delta_ms <= 0)
    {
        return;
    }

    if (sam->gameState() != GameState::Playing)
    {
        return;
    }

    // 生成计时
    m_elapsed_spawn_ms += delta_ms;
    if (m_elapsed_spawn_ms >= m_spawn_interval_ms)
    {
        trySpawnApple();
        m_elapsed_spawn_ms = 0;
    }

    // 更新前快照，用于判断哪些苹果被移除
    const QList<AppleModel*> previous_apples = sam->activeApples();

    // 公共模型更新（移动、退场检测、miss/hit 统计、过关/失败判断）
    sam->update(delta_ms);

    // 回收已失活的苹果
    recycleInactiveApples(previous_apples);
}

bool SaveAppleController::handleLetterInput(QChar input_letter)
{
    auto* sam = saveAppleModel();
    if (sam == nullptr || m_apple_factory == nullptr)
    {
        return false;
    }

    const QList<AppleModel*> previous_apples = sam->activeApples();

    const bool handled = TypingGameControllerBase::handleLetterInput(input_letter);

    if (handled)
    {
        recycleInactiveApples(previous_apples);
    }

    return handled;
}

void SaveAppleController::setSceneWidth(int scene_width)
{
    if (scene_width <= 0)
    {
        return;
    }

    setSceneSize(scene_width, static_cast<int>(sceneRect().height()));
}

int SaveAppleController::sceneWidth() const
{
    return static_cast<int>(sceneRect().width());
}

void SaveAppleController::setSpawnY(int spawn_y)
{
    m_spawn_y = spawn_y;
}

int SaveAppleController::spawnY() const
{
    return m_spawn_y;
}

void SaveAppleController::setSpawnIntervalMs(int spawn_interval_ms)
{
    if (spawn_interval_ms <= 0)
    {
        return;
    }

    m_spawn_interval_ms = spawn_interval_ms;
}

int SaveAppleController::spawnIntervalMs() const
{
    return m_spawn_interval_ms;
}

SaveAppleModel* SaveAppleController::saveAppleModel() const
{
    return static_cast<SaveAppleModel*>(model());
}

void SaveAppleController::resetSpawnTimer()
{
    m_elapsed_spawn_ms = 0;
}

void SaveAppleController::trySpawnApple()
{
    auto* sam = saveAppleModel();
    if (sam == nullptr || m_apple_factory == nullptr)
    {
        return;
    }

    if (!sam->canSpawnApple())
    {
        return;
    }

    const QChar spawn_letter = generateAvailableLetter();
    if (spawn_letter.isNull())
    {
        return;
    }

    AppleModel* apple = m_apple_factory->create();
    if (apple == nullptr)
    {
        return;
    }

    apple->setLetter(spawn_letter);
    apple->setPosition(generateSpawnX(), m_spawn_y);
    apple->setFallSpeed(sam->currentAppleFallSpeed());

    if (!sam->registerApple(apple))
    {
        m_apple_factory->recycle(apple);
    }
}

QChar SaveAppleController::generateAvailableLetter() const
{
    auto* sam = saveAppleModel();
    if (sam == nullptr)
    {
        return QChar();
    }

    QString available_letters;
    for (QChar letter = QChar('A'); letter <= QChar('Z'); letter = QChar(letter.unicode() + 1))
    {
        if (sam->isLetterAvailable(letter))
        {
            available_letters.append(letter);
        }
    }

    if (available_letters.isEmpty())
    {
        return QChar();
    }

    const int index = QRandomGenerator::global()->bounded(available_letters.size());
    return available_letters.at(index);
}

int SaveAppleController::generateSpawnX() const
{
    const int scene_w = static_cast<int>(sceneRect().width());
    const int min_x = 20;
    const int max_x = (scene_w > 40) ? (scene_w - 20) : 20;

    if (max_x <= min_x)
    {
        return min_x;
    }

    return QRandomGenerator::global()->bounded(min_x, max_x);
}

void SaveAppleController::recycleInactiveApples(const QList<AppleModel*>& previous_apples)
{
    if (m_apple_factory == nullptr)
    {
        return;
    }

    auto* sam = saveAppleModel();
    if (sam == nullptr)
    {
        return;
    }

    const QList<AppleModel*> current_apples = sam->activeApples();

    for (AppleModel* apple : previous_apples)
    {
        if (apple == nullptr)
        {
            continue;
        }

        if (!containsApple(current_apples, apple))
        {
            m_apple_factory->recycle(apple);
        }
    }
}

bool SaveAppleController::containsApple(const QList<AppleModel*>& apples, AppleModel* target) const
{
    return std::any_of(apples.begin(), apples.end(),
    [target](const AppleModel* apple)
    {
        return apple == target;
    });
}
