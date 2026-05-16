/* ------------------------------------------------------------------
 // 文件名     : spacewar_settings_dialog.cpp
------------------------------------------------------------------ */

#include "spacewar_settings_dialog.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPaintEvent>
#include <QPushButton>
#include <QSlider>
#include <QString>
#include <QVBoxLayout>

#include "app/resources/asset_paths.h"
#include "common/view/sprite_button.h"
#include "spacewar/model/spacewar_model.h"

namespace
{
constexpr int kDialogWidth = 640;
constexpr int kDialogHeight = 460;

constexpr int kButtonWidth = 120;
constexpr int kButtonHeight = 52;
}

SpaceWarSettingsDialog::SpaceWarSettingsDialog(QWidget* parent)
    : QDialog(parent)
    , m_ok_button_pixmap(UiAssetPaths::Common::okButton())
    , m_cancel_button_pixmap(UiAssetPaths::Common::cancelButton())
{
    setupUi();
}

void SpaceWarSettingsDialog::setModel(SpaceWarModel* model)
{
    m_model = model;
    loadFromModel();
}

SpaceWarModel* SpaceWarSettingsDialog::model() const
{
    return m_model;
}

void SpaceWarSettingsDialog::accept()
{
    applyToModel();
    QDialog::accept();
}

void SpaceWarSettingsDialog::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QLinearGradient grad(0, 0, 0, height());
    grad.setColorAt(0.0, QColor(8, 14, 36));
    grad.setColorAt(1.0, QColor(20, 32, 80));
    painter.fillRect(rect(), grad);
}

