/* ------------------------------------------------------------------
 // 文件名     : saveapple_settings_dialog.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 拯救苹果游戏设置弹窗实现
------------------------------------------------------------------ */

#include "saveapple_settings_dialog.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QString>
#include <QFileInfo>

#include "saveapple/model/saveapple_model.h"
#include "common/view/sprite_button.h"
#include "app/resources/asset_paths.h"

class SpriteButton;

namespace
{
constexpr int kDialogWidth = 620;
constexpr int kDialogHeight = 420;

constexpr int kButtonWidth = 120;
constexpr int kButtonHeight = 52;
}

SaveAppleSettingsDialog::SaveAppleSettingsDialog(QWidget* parent)
    : QDialog(parent)
    , m_background_pixmap(UiAssetPaths::Apple::background())
    , m_ok_button_pixmap(UiAssetPaths::Common::okButton())
    , m_cancel_button_pixmap(UiAssetPaths::Common::cancelButton())
{
    setupUi();
}

void SaveAppleSettingsDialog::setModel(SaveAppleModel* model)
{
    m_model = model;
    loadFromModel();
}

SaveAppleModel* SaveAppleSettingsDialog::model() const
{
    return m_model;
}

void SaveAppleSettingsDialog::accept()
{
    applyToModel();
    QDialog::accept();
}

void SaveAppleSettingsDialog::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (!m_background_pixmap.isNull())
    {
        // painter.drawPixmap(rect(), m_background_pixmap);
        painter.fillRect(rect(), QColor(255, 244, 228));
    }
    else
    {
        painter.fillRect(rect(), QColor(255, 244, 228));
    }

    QDialog::paintEvent(event);
}

