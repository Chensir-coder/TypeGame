/* ------------------------------------------------------------------
 // 文件名     : spacewar_view.cpp
------------------------------------------------------------------ */

#include "spacewar_view.h"
#include "spacewar_playfield_widget.h"

#include <QFocusEvent>
#include <QFont>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QtGlobal>

#include "app/resources/asset_paths.h"
#include "common/core/game_state.h"
#include "spacewar/view/spacewar_fail_dialog.h"
#include "common/view/sprite_button.h"
#include "spacewar/model/entity/reward_word_entity.h"
#include "spacewar/model/spacewar_model.h"
#include "common/service/config/configure_settings.h"

namespace
{
QPixmap spacewarButtonSpriteWithCaption(const QString& base_path, const QString& caption)
{
    QPixmap sprite(base_path);
    if (sprite.isNull() || caption.isEmpty()) {
        return sprite;
    }

    const int frame_width = sprite.width() / 3;
    if (frame_width <= 0) {
        return sprite;
    }

    QPixmap composed(sprite.size());
    composed.fill(Qt::transparent);

    QPainter painter(&composed);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.drawPixmap(0, 0, sprite);

    QFont font;
    font.setFamily(QStringLiteral("Microsoft YaHei"));
    font.setBold(true);
    font.setPixelSize(qMax(16, qRound(sprite.height() * 0.46)));
    painter.setFont(font);

    for (int frame = 0; frame < 3; ++frame) {
        const int x = frame * frame_width;
        /* 原图已带「返回」「选项」等字，直接叠字会重影：先盖住每帧中间区域再画标题 */
        const QRect face_cover(
            x + qRound(frame_width * 0.13),
            1,
            qRound(frame_width * 0.86),
            sprite.height() - 2);
        QLinearGradient face_grad(face_cover.topLeft(), face_cover.bottomLeft());
        face_grad.setColorAt(0.0, QColor(255, 255, 255));
        face_grad.setColorAt(0.52, QColor(238, 240, 242));
        face_grad.setColorAt(1.0, QColor(206, 209, 213));
        painter.setPen(Qt::NoPen);
        painter.setBrush(face_grad);
        painter.drawRoundedRect(face_cover, 5, 5);

        const QRect text_rect = QRect(x, 0, frame_width, sprite.height()).adjusted(
            qRound(frame_width * 0.18), 0, -qRound(frame_width * 0.08), 0);
        painter.setPen(QColor(255, 255, 255, 140));
        painter.drawText(text_rect.translated(1, 1), Qt::AlignCenter, caption);
        painter.setPen(QColor(10, 15, 22));
        painter.drawText(text_rect, Qt::AlignCenter, caption);
    }

    return composed;
}

} // namespace
/* =========================== SpaceWarView =========================== */

SpaceWarView::SpaceWarView(QWidget* parent)
    : QWidget(parent)
    , m_start_pixmap(UiAssetPaths::SpaceWar::startButton())
    , m_pause_pixmap(spacewarButtonSpriteWithCaption(UiAssetPaths::SpaceWar::returnButton(),
                                                     QStringLiteral("暂 停")))
    , m_stop_pixmap(spacewarButtonSpriteWithCaption(UiAssetPaths::SpaceWar::optionButton(),
                                                    QStringLiteral("结 束")))
    , m_settings_pixmap(UiAssetPaths::SpaceWar::optionButton())
    , m_exit_pixmap(UiAssetPaths::SpaceWar::exitButton())
{
    setupUi();
    setupConnections();
    refreshHudFromModel();
    refreshButtonStates();
    setFocusPolicy(Qt::StrongFocus);
}

SpaceWarView::~SpaceWarView() = default;

void SpaceWarView::setModel(SpaceWarModel* model)
{
    m_model = model;

    if (m_playfield != nullptr) {
        m_playfield->setGameModel(m_model);
    }

    refreshHudFromModel();
    refreshButtonStates();
    update();
}

SpaceWarModel* SpaceWarView::model() const
{
    return m_model;
}

void SpaceWarView::tickSceneRefresh(int delta_ms)
{
    if (m_playfield != nullptr) {
        m_playfield->tick(delta_ms);
    }
}

void SpaceWarView::onModelSignalBridgeChanged()
{
    refreshHudFromModel();
    refreshButtonStates();
    tickSceneRefresh(0);
    update();
}

void SpaceWarView::onEnemyHitByBullet(QChar letter)
{
    if (m_playfield != nullptr) {
        m_playfield->onEnemyHitByBullet(letter);
    }
}

void SpaceWarView::onRewardExpired(const QPointF& center,
                                   const QSizeF& size,
                                   const QString& word)
{
    if (m_playfield != nullptr) {
        m_playfield->onRewardExpired(center, size, word);
    }
}

