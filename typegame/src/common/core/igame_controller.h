#ifndef __TYPEGAME_IGAME_CONTROLLER_H__
#define __TYPEGAME_IGAME_CONTROLLER_H__

#include <QChar>

/* ------------------------------------------------------------------
 // 文件名     : igame_controller.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 通用游戏控制器接口
------------------------------------------------------------------ */

/**
 * @brief 通用游戏控制器接口。
 *
 * 职责：
 * 1. 负责驱动游戏生命周期。
 * 2. 负责处理来自输入层的行为请求。
 * 3. 负责协调 model、view、service 等模块工作。
 *
 * 边界：
 * 1. 控制器不直接承载核心业务数据。
 * 2. 控制器不应承担具体绘制职责。
 * 3. 控制器应尽量作为流程调度中心存在。
 */
class IGameController
{
public:
    virtual ~IGameController() = default;

    /**
     * @brief 初始化控制器。
     */
    virtual void initialize() = 0;

    /**
     * @brief 开始游戏。
     */
    virtual void startGame() = 0;

    /**
     * @brief 暂停游戏。
     */
    virtual void pauseGame() = 0;

    /**
     * @brief 恢复游戏。
     */
    virtual void resumeGame() = 0;

    /**
     * @brief 停止游戏。
     */
    virtual void stopGame() = 0;

    /**
     * @brief 重开游戏。
     */
    virtual void restartGame() = 0;

    /**
     * @brief 驱动一次逻辑更新。
     *
     * @param delta_ms 距离上一次更新的毫秒数
     */
    virtual void update(int delta_ms) = 0;

    /**
     * @brief 处理字母输入。
     *
     * @param input_letter 输入字母
     * @return true 表示输入被成功处理，false 表示未处理
     */
    virtual bool handleLetterInput(QChar input_letter) = 0;
};

#endif // __TYPEGAME_IGAME_CONTROLLER_H__
