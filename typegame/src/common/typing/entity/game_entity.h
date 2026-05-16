#ifndef __TYPEGAME_GAME_ENTITY_H__
#define __TYPEGAME_GAME_ENTITY_H__

#include "game_object_base.h"

/* ------------------------------------------------------------------
 // 文件名     : game_entity.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : game_entity 头文件声明
------------------------------------------------------------------ */

class GameEntity : public GameObjectBase
{
public:
    GameEntity();
    explicit GameEntity(int id);
    ~GameEntity() override = default;

    GameEntity(GameEntity&&) noexcept = default;
    GameEntity& operator=(GameEntity&&) noexcept = default;

    void update(qreal deltaTime) override;

    bool isActive() const;
    void setActive(bool active);

    bool isVisible() const;
    void setVisible(bool visible);

    void reset() override;

    int currentFrameIndex() const;
    int frameCount() const;

    /*
     * 设置精灵序列帧参数；frames_per_second<=0 时不在 update 中推进帧索引。
     */
    void setFramePlayback(int frameCount, qreal framesPerSecond);

    /*
     * 在 cycleSeconds 秒内按顺序走完 frameCount 帧并循环；
     * 等价 setFramePlayback(frameCount, frameCount / cycleSeconds)。
     */
    void setFramePlaybackCycle(int frameCount, qreal cycleSeconds = 1.0);

protected:
    void advanceFrameAnimation(qreal deltaTime);
    void onReset() override;

protected:
    bool m_active = false;
    bool m_visible = true;

    int m_frameCount = 12;
    int m_frameIndex = 0;
    qreal m_frameFps = 12.0;
    qreal m_frameRemainder = 0.0;
};

#endif // __TYPEGAME_GAME_ENTITY_H__