void SpaceWarView::onRewardCompleted(const QPointF& center,
                                     const QSizeF& size,
                                     const QString& word)
{
    if (m_playfield != nullptr) {
        m_playfield->onRewardCompleted(center, size, word);
    }
}

void SpaceWarView::onEnemyCollidedWithPlayer(const QPointF& enemyCenter,
                                             const QSizeF& enemySize,
                                             EnemyPlayerCollisionReason reason)
{
    if (m_playfield != nullptr) {
        m_playfield->onEnemyCollidedWithPlayer(enemyCenter, enemySize, reason);
    }
}

void SpaceWarView::onDifficultyUpgraded(int level, int maxLevel)
{
    if (m_playfield != nullptr) {
        m_playfield->onDifficultyUpgraded(level, maxLevel);
    }
}

void SpaceWarView::onDialogGameOverRequested(int missCount, int maxMissCount, int score)
{
    Q_UNUSED(missCount);
    Q_UNUSED(maxMissCount);

    showFailDialog(score);
}

void SpaceWarView::onDialogLevelCompletedRequested(int score)
{
    /* 飞机大战不存在通关，仅为兼容基类信号 */
    Q_UNUSED(score);
}

void SpaceWarView::keyPressEvent(QKeyEvent* event)
{
    if (event == nullptr) {
        QWidget::keyPressEvent(event);
        return;
    }

    if (event->isAutoRepeat()) {
        event->accept();
        return;
    }

    const int key = event->key();

    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        const QChar letter(static_cast<char>(key));
        emit letterTyped(letter.toUpper());
        event->accept();
        tickSceneRefresh(0);
        return;
    }

    if (key == Qt::Key_Left || key == Qt::Key_Right
        || key == Qt::Key_Up || key == Qt::Key_Down)
    {
        m_pressedKeys.insert(key);
        emitMoveAxesIfChanged();
        event->accept();
        return;
    }

    if (key == Qt::Key_Escape) {
        emit pauseResumeClicked();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void SpaceWarView::keyReleaseEvent(QKeyEvent* event)
{
    if (event == nullptr) {
        QWidget::keyReleaseEvent(event);
        return;
    }

    if (event->isAutoRepeat()) {
        event->accept();
        return;
    }

    const int key = event->key();
    if (key == Qt::Key_Left || key == Qt::Key_Right
        || key == Qt::Key_Up || key == Qt::Key_Down)
    {
        m_pressedKeys.remove(key);
        emitMoveAxesIfChanged();
        event->accept();
        return;
    }

    QWidget::keyReleaseEvent(event);
}

void SpaceWarView::focusOutEvent(QFocusEvent* event)
{
    /* 失去焦点时清空方向键状态，防止持续移动 */
    m_pressedKeys.clear();
    emitMoveAxesIfChanged();
    QWidget::focusOutEvent(event);
}

void SpaceWarView::setupUi()
{
    const auto& ui = ConfigureSettings::instance().spacewar().ui;

    setObjectName(QStringLiteral("spacewar_view"));
    setMinimumSize(1280, 840);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    m_playfield = new SpaceWarPlayfieldWidget(this);
    m_playfield->setGameModel(m_model);

    m_bar_container = new QWidget(this);
    m_bar_container->setObjectName(QStringLiteral("spacewar_control_bar"));
    m_bar_container->setMinimumHeight(qMax(ui.barMinHeight, 118));
    m_bar_container->setStyleSheet(QStringLiteral(
        "QWidget#spacewar_control_bar {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                              stop:0 rgba(5, 12, 30, 244),"
        "                              stop:0.48 rgba(13, 29, 58, 238),"
        "                              stop:1 rgba(3, 7, 18, 248));"
        "  border-top: 1px solid rgba(118, 214, 255, 170);"
        "}"));

    auto* bar_root_layout = new QVBoxLayout(m_bar_container);
    bar_root_layout->setContentsMargins(ui.barHMargin, 9, ui.barHMargin, 9);
    bar_root_layout->setSpacing(8);

    auto* status_layout = new QHBoxLayout();
    status_layout->setContentsMargins(0, 0, 0, 0);
    status_layout->setSpacing(10);

    m_bar_layout = new QHBoxLayout();
    m_bar_layout->setContentsMargins(0, 0, 0, 0);
    m_bar_layout->setSpacing(10);

    m_score_label = new QLabel(QStringLiteral("Score: 0"), m_bar_container);
    m_progress_label = new QLabel(QStringLiteral("Reward: --"), m_bar_container);
    m_state_label = new QLabel(QStringLiteral("State: Initial"), m_bar_container);
    m_difficulty_label = new QLabel(QStringLiteral("Difficulty: 1"), m_bar_container);
    m_hp_label = new QLabel(QStringLiteral("HP: 18 / 18"), m_bar_container);
    m_accuracy_label = new QLabel(QStringLiteral("Input acc: —"), m_bar_container);

    const QString status_label_style = QStringLiteral(
        "QLabel {"
        "  color: #edf8ff;"
        "  font-size: 14px;"
        "  font-weight: 700;"
        "  padding: 6px 12px;"
        "  background: rgba(10, 24, 48, 188);"
        "  border: 1px solid rgba(105, 205, 255, 110);"
        "  border-radius: 10px;"
        "}");

    m_score_label->setStyleSheet(status_label_style);
    m_progress_label->setStyleSheet(status_label_style);
    m_state_label->setStyleSheet(status_label_style);
    m_difficulty_label->setStyleSheet(status_label_style);
    m_hp_label->setStyleSheet(status_label_style);
    m_accuracy_label->setStyleSheet(status_label_style);

    status_layout->addWidget(m_score_label, 0);
    status_layout->addWidget(m_hp_label, 0);
    status_layout->addWidget(m_accuracy_label, 0);
    status_layout->addWidget(m_progress_label, 1);
    status_layout->addWidget(m_state_label, 0);
    status_layout->addWidget(m_difficulty_label, 0);

    m_start_button = new SpriteButton(m_bar_container);
    m_pause_resume_button = new SpriteButton(m_bar_container);
    m_stop_button = new SpriteButton(m_bar_container);
    m_settings_button = new SpriteButton(m_bar_container);
    m_exit_button = new SpriteButton(m_bar_container);

    const QSize commandButtonSize(156, 36);
    m_start_button->setFixedSize(commandButtonSize);
    m_pause_resume_button->setFixedSize(commandButtonSize);
    m_stop_button->setFixedSize(commandButtonSize);
    m_settings_button->setFixedSize(commandButtonSize);
    m_exit_button->setFixedSize(commandButtonSize);

    m_start_button->setSprite(m_start_pixmap);
    m_pause_resume_button->setSprite(m_pause_pixmap);
    m_stop_button->setSprite(m_stop_pixmap);
    m_settings_button->setSprite(m_settings_pixmap);
    m_exit_button->setSprite(m_exit_pixmap);

    auto* command_title = new QLabel(QStringLiteral("SPACEWAR COMMAND DECK"), m_bar_container);
    command_title->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: rgba(175, 228, 255, 220);"
        "  font-size: 13px;"
        "  font-weight: 800;"
        "  letter-spacing: 1px;"
        "  background: transparent;"
        "}"));

    m_bar_layout->addWidget(command_title, 0, Qt::AlignVCenter);
    m_bar_layout->addStretch(1);
    m_bar_layout->addWidget(m_start_button, 0, Qt::AlignVCenter);
    m_bar_layout->addWidget(m_pause_resume_button, 0, Qt::AlignVCenter);
    m_bar_layout->addWidget(m_stop_button, 0, Qt::AlignVCenter);
    m_bar_layout->addWidget(m_settings_button, 0, Qt::AlignVCenter);
    m_bar_layout->addWidget(m_exit_button, 0, Qt::AlignVCenter);

    bar_root_layout->addLayout(status_layout);
    bar_root_layout->addLayout(m_bar_layout);

    root->addWidget(m_playfield, 1);
    root->addWidget(m_bar_container, 0);
}

