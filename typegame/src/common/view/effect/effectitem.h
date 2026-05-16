#ifndef __TYPEGAME_EFFECTITEM_H__
#define __TYPEGAME_EFFECTITEM_H__

#include <QPointF>
#include <QSizeF>
#include <QPixmap>
#include <QVector>
#include <QPainter>

/* ------------------------------------------------------------------
 // 文件名     : effectitem.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : effectitem 头文件声明
------------------------------------------------------------------ */

class EffectItem
{
public:
    EffectItem();
    EffectItem(const QPointF& logicalPos,
               const QSizeF& logicalSize,
               const QVector<QPixmap>& frames,
               qreal lifeTime,
               qreal frameInterval = 0.1,
               bool loop = false);

    virtual ~EffectItem() = default;

    virtual void update(qreal dt);
    virtual void paint(QPainter* painter,
                       qreal viewScale,
                       const QPointF& cameraOffset = QPointF(0.0, 0.0)) const;

    bool isAlive() const;
    void kill();

    void setLogicalPosition(const QPointF& pos);
    QPointF logicalPosition() const;

    void setVelocity(const QPointF& vel);
    QPointF velocity() const;

    void setLogicalSize(const QSizeF& size);
    QSizeF logicalSize() const;

    void setLocalScale(qreal scale);
    qreal localScale() const;

    void setRotation(qreal rotation);
    qreal rotation() const;

    void setOpacity(qreal opacity);
    qreal opacity() const;

    void setLoop(bool loop);
    bool loop() const;

    void setLifeTime(qreal lifeTime);
    qreal lifeTime() const;

    void setFrameInterval(qreal interval);
    qreal frameInterval() const;

    qreal elapsedTime() const;

    void setFrames(const QVector<QPixmap>& frames);
    const QVector<QPixmap>& frames() const;

    void setInfiniteLife(bool infinite);
    bool hasInfiniteLife() const;

protected:
    virtual void updateMovement(qreal dt);
    virtual void onUpdate(qreal dt);
    virtual void updateFrame();
    virtual void onLifeEnd();

    int currentFrameIndex() const;
    const QPixmap* currentPixmap() const;

protected:
    QPointF m_logicalPos = QPointF(0.0, 0.0);
    QPointF m_velocity = QPointF(0.0, 0.0);

    QSizeF m_logicalSize = QSizeF(0.0, 0.0);
    QVector<QPixmap> m_frames;

    qreal m_elapsed = 0.0;
    qreal m_lifeTime = 1.0;
    qreal m_frameInterval = 0.1;

    qreal m_localScale = 1.0;
    qreal m_rotation = 0.0;
    qreal m_opacity = 1.0;

    bool m_alive = true;
    bool m_loop = false;

    int m_currentFrame = 0;

    bool m_infiniteLife = false;
};

#endif // __TYPEGAME_EFFECTITEM_H__