void SaveAppleSettingsDialog::setupUi()
{
    setObjectName(QStringLiteral("saveapple_settings_dialog"));
    setModal(true);
    resize(kDialogWidth, kDialogHeight);

    // 去掉顶部原生标题栏（最小化/关闭那一栏）
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);

    QVBoxLayout* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(36, 28, 36, 24);
    main_layout->setSpacing(18);

    m_title_label = new QLabel(QStringLiteral("APPLE SETTINGS"), this);
    m_title_label->setAlignment(Qt::AlignCenter);
    m_title_label->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #FFF7E8;"
        "  font-size: 28px;"
        "  font-weight: 800;"
        "  letter-spacing: 2px;"
        "  background: transparent;"
        "}"
    ));

    m_panel_container = new QWidget(this);
    m_panel_container->setStyleSheet(QStringLiteral(
        "QWidget {"
        "  background: rgba(32, 20, 12, 145);"
        "  border: 1px solid rgba(255, 245, 220, 110);"
        "  border-radius: 20px;"
        "}"
        "QLabel {"
        "  color: #FFF6E7;"
        "  font-size: 16px;"
        "  font-weight: 700;"
        "  background: transparent;"
        "}"
        "QSpinBox {"
        "  min-height: 36px;"
        "  padding: 4px 12px;"
        "  color: #2E1C10;"
        "  background: rgba(255, 248, 236, 235);"
        "  border: 2px solid rgba(180, 120, 60, 180);"
        "  border-radius: 10px;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "}"
        "QSpinBox::up-button, QSpinBox::down-button {"
        "  width: 22px;"
        "  border: none;"
        "  background: transparent;"
        "}"
        "QCheckBox {"
        "  color: #FFF6E7;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "  spacing: 8px;"
        "  background: transparent;"
        "}"
        "QCheckBox::indicator {"
        "  width: 22px;"
        "  height: 22px;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "  background: rgba(255, 248, 236, 235);"
        "  border: 2px solid rgba(180, 120, 60, 180);"
        "  border-radius: 6px;"
        "}"
        "QCheckBox::indicator:checked {"
        "  background: #FFB347;"
        "  border: 2px solid #E58B1F;"
        "  border-radius: 6px;"
        "}"
        "QSlider::groove:horizontal {"
        "  height: 10px;"
        "  background: rgba(255, 241, 214, 180);"
        "  border-radius: 5px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: #FFB347;"
        "  border-radius: 5px;"
        "}"
        "QSlider::handle:horizontal {"
        "  width: 24px;"
        "  margin: -8px 0;"
        "  background: #FFF7E8;"
        "  border: 2px solid #D97706;"
        "  border-radius: 12px;"
        "}"
    ));

    QVBoxLayout* panel_layout = new QVBoxLayout(m_panel_container);
    panel_layout->setContentsMargins(26, 22, 26, 20);
    panel_layout->setSpacing(16);

    m_form_layout = new QFormLayout();
    m_form_layout->setLabelAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_form_layout->setFormAlignment(Qt::AlignTop);
    m_form_layout->setHorizontalSpacing(18);
    m_form_layout->setVerticalSpacing(16);

    m_speed_level_slider = new QSlider(Qt::Horizontal, m_panel_container);
    m_speed_level_slider->setRange(1, 10);
    m_speed_level_slider->setValue(1);
    m_speed_level_slider->setPageStep(1);
    m_speed_level_slider->setTickPosition(QSlider::TicksBelow);
    m_speed_level_slider->setTickInterval(1);

    m_difficulty_value_label = new QLabel(QStringLiteral("1"), m_panel_container);
    m_difficulty_value_label->setAlignment(Qt::AlignCenter);
    m_difficulty_value_label->setFixedWidth(40);
    m_difficulty_value_label->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #FFE9BF;"
        "  font-size: 18px;"
        "  font-weight: 800;"
        "  background: rgba(90, 50, 20, 160);"
        "  border: 1px solid rgba(255, 230, 180, 120);"
        "  border-radius: 10px;"
        "  padding: 6px 4px;"
        "}"
    ));

    QWidget* difficulty_widget = new QWidget(m_panel_container);
    QHBoxLayout* difficulty_layout = new QHBoxLayout(difficulty_widget);
    difficulty_layout->setContentsMargins(0, 0, 0, 0);
    difficulty_layout->setSpacing(12);
    difficulty_layout->addWidget(m_speed_level_slider, 1);
    difficulty_layout->addWidget(m_difficulty_value_label);

    m_max_apple_count_spin = new QSpinBox(m_panel_container);
    m_max_apple_count_spin->setRange(1, 5);
    m_max_apple_count_spin->setValue(3);

    m_target_success_count_spin = new QSpinBox(m_panel_container);
    m_target_success_count_spin->setRange(1, 999);
    m_target_success_count_spin->setValue(10);

    m_max_miss_count_spin = new QSpinBox(m_panel_container);
    m_max_miss_count_spin->setRange(1, 999);
    m_max_miss_count_spin->setValue(5);

    m_sound_enabled_check = new QCheckBox(QStringLiteral("Enable Sound"), m_panel_container);
    m_sound_enabled_check->setChecked(true);

    QLabel* difficulty_label = new QLabel(QStringLiteral("Difficulty"), m_panel_container);
    QLabel* max_apple_label = new QLabel(QStringLiteral("Max Apple Count"), m_panel_container);
    QLabel* target_success_label = new QLabel(QStringLiteral("Target Success Count"), m_panel_container);
    QLabel* max_miss_label = new QLabel(QStringLiteral("Max Miss Count"), m_panel_container);
    QLabel* sound_label = new QLabel(QStringLiteral("Sound"), m_panel_container);

    m_form_layout->addRow(difficulty_label, difficulty_widget);
    m_form_layout->addRow(max_apple_label, m_max_apple_count_spin);
    m_form_layout->addRow(target_success_label, m_target_success_count_spin);
    m_form_layout->addRow(max_miss_label, m_max_miss_count_spin);
    m_form_layout->addRow(sound_label, m_sound_enabled_check);

    panel_layout->addLayout(m_form_layout);

    QWidget* button_container = new QWidget(this);
    button_container->setStyleSheet(QStringLiteral("background: transparent;"));

    QHBoxLayout* button_layout = new QHBoxLayout(button_container);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(24);
    button_layout->addStretch();

    m_ok_sprite_button = new SpriteButton(button_container);
    m_cancel_sprite_button = new SpriteButton(button_container);


    const QString button_style = QStringLiteral(
        "QPushButton {"
        "  background: transparent;"
        "  border: none;"
        "}"
        "QPushButton:hover {"
        "  background: rgba(255, 255, 255, 18);"
        "  border-radius: 12px;"
        "}"
        "QPushButton:pressed {"
        "  background: rgba(0, 0, 0, 28);"
        "  border-radius: 12px;"
        "}"
    );

    m_ok_sprite_button->setFixedSize(kButtonWidth, kButtonHeight);
    m_cancel_sprite_button->setFixedSize(kButtonWidth, kButtonHeight);
    m_ok_sprite_button->setCursor(Qt::PointingHandCursor);
    m_cancel_sprite_button->setCursor(Qt::PointingHandCursor);
    m_ok_sprite_button->setStyleSheet(button_style);
    m_cancel_sprite_button->setStyleSheet(button_style);

    m_ok_sprite_button->setSprite(m_ok_button_pixmap);
    m_cancel_sprite_button->setSprite(m_cancel_button_pixmap);


    button_layout->addWidget(m_ok_sprite_button);
    button_layout->addWidget(m_cancel_sprite_button);
    button_layout->addStretch();

    main_layout->addWidget(m_title_label);
    main_layout->addWidget(m_panel_container, 1);
    main_layout->addWidget(button_container);

    connect(m_speed_level_slider, &QSlider::valueChanged, this, [this](int value) {
        if (m_difficulty_value_label != nullptr)
        {
            m_difficulty_value_label->setText(QString::number(value));
        }
    });

    connect(m_ok_sprite_button, &SpriteButton::clicked, this, &SaveAppleSettingsDialog::accept);
    connect(m_cancel_sprite_button, &SpriteButton::clicked, this, &SaveAppleSettingsDialog::reject);
}

void SaveAppleSettingsDialog::loadFromModel()
{
    if (m_model == nullptr)
    {
        return;
    }

    m_speed_level_slider->setValue(m_model->speedLevel());
    m_max_apple_count_spin->setValue(m_model->maxAppleCount());
    m_target_success_count_spin->setValue(m_model->targetSuccessCount());
    m_max_miss_count_spin->setValue(m_model->maxMissCount());
    m_sound_enabled_check->setChecked(m_model->soundEnabled());

    if (m_difficulty_value_label != nullptr)
    {
        m_difficulty_value_label->setText(QString::number(m_speed_level_slider->value()));
    }
}

void SaveAppleSettingsDialog::applyToModel()
{
    if (m_model == nullptr)
    {
        return;
    }

    m_model->setSpeedLevel(m_speed_level_slider->value());
    m_model->setMaxAppleCount(m_max_apple_count_spin->value());
    m_model->setTargetSuccessCount(m_target_success_count_spin->value());
    m_model->setMaxMissCount(m_max_miss_count_spin->value());
    m_model->setSoundEnabled(m_sound_enabled_check->isChecked());
}
