#ifndef __TYPEGAME_SAVEAPPLE_SETTINGS_DIALOG_H__
#define __TYPEGAME_SAVEAPPLE_SETTINGS_DIALOG_H__

#include <QDialog>
#include <QPixmap>

/* ------------------------------------------------------------------
 // 文件名     : saveapple_settings_dialog.h
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 拯救苹果游戏设置弹窗
------------------------------------------------------------------ */

class QCheckBox;
class QFormLayout;
class QSlider;
class QSpinBox;
class QPushButton;
class QLabel;
class QWidget;
class SaveAppleModel;
class QPaintEvent;
class SpriteButton;

/**
 * @brief 拯救苹果游戏设置弹窗。
 *
 * 职责：
 * 1. 以弹窗形式展示和编辑苹果游戏的核心设置项。
 * 2. 在用户确认后将设置写回 SaveAppleModel。
 *
 * 边界：
 * 1. 不负责游戏流程控制。
 * 2. 不负责业务规则计算。
 * 3. 不直接驱动页面切换。
 */
class SaveAppleSettingsDialog final : public QDialog
{
    Q_OBJECT

public:
    explicit SaveAppleSettingsDialog(QWidget* parent = nullptr);
    ~SaveAppleSettingsDialog() override = default;

    SaveAppleSettingsDialog(const SaveAppleSettingsDialog&) = delete;
    SaveAppleSettingsDialog& operator=(const SaveAppleSettingsDialog&) = delete;

    /**
     * @brief 绑定苹果游戏模型。
     *
     * @param model 模型指针（非拥有型）
     */
    void setModel(SaveAppleModel* model);

    /**
     * @brief 获取当前绑定模型。
     *
     * @return 模型指针
     */
    SaveAppleModel* model() const;

protected:
    void accept() override;
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUi();
    void loadFromModel();
    void applyToModel();

private:
    SaveAppleModel* m_model = nullptr;               ///< 苹果游戏模型（非拥有型）
    QFormLayout* m_form_layout = nullptr;            ///< 表单布局

    QWidget* m_panel_container = nullptr;            ///< 中央面板容器
    QLabel* m_title_label = nullptr;                 ///< 标题
    QLabel* m_difficulty_value_label = nullptr;      ///< 难度值显示

    QSlider* m_speed_level_slider = nullptr;         ///< 困难等级/速度等级滑块
    QSpinBox* m_max_apple_count_spin = nullptr;      ///< 最大苹果数
    QSpinBox* m_target_success_count_spin = nullptr; ///< 目标成功数
    QSpinBox* m_max_miss_count_spin = nullptr;       ///< 最大失败数
    QCheckBox* m_sound_enabled_check = nullptr;      ///< 音效开关

    QPushButton* m_ok_button = nullptr;              ///< 确认按钮
    QPushButton* m_cancel_button = nullptr;          ///< 取消按钮

    QPixmap m_background_pixmap;                     ///< 背景图
    QPixmap m_ok_button_pixmap;                      ///< OK按钮图
    QPixmap m_cancel_button_pixmap;                  ///< NO按钮图

    SpriteButton* m_ok_sprite_button = nullptr;       ///< OK精灵按钮
    SpriteButton* m_cancel_sprite_button = nullptr;   ///< NO精灵按钮

};

#endif // __TYPEGAME_SAVEAPPLE_SETTINGS_DIALOG_H__
