/* ------------------------------------------------------------------
 // 文件名     : sprite_button.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 精灵按钮控件实现
------------------------------------------------------------------ */

#include "sprite_button.h"

#include <QEnterEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>

SpriteButton::SpriteButton(QWidget* parent)
    : QPushButton(parent)
{
    setCursor(Qt::PointingHandCursor);
    setFlat(true);
    setFocusPolicy(Qt::NoFocus);
    setAutoFillBackground(false);
    setStyleSheet(QStringLiteral(
        "QPushButton { border: none; padding: 0px; background: transparent; outline: none; }"));
}

void SpriteButton::setSprite(const QString& image_path)
{
    m_sprite.load(image_path);
    update();
}

void SpriteButton::setSprite(const QPixmap& pixmap)
{
    m_sprite = pixmap;
    update();
}

void SpriteButton::enterEvent(QEvent* event)
{
    m_state = State::Hover;
    update();
    QPushButton::enterEvent(event);
}

void SpriteButton::leaveEvent(QEvent* event)
{
    m_state = State::Normal;
    update();
    QPushButton::leaveEvent(event);
}

void SpriteButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_state = State::Pressed;
        update();
    }

    QPushButton::mousePressEvent(event);
}

void SpriteButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (rect().contains(event->pos()))
    {
        m_state = State::Hover;
    }
    else
    {
        m_state = State::Normal;
    }

    update();
    QPushButton::mouseReleaseEvent(event);
}

void SpriteButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (m_sprite.isNull())
    {
        return;
    }

    const int frame_width = m_sprite.width() / 3;
    const int frame_height = m_sprite.height();

    int frame_index = 0;
    switch (m_state)
    {
    case State::Normal:
        frame_index = 0;
        break;
    case State::Hover:
        frame_index = 1;
        break;
    case State::Pressed:
        frame_index = 2;
        break;
    }

    const QRect src_rect(frame_index * frame_width, 0, frame_width, frame_height);
    painter.drawPixmap(rect(), m_sprite, src_rect);
}
