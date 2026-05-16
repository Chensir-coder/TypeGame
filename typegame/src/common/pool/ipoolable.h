#ifndef __TYPEGAME_IPOOLABLE_H__
#define __TYPEGAME_IPOOLABLE_H__


/* ------------------------------------------------------------------
 // 文件名     : ipoolable.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 可池化对象接口
------------------------------------------------------------------ */

/**
 * @brief 可池化对象接口。
 *
 * 职责：
 * 1. 约束一个对象在进入对象池复用体系后，至少应具备的基础行为。
 * 2. 为对象池统一调用 reset() 提供抽象。
 *
 * 说明：
 * 1. AppleModel、EnemyModel、BulletModel 等高频创建销毁对象都适合实现该接口。
 * 2. 当前激活状态管理由具体对象自身提供，不在该接口中重复定义。
 */
class IPoolable
{
public:
    virtual ~IPoolable() = default;

    /**
     * @brief 重置对象状态，使其可再次复用。
     */
    virtual void reset() = 0;
};

#endif // __TYPEGAME_IPOOLABLE_H__
