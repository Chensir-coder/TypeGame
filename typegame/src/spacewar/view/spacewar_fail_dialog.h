#ifndef __TYPEGAME_SPACEWAR_FAIL_DIALOG_H__
#define __TYPEGAME_SPACEWAR_FAIL_DIALOG_H__

#include <QDialog>
#include <QPixmap>

/* ------------------------------------------------------------------
 // 文件名     : spacewar_fail_dialog.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : spacewar_fail_dialog 头文件声明
------------------------------------------------------------------ */

class QLabel;
class SpriteButton;

/**
 * 飞机大战专用失败提示框：背景由 paintEvent 自绘，不依赖通用对话框资源图。
 * 仍使用 Common 的退出 / 再来一局精灵条作为两个 SpriteButton。
 */
class SpaceWarFailDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit SpaceWarFailDialog(const QPixmap& exit_button_pixmap,
                                const QPixmap& retry_button_pixmap,
                                QWidget* parent = nullptr);
    ~SpaceWarFailDialog() override = default;

    void setMessage(const QString& message);

signals:
    void actionOneClicked();
    void actionTwoClicked();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUi();

    QLabel* m_message_label = nullptr;
    SpriteButton* m_button_one = nullptr;
    SpriteButton* m_button_two = nullptr;

    QPixmap m_button_one_pixmap;
    QPixmap m_button_two_pixmap;
};

#endif // __TYPEGAME_SPACEWAR_FAIL_DIALOG_H__
