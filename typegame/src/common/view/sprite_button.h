#ifndef __TYPEGAME_SPRITE_BUTTON_H__
#define __TYPEGAME_SPRITE_BUTTON_H__

#include <QPushButton>
#include <QPixmap>

/* ------------------------------------------------------------------
 // 文件名     : sprite_button.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 精灵按钮控件
------------------------------------------------------------------ */

class SpriteButton : public QPushButton
{
    Q_OBJECT

public:
    explicit SpriteButton(QWidget* parent = nullptr);

    void setSprite(const QString& image_path);
    void setSprite(const QPixmap& pixmap);

protected:
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    enum class State
    {
        Normal,
        Hover,
        Pressed
    };

    QPixmap m_sprite;
    State m_state = State::Normal;
};

#endif // __TYPEGAME_SPRITE_BUTTON_H__
