#ifndef __TYPEGAME_SPACEWAR_SETTINGS_DIALOG_H__
#define __TYPEGAME_SPACEWAR_SETTINGS_DIALOG_H__

#include <QDialog>
#include <QPixmap>

/* ------------------------------------------------------------------
 // 文件名     : spacewar_settings_dialog.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-26
 // 功能描述   : 飞机大战游戏设置弹窗
------------------------------------------------------------------ */

class QCheckBox;
class QFormLayout;
class QLabel;
class QSlider;
class QPaintEvent;
class SpaceWarModel;
class SpriteButton;

/**
 * 飞机大战设置弹窗。
 *
 * 包含设置项：
 *   1. 最大同屏敌机数量（1-10，使用滑块）
 *   2. 敌机速度等级（1-10，使用滑块）
 *   3. 难度升级间隔（秒，使用滑块）
 *   4. 奖励模式开关
 *   5. 音效开关
 *
 * 边界：
 *   1. 不直接驱动游戏流程；用户确认后将设置写回 SpaceWarModel，由控制器在 update 中应用。
 *   2. 不持有任何长生命周期对象。
 */
class SpaceWarSettingsDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit SpaceWarSettingsDialog(QWidget* parent = nullptr);
    ~SpaceWarSettingsDialog() override = default;

    SpaceWarSettingsDialog(const SpaceWarSettingsDialog&) = delete;
    SpaceWarSettingsDialog& operator=(const SpaceWarSettingsDialog&) = delete;

    void setModel(SpaceWarModel* model);
    SpaceWarModel* model() const;

protected:
    void accept() override;
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUi();
    void loadFromModel();
    void applyToModel();

private:
    SpaceWarModel* m_model = nullptr;

    QFormLayout* m_form_layout = nullptr;
    QWidget* m_panel_container = nullptr;
    QLabel* m_title_label = nullptr;

    QSlider* m_max_enemy_slider = nullptr;
    QLabel* m_max_enemy_value_label = nullptr;
    QSlider* m_enemy_speed_slider = nullptr;
    QLabel* m_enemy_speed_value_label = nullptr;
    QSlider* m_upgrade_interval_slider = nullptr;
    QLabel* m_upgrade_interval_value_label = nullptr;
    QCheckBox* m_reward_mode_check = nullptr;
    QCheckBox* m_sound_enabled_check = nullptr;

    SpriteButton* m_ok_sprite_button = nullptr;
    SpriteButton* m_cancel_sprite_button = nullptr;

    QPixmap m_ok_button_pixmap;
    QPixmap m_cancel_button_pixmap;
};

#endif // __TYPEGAME_SPACEWAR_SETTINGS_DIALOG_H__
