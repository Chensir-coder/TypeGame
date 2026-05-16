#ifndef __TYPEGAME_TWO_ACTION_NOTIFY_DIALOG_H__
#define __TYPEGAME_TWO_ACTION_NOTIFY_DIALOG_H__

#include <QDialog>
#include <QPixmap>

/* ------------------------------------------------------------------
 // 文件名     : two_action_notify_dialog.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : two_action_notify_dialog 头文件声明
------------------------------------------------------------------ */

class QLabel;
class SpriteButton;
class QPaintEvent;

class TwoActionNotifyDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit TwoActionNotifyDialog(const QPixmap& button_one_pixmap,
                                   const QPixmap& button_two_pixmap,
                                   QWidget* parent = nullptr);
    ~TwoActionNotifyDialog() override = default;

    void setMessage(const QString& message);
    void setBackgroundPixmap(const QPixmap& background_pixmap);

signals:
    void actionOneClicked();
    void actionTwoClicked();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUi();

private:
    QLabel* m_message_label = nullptr;
    SpriteButton* m_button_one = nullptr;
    SpriteButton* m_button_two = nullptr;

    QPixmap m_button_one_pixmap;
    QPixmap m_button_two_pixmap;
    QPixmap m_background_pixmap;
};

#endif // __TYPEGAME_TWO_ACTION_NOTIFY_DIALOG_H__