void SpaceWarSettingsDialog::setupUi()
{
    setObjectName(QStringLiteral("spacewar_settings_dialog"));
    setModal(true);
    resize(kDialogWidth, kDialogHeight);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);

    auto* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(36, 28, 36, 24);
    main_layout->setSpacing(18);

    m_title_label = new QLabel(QStringLiteral("SPACE WAR SETTINGS"), this);
    m_title_label->setAlignment(Qt::AlignCenter);
    m_title_label->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #DDF1FF;"
        "  font-size: 28px;"
        "  font-weight: 800;"
        "  letter-spacing: 2px;"
        "  background: transparent;"
        "}"));

    m_panel_container = new QWidget(this);
    m_panel_container->setStyleSheet(QStringLiteral(
        "QWidget {"
        "  background: rgba(8, 18, 44, 200);"
        "  border: 1px solid rgba(160, 200, 255, 130);"
        "  border-radius: 18px;"
        "}"
        "QLabel {"
        "  color: #E2F0FF;"
        "  font-size: 16px;"
        "  font-weight: 700;"
        "  background: transparent;"
        "}"
        "QCheckBox {"
        "  color: #E2F0FF;"
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
        "  background: rgba(225, 240, 255, 220);"
        "  border: 2px solid rgba(120, 180, 255, 200);"
        "  border-radius: 6px;"
        "}"
        "QCheckBox::indicator:checked {"
        "  background: #4FB6FF;"
        "  border: 2px solid #1F88E0;"
        "  border-radius: 6px;"
        "}"
        "QSlider::groove:horizontal {"
        "  height: 10px;"
        "  background: rgba(40, 60, 110, 220);"
        "  border-radius: 5px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: #4FB6FF;"
        "  border-radius: 5px;"
        "}"
        "QSlider::handle:horizontal {"
        "  width: 24px;"
        "  margin: -8px 0;"
        "  background: #FFFFFF;"
        "  border: 2px solid #1F88E0;"
        "  border-radius: 12px;"
        "}"));

    auto* panel_layout = new QVBoxLayout(m_panel_container);
    panel_layout->setContentsMargins(26, 22, 26, 20);
    panel_layout->setSpacing(16);

    m_form_layout = new QFormLayout();
    m_form_layout->setLabelAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    m_form_layout->setFormAlignment(Qt::AlignTop);
    m_form_layout->setHorizontalSpacing(18);
    m_form_layout->setVerticalSpacing(16);

    m_max_enemy_slider = new QSlider(Qt::Horizontal, m_panel_container);
    m_max_enemy_slider->setRange(1, 10);
    m_max_enemy_slider->setValue(4);
    m_max_enemy_slider->setPageStep(1);
    m_max_enemy_slider->setTickPosition(QSlider::TicksBelow);
    m_max_enemy_slider->setTickInterval(1);

    m_max_enemy_value_label = new QLabel(QStringLiteral("4"), m_panel_container);
    m_max_enemy_value_label->setAlignment(Qt::AlignCenter);
    m_max_enemy_value_label->setFixedWidth(40);
    m_max_enemy_value_label->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #E2F0FF;"
        "  font-size: 18px;"
        "  font-weight: 800;"
        "  background: rgba(20, 40, 90, 220);"
        "  border: 1px solid rgba(160, 210, 255, 140);"
        "  border-radius: 10px;"
        "  padding: 6px 4px;"
        "}"));

    auto* max_enemy_widget = new QWidget(m_panel_container);
    auto* max_enemy_layout = new QHBoxLayout(max_enemy_widget);
    max_enemy_layout->setContentsMargins(0, 0, 0, 0);
    max_enemy_layout->setSpacing(12);
    max_enemy_layout->addWidget(m_max_enemy_slider, 1);
    max_enemy_layout->addWidget(m_max_enemy_value_label);

    m_enemy_speed_slider = new QSlider(Qt::Horizontal, m_panel_container);
    m_enemy_speed_slider->setRange(1, 10);
    m_enemy_speed_slider->setValue(5);
    m_enemy_speed_slider->setPageStep(1);
    m_enemy_speed_slider->setTickPosition(QSlider::TicksBelow);
    m_enemy_speed_slider->setTickInterval(1);

    m_enemy_speed_value_label = new QLabel(QStringLiteral("5"), m_panel_container);
    m_enemy_speed_value_label->setAlignment(Qt::AlignCenter);
    m_enemy_speed_value_label->setFixedWidth(40);
    m_enemy_speed_value_label->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #E2F0FF;"
        "  font-size: 18px;"
        "  font-weight: 800;"
        "  background: rgba(20, 40, 90, 220);"
        "  border: 1px solid rgba(160, 210, 255, 140);"
        "  border-radius: 10px;"
        "  padding: 6px 4px;"
        "}"));

    auto* speed_widget = new QWidget(m_panel_container);
    auto* speed_layout = new QHBoxLayout(speed_widget);
    speed_layout->setContentsMargins(0, 0, 0, 0);
    speed_layout->setSpacing(12);
    speed_layout->addWidget(m_enemy_speed_slider, 1);
    speed_layout->addWidget(m_enemy_speed_value_label);

    m_upgrade_interval_slider = new QSlider(Qt::Horizontal, m_panel_container);
    m_upgrade_interval_slider->setRange(1, 600);
    m_upgrade_interval_slider->setValue(20);
    m_upgrade_interval_slider->setSingleStep(1);
    m_upgrade_interval_slider->setPageStep(10);
    m_upgrade_interval_slider->setTickPosition(QSlider::TicksBelow);
    m_upgrade_interval_slider->setTickInterval(60);

    m_upgrade_interval_value_label = new QLabel(QStringLiteral("20 s"), m_panel_container);
    m_upgrade_interval_value_label->setAlignment(Qt::AlignCenter);
    m_upgrade_interval_value_label->setFixedWidth(64);
    m_upgrade_interval_value_label->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #E2F0FF;"
        "  font-size: 16px;"
        "  font-weight: 800;"
        "  background: rgba(20, 40, 90, 220);"
        "  border: 1px solid rgba(160, 210, 255, 140);"
        "  border-radius: 10px;"
        "  padding: 6px 4px;"
        "}"));

    auto* interval_widget = new QWidget(m_panel_container);
    auto* interval_layout = new QHBoxLayout(interval_widget);
    interval_layout->setContentsMargins(0, 0, 0, 0);
    interval_layout->setSpacing(12);
    interval_layout->addWidget(m_upgrade_interval_slider, 1);
    interval_layout->addWidget(m_upgrade_interval_value_label);

    m_reward_mode_check = new QCheckBox(QStringLiteral("Enable Bonus Words"), m_panel_container);
    m_reward_mode_check->setChecked(true);

    m_sound_enabled_check = new QCheckBox(QStringLiteral("Enable Sound"), m_panel_container);
    m_sound_enabled_check->setChecked(true);

    m_form_layout->addRow(new QLabel(QStringLiteral("Max Enemies"), m_panel_container),
                          max_enemy_widget);
    m_form_layout->addRow(new QLabel(QStringLiteral("Enemy Speed"), m_panel_container),
                          speed_widget);
    m_form_layout->addRow(new QLabel(QStringLiteral("Upgrade Interval"), m_panel_container),
                          interval_widget);
    m_form_layout->addRow(new QLabel(QStringLiteral("Reward Mode"), m_panel_container),
                          m_reward_mode_check);
    m_form_layout->addRow(new QLabel(QStringLiteral("Sound"), m_panel_container),
                          m_sound_enabled_check);

    panel_layout->addLayout(m_form_layout);

    auto* button_container = new QWidget(this);
    button_container->setStyleSheet(QStringLiteral("background: transparent;"));

    auto* button_layout = new QHBoxLayout(button_container);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(24);
    button_layout->addStretch();

    m_ok_sprite_button = new SpriteButton(button_container);
    m_cancel_sprite_button = new SpriteButton(button_container);

    const QString button_style = QStringLiteral(
        "QPushButton { background: transparent; border: none; }"
        "QPushButton:hover { background: rgba(255,255,255,18); border-radius: 12px; }"
        "QPushButton:pressed { background: rgba(0,0,0,28); border-radius: 12px; }");

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

    connect(m_max_enemy_slider, &QSlider::valueChanged, this, [this](int value) {
        if (m_max_enemy_value_label != nullptr) {
            m_max_enemy_value_label->setText(QString::number(value));
        }
    });

    connect(m_enemy_speed_slider, &QSlider::valueChanged, this, [this](int value) {
        if (m_enemy_speed_value_label != nullptr) {
            m_enemy_speed_value_label->setText(QString::number(value));
        }
    });

    connect(m_upgrade_interval_slider, &QSlider::valueChanged, this, [this](int value) {
        if (m_upgrade_interval_value_label != nullptr) {
            m_upgrade_interval_value_label->setText(
                QStringLiteral("%1 s").arg(value));
        }
    });

    connect(m_ok_sprite_button, &SpriteButton::clicked, this, &SpaceWarSettingsDialog::accept);
    connect(m_cancel_sprite_button, &SpriteButton::clicked, this, &SpaceWarSettingsDialog::reject);
}