void SpaceWarView::setupConnections()
{
    connect(m_start_button, &QPushButton::clicked, this, &SpaceWarView::startClicked);
    connect(m_pause_resume_button, &QPushButton::clicked, this, &SpaceWarView::pauseResumeClicked);
    connect(m_stop_button, &QPushButton::clicked, this, &SpaceWarView::stopClicked);
    connect(m_settings_button, &QPushButton::clicked, this, &SpaceWarView::settingsClicked);
    connect(m_exit_button, &QPushButton::clicked, this, &SpaceWarView::exitClicked);
}

void SpaceWarView::emitMoveAxesIfChanged()
{
    int h = 0;
    if (m_pressedKeys.contains(Qt::Key_Left))  { h -= 1; }
    if (m_pressedKeys.contains(Qt::Key_Right)) { h += 1; }

    int v = 0;
    if (m_pressedKeys.contains(Qt::Key_Up))   { v -= 1; }
    if (m_pressedKeys.contains(Qt::Key_Down)) { v += 1; }

    if (h == m_lastEmittedHAxis && v == m_lastEmittedVAxis) {
        return;
    }
    m_lastEmittedHAxis = h;
    m_lastEmittedVAxis = v;
    emit moveKeyChanged(h, v);
}

void SpaceWarView::refreshHudFromModel()
{
    if (m_score_label == nullptr || m_progress_label == nullptr
        || m_state_label == nullptr || m_difficulty_label == nullptr
        || m_hp_label == nullptr || m_accuracy_label == nullptr)
    {
        return;
    }

    if (m_model == nullptr) {
        m_score_label->setText(QStringLiteral("Score: 0"));
        m_progress_label->setText(QStringLiteral("Reward: --"));
        m_state_label->setText(QStringLiteral("State: Initial"));
        m_difficulty_label->setText(QStringLiteral("Difficulty: 1"));
        m_hp_label->setText(QStringLiteral("HP: 18 / 18"));
        m_accuracy_label->setText(QStringLiteral("Input acc: —"));
        return;
    }

    m_score_label->setText(QStringLiteral("Score: %1").arg(m_model->score()));

    if (RewardWordEntity* reward = m_model->activeReward()) {
        const QString w = reward->word().toLower();
        const QString typed = reward->typedPart().toLower();
        m_progress_label->setText(
            QStringLiteral("Reward: %1 (%2/%3)")
                .arg(w)
                .arg(typed.size())
                .arg(w.size()));
    } else {
        m_progress_label->setText(QStringLiteral("Reward: --"));
    }

    m_state_label->setText(QStringLiteral("State: %1").arg(gameStateText()));
    m_difficulty_label->setText(QStringLiteral("Difficulty: %1").arg(m_model->difficultyLevel()));
    m_hp_label->setText(
        QStringLiteral("HP: %1 / %2")
            .arg(m_model->playerHealth())
            .arg(m_model->playerMaxHealth()));

    const auto& st = m_model->stats();
    if (st.typingKeystrokeTotal() <= 0) {
        m_accuracy_label->setText(QStringLiteral("Input acc: —"));
    } else {
        m_accuracy_label->setText(
            QStringLiteral("Input acc: %1%")
                .arg(st.typingInputAccuracyPercent()));
    }
}

