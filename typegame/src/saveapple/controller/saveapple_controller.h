#ifndef __TYPEGAME_SAVEAPPLE_CONTROLLER_H__
#define __TYPEGAME_SAVEAPPLE_CONTROLLER_H__

#include <QList>

#include "common/typing/typing_game_controller_base.h"

/* ------------------------------------------------------------------
// 文件名     : saveapple_controller.h
// 创建者     : 3263297739@qq.com
// 创建时间   : 2026-04-13
// 功能描述   : 拯救苹果游戏主控制器
------------------------------------------------------------------ */

class SaveAppleModel;
class AppleFactory;
class AppleModel;

/**
 * @brief 拯救苹果游戏主控制器。
 *
 * 继承 TypingGameControllerBase，复用公共控制流程：
 *   - initialize / startGame / pauseGame / resumeGame / stopGame / restartGame
 *   - handleLetterInput → model->handleInput
 *
 * 在此基础上扩展苹果游戏专属逻辑：
 *   - override stopGame / restartGame：在状态变更后回收已失活苹果
 *   - override update：在公共更新前后处理苹果生成与回收
 *   - override handleLetterInput：命中后同步回收失活苹果
 *
 * 保留旧接口：setSceneWidth / sceneWidth / setSpawnY / setSpawnIntervalMs 等。
 */
class SaveAppleController final : public TypingGameControllerBase
{
public:
    explicit SaveAppleController(SaveAppleModel* model, AppleFactory* apple_factory);
    ~SaveAppleController() override = default;

    SaveAppleController(const SaveAppleController&) = delete;
    SaveAppleController& operator=(const SaveAppleController&) = delete;

    void initialize() override;
    void stopGame() override;
    void restartGame() override;
    void update(int delta_ms) override;
    bool handleLetterInput(QChar input_letter) override;

    /* ---- 兼容旧 API：场景宽度（委托给 setSceneSize） ---- */
    void setSceneWidth(int scene_width);
    int sceneWidth() const;

    /* ---- 兼容旧 API：生成起始 Y 坐标 ---- */
    void setSpawnY(int spawn_y);
    int spawnY() const;

    /* ---- 兼容旧 API：生成间隔（毫秒） ---- */
    void setSpawnIntervalMs(int spawn_interval_ms);
    int spawnIntervalMs() const;

private:
    SaveAppleModel* saveAppleModel() const;

    void resetSpawnTimer();
    void trySpawnApple();
    QChar generateAvailableLetter() const;
    int generateSpawnX() const;

    void recycleInactiveApples(const QList<AppleModel*>& previous_apples);
    bool containsApple(const QList<AppleModel*>& apples, AppleModel* target) const;

private:
    AppleFactory* m_apple_factory = nullptr;  ///< 苹果工厂（非拥有型指针）

    int m_spawn_y = -40;           ///< 苹果初始生成 Y 坐标
    int m_spawn_interval_ms = 900; ///< 苹果生成间隔（毫秒）
    int m_elapsed_spawn_ms = 0;    ///< 当前已累计生成时间（毫秒）
};

#endif // __TYPEGAME_SAVEAPPLE_CONTROLLER_H__