void SpaceWarSettingsDialog::loadFromModel()
{
    if (m_model == nullptr) {
        return;
    }

    m_max_enemy_slider->setValue(m_model->maxEnemyCount());
    m_enemy_speed_slider->setValue(m_model->enemySpeedLevel());
    const int intervalSec = qBound(1, m_model->difficultyUpgradeIntervalSec(), 600);
    m_upgrade_interval_slider->setValue(intervalSec);
    m_reward_mode_check->setChecked(m_model->rewardModeEnabled());
    m_sound_enabled_check->setChecked(m_model->spaceWarSoundEnabled());

    if (m_max_enemy_value_label != nullptr) {
        m_max_enemy_value_label->setText(QString::number(m_max_enemy_slider->value()));
    }
    if (m_enemy_speed_value_label != nullptr) {
        m_enemy_speed_value_label->setText(QString::number(m_enemy_speed_slider->value()));
    }
    if (m_upgrade_interval_value_label != nullptr) {
        m_upgrade_interval_value_label->setText(
            QStringLiteral("%1 s").arg(m_upgrade_interval_slider->value()));
    }
}

void SpaceWarSettingsDialog::applyToModel()
{
    if (m_model == nullptr) {
        return;
    }

    m_model->setMaxEnemyCount(m_max_enemy_slider->value());
    m_model->setEnemySpeedLevel(m_enemy_speed_slider->value());
    m_model->setDifficultyUpgradeIntervalSec(m_upgrade_interval_slider->value());
    m_model->setRewardModeEnabled(m_reward_mode_check->isChecked());
    m_model->setSpaceWarSoundEnabled(m_sound_enabled_check->isChecked());
}