void SpaceWarView::refreshButtonStates()
{
    if (m_start_button == nullptr || m_pause_resume_button == nullptr
        || m_stop_button == nullptr || m_settings_button == nullptr
        || m_exit_button == nullptr)
    {
        return;
    }

    m_settings_button->setEnabled(true);
    m_exit_button->setEnabled(true);

    if (m_model == nullptr) {
        m_start_button->setEnabled(true);
        m_pause_resume_button->setEnabled(false);
        m_stop_button->setEnabled(false);
        return;
    }

    switch (m_model->gameState()) {
    case GameState::Initial:
        m_start_button->setEnabled(true);
        m_pause_resume_button->setEnabled(false);
        m_stop_button->setEnabled(false);
        break;
    case GameState::Playing:
        m_start_button->setEnabled(false);
        m_pause_resume_button->setEnabled(true);
        m_stop_button->setEnabled(true);
        break;
    case GameState::Paused:
        m_start_button->setEnabled(false);
        m_pause_resume_button->setEnabled(true);
        m_stop_button->setEnabled(true);
        break;
    case GameState::End:
        m_start_button->setEnabled(true);
        m_pause_resume_button->setEnabled(false);
        m_stop_button->setEnabled(false);
        break;
    default:
        m_start_button->setEnabled(false);
        m_pause_resume_button->setEnabled(false);
        m_stop_button->setEnabled(false);
        break;
    }
}

QString SpaceWarView::gameStateText() const
{
    if (m_model == nullptr) {
        return QStringLiteral("Initial");
    }

    switch (m_model->gameState()) {
    case GameState::Initial: return QStringLiteral("Initial");
    case GameState::Playing: return QStringLiteral("Playing");
    case GameState::Paused:  return QStringLiteral("Paused");
    case GameState::End:     return QStringLiteral("End");
    default:                 return QStringLiteral("Unknown");
    }
}

void SpaceWarView::showFailDialog(int score)
{
    if (m_fail_dialog != nullptr) {
        m_fail_dialog->setMessage(
            QStringLiteral("游戏结束！得分：%1\n是否再来一局？").arg(score));
        m_fail_dialog->raise();
        m_fail_dialog->activateWindow();
        return;
    }

    const QPixmap end_btn(UiAssetPaths::Common::mainDialogExit());
    const QPixmap retry_btn(UiAssetPaths::Common::mainDialogReplay());

    auto* dialog = new SpaceWarFailDialog(end_btn, retry_btn, this);
    dialog->setAttribute(Qt::WA_DeleteOnClose, true);
    m_fail_dialog = dialog;

    dialog->setMessage(
        QStringLiteral("游戏结束！得分：%1\n是否再来一局？").arg(score));

    connect(dialog, &SpaceWarFailDialog::actionOneClicked, this, [this]() {
        emit stopClicked();
    });
    connect(dialog, &SpaceWarFailDialog::actionTwoClicked, this, [this]() {
        emit retryClicked();
    });
    connect(dialog, &QObject::destroyed, this, [this]() {
        m_fail_dialog = nullptr;
    });

    dialog->open();
    dialog->raise();
    dialog->activateWindow();
}
