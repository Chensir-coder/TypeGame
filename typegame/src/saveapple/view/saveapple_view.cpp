/* ------------------------------------------------------------------
 // 文件名     : saveapple_view.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 拯救苹果游戏主页面实现
------------------------------------------------------------------ */

#include "saveapple_view.h"

#include "common/core/game_event.h"
#include "common/core/game_state.h"
#include "app/resources/asset_paths.h"
#include "saveapple/controller/saveapple_controller.h"
#include "saveapple/model/apple_model.h"
#include "saveapple/model/saveapple_model.h"
#include "common/view/dialog/two_action_notify_dialog.h"


#include <QFont>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QPushButton>
#include <QResizeEvent>
#include <QStyle>
#include <QDebug>



namespace
{
constexpr int kStatusLeft = 20;
constexpr int kStatusTop = 18;
constexpr int kStatusHeight = 56;

constexpr int kSceneTopMargin = 0;
constexpr int kSceneLeftMargin = 0;
constexpr int kSceneRightMargin = 0;
constexpr int kSceneBottomMargin = 0;

// 按钮位置设置
struct ButtonLayoutRatio
{
    qreal x_ratio;
    qreal y_ratio;
    qreal width_ratio;
    qreal height_ratio;
};

constexpr ButtonLayoutRatio kStartButtonLayout    { 0.2, 0.79, 0.06, 0.08 };
constexpr ButtonLayoutRatio kPauseButtonLayout    { 0.245, 0.87, 0.07, 0.09 };
constexpr ButtonLayoutRatio kStopButtonLayout     { 0.185, 0.91, 0.06, 0.08 };
constexpr ButtonLayoutRatio kSettingsButtonLayout { 0.14, 0.845, 0.06, 0.085 };
constexpr ButtonLayoutRatio kExitButtonLayout     { 0.02, 0.85, 0.1, 0.1 };

constexpr int kButtonMinWidth = 10;
constexpr int kButtonMinHeight = 10;
constexpr int kButtonMaxWidth = 300;
constexpr int kButtonMaxHeight = 300;

QRect geometryFromRatio(const QSize& view_size, const ButtonLayoutRatio& ratio)
{
    const int button_width = qBound(
        kButtonMinWidth,
        static_cast<int>(view_size.width() * ratio.width_ratio),
        kButtonMaxWidth);

    const int button_height = qBound(
        kButtonMinHeight,
        static_cast<int>(view_size.height() * ratio.height_ratio),
        kButtonMaxHeight);

    const int x = static_cast<int>(view_size.width() * ratio.x_ratio);
    const int y = static_cast<int>(view_size.height() * ratio.y_ratio);

    return QRect(x, y, button_width, button_height);
}

// 篮子和成功小苹果区域布局
struct RectLayoutRatio
{
    qreal x_ratio;
    qreal y_ratio;
    qreal width_ratio;
    qreal height_ratio;
};


// 原始设计稿比例
constexpr RectLayoutRatio kBasketLayout      { 0.7, 0.79, 0.25, 0.2 };
constexpr RectLayoutRatio kSavedApplesLayout { 0.7, 0.7, 0.2, 0.3 };


QRect rectFromRatio(const QSize& view_size, const RectLayoutRatio& ratio)
{
    const int w = static_cast<int>(view_size.width() * ratio.width_ratio);
    const int h = static_cast<int>(view_size.height() * ratio.height_ratio);
    const int x = static_cast<int>(view_size.width() * ratio.x_ratio);
    const int y = static_cast<int>(view_size.height() * ratio.y_ratio);

    return QRect(x, y, w, h);
}

// 小苹果尺寸与间距
constexpr qreal kSavedAppleSizeRatio = 0.028;
constexpr qreal kSavedAppleSpacingRatio = 0.005;

constexpr int kSavedAppleMinSize = 16;
constexpr int kSavedAppleMaxSize = 40;
constexpr int kSavedAppleMinSpacing = 2;
constexpr int kSavedAppleMaxSpacing = 40;

// 下落苹果半径
constexpr qreal kAppleRadiusRatio = 0.025;
constexpr int kAppleRadiusMin = 14;
constexpr int kAppleRadiusMax = 40;
}


SaveAppleView::SaveAppleView(QWidget* parent)
    : QWidget(parent)
    , m_background_pixmap(UiAssetPaths::Apple::background())
    , m_apple_pixmap(UiAssetPaths::Apple::normalApple())
    , m_small_apple_pixmap(UiAssetPaths::Apple::smallApple())
    , m_basket_pixmap(UiAssetPaths::Apple::basket())
    , m_start_button_pixmap(UiAssetPaths::Common::startButton())
    , m_pause_button_pixmap(UiAssetPaths::Common::pauseButton())
    , m_stop_button_pixmap(UiAssetPaths::Common::endButton())
    , m_settings_button_pixmap(UiAssetPaths::Common::setupButton())
    , m_exit_button_pixmap(UiAssetPaths::Common::exitButton())
{

    // 帮我朋友修改的特定适配代码，后续可以删除

      

    // ***********************

    setupUi();
    setupConnections();
    refreshFromModel();
    refreshButtonStates();
    setFocusPolicy(Qt::StrongFocus);
}

void SaveAppleView::setModel(SaveAppleModel* model)
{
    m_model = model;
    refreshFromModel();
    refreshButtonStates();
    update();
}

SaveAppleModel* SaveAppleView::model() const
{
    return m_model;
}

void SaveAppleView::setController(SaveAppleController* controller)
{
    m_controller = controller;
}

SaveAppleController* SaveAppleView::controller() const
{
    return m_controller;
}

void SaveAppleView::setAppleRadius(int apple_radius)
{
    if (apple_radius > 0)
    {
        m_apple_radius = apple_radius;
        update();
    }
}

int SaveAppleView::appleRadius() const
{
    return m_apple_radius;
}

void SaveAppleView::setFailLineVisible(bool visible)
{
    m_fail_line_visible = visible;
    update();
}

bool SaveAppleView::isFailLineVisible() const
{
    return m_fail_line_visible;
}

void SaveAppleView::refreshFromModel()
{
    if (m_score_label == nullptr
        || m_progress_label == nullptr
        || m_state_label == nullptr
        || m_difficulty_label == nullptr
        || m_max_miss_label == nullptr)
    {
        return;
    }

    if (m_model == nullptr)
    {
        m_score_label->setText(QStringLiteral("Score: 0"));
        m_progress_label->setText(QStringLiteral("Success: 0 / 0"));
        m_state_label->setText(QStringLiteral("State: Initial"));
        m_difficulty_label->setText(QStringLiteral("Difficulty: 1"));
        m_max_miss_label->setText(QStringLiteral("Max Miss: 0"));
        return;
    }

    m_score_label->setText(QStringLiteral("Score: %1").arg(m_model->score()));
    m_progress_label->setText(
        QStringLiteral("Success: %1 / %2")
            .arg(m_model->successCount())
            .arg(m_model->targetSuccessCount()));
    m_state_label->setText(QStringLiteral("State: %1").arg(gameStateText()));
    m_difficulty_label->setText(QStringLiteral("Difficulty: %1").arg(m_model->speedLevel()));
    m_max_miss_label->setText(QStringLiteral("Max Miss: %1").arg(m_model->maxMissCount()));
}

void SaveAppleView::refreshButtonStates()
{
    if (m_start_button == nullptr
        || m_pause_resume_button == nullptr
        || m_stop_button == nullptr
        || m_settings_button == nullptr
        || m_exit_button == nullptr)
    {
        return;
    }

    m_settings_button->setEnabled(true);
    m_exit_button->setEnabled(true);

    if (m_model == nullptr)
    {
        m_start_button->setEnabled(true);
        m_pause_resume_button->setEnabled(false);
        m_pause_resume_button->setText(QStringLiteral("Pause"));
        m_stop_button->setEnabled(false);
        return;
    }

    switch (m_model->gameState())
    {
    case GameState::Initial:
        m_start_button->setEnabled(true);
        m_pause_resume_button->setEnabled(false);
        m_pause_resume_button->setText(QStringLiteral("Pause"));
        m_stop_button->setEnabled(false);
        break;

    case GameState::Playing:
        m_start_button->setEnabled(false);
        m_pause_resume_button->setEnabled(true);
        m_pause_resume_button->setText(QStringLiteral("Pause"));
        m_stop_button->setEnabled(true);
        break;

    case GameState::Paused:
        m_start_button->setEnabled(false);
        m_pause_resume_button->setEnabled(true);
        m_pause_resume_button->setText(QStringLiteral("Resume"));
        m_stop_button->setEnabled(true);
        break;

    case GameState::End:
        m_start_button->setEnabled(true);
        m_pause_resume_button->setEnabled(false);
        m_pause_resume_button->setText(QStringLiteral("Pause"));
        m_stop_button->setEnabled(false);
        break;

    default:
        m_start_button->setEnabled(false);
        m_pause_resume_button->setEnabled(false);
        m_pause_resume_button->setText(QStringLiteral("Pause"));
        m_stop_button->setEnabled(false);
        break;
    }
}

void SaveAppleView::onGameEvent(const GameEvent& event)
{
    Q_UNUSED(event);

    refreshFromModel();
    refreshButtonStates();
    update();

    if (m_model == nullptr)
    {
        return;
    }

    if (event.type == GameEventType::LevelCompleted)
    {
        showPassDialog();
    }
    else if (event.type == GameEventType::GameOver)
    {
        showFailDialog();
    }
}


void SaveAppleView::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    if (m_scene_rect.isEmpty())
    {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    drawScene(painter);
}

void SaveAppleView::keyPressEvent(QKeyEvent* event)
{
    if (event == nullptr)
    {
        return;
    }

    if (m_controller == nullptr)
    {
        QWidget::keyPressEvent(event);
        return;
    }

    const int key = event->key();
    if (key >= Qt::Key_A && key <= Qt::Key_Z)
    {
        const QChar input_letter(static_cast<char>(key));
        m_controller->handleLetterInput(input_letter.toUpper());
        update();
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}

void SaveAppleView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    updateSceneRect();
    updateOverlayGeometries();
    updateButtonGeometries();

    // 设置失败线为场景高度的70%，留出篮子和底部空白区域
    if (m_model != nullptr)
    {
        m_model->setFailLineY(static_cast<int>(m_scene_rect.height() * 0.7));
    }


}

void SaveAppleView::showPassDialog()
{
    TwoActionNotifyDialog* dialog = new TwoActionNotifyDialog(
    UiAssetPaths::Apple::endButton(),
    UiAssetPaths::Apple::nextButton(),
    this);

    dialog->setBackgroundPixmap(UiAssetPaths::Apple::dialogBackground());
    dialog->setMessage(QStringLiteral("恭喜您，过关了！"));

    connect(dialog, &TwoActionNotifyDialog::actionOneClicked, this, [this]() {
        qDebug() << "[showPassDialog] actionOneClicked received";
        qDebug() << "[showPassDialog] Current game state:" << static_cast<int>(m_model ? m_model->gameState() : GameState::Initial);
        qDebug() << "[showPassDialog] Emitting stopClicked";
        emit stopClicked();
        qDebug() << "[showPassDialog] stopClicked emitted";
    });

    // 连接下一关按钮，点击后提升难度并重新开始游戏
    connect(dialog, &TwoActionNotifyDialog::actionTwoClicked, this, [this]() {
        qDebug() << "[showPassDialog] actionTwoClicked received";
        if (m_model != nullptr)
        {
            const int next_level = qMin(m_model->speedLevel() + 1, 10);
            m_model->setSpeedLevel(next_level);
            qDebug() << "[showPassDialog] Speed level set to:" << next_level;
        }
        emit startClicked();
    });

    dialog->exec();
    delete dialog;
}

void SaveAppleView::showFailDialog()
{
    TwoActionNotifyDialog* dialog = new TwoActionNotifyDialog(
        UiAssetPaths::Apple::endButton(),
        UiAssetPaths::Apple::retryButton(),
        this);

    dialog->setBackgroundPixmap(UiAssetPaths::Apple::dialogBackground());
    dialog->setMessage(QStringLiteral("很遗憾，挑战失败了！"));

    connect(dialog, &TwoActionNotifyDialog::actionOneClicked, this, [this]() {
        qDebug() << "[showFailDialog] actionOneClicked received";
        qDebug() << "[showFailDialog] Current game state:" << static_cast<int>(m_model ? m_model->gameState() : GameState::Initial);
        qDebug() << "[showFailDialog] Emitting stopClicked";
        emit stopClicked();
        qDebug() << "[showFailDialog] stopClicked emitted";
    });

    // 连接重试按钮，点击后重新开始当前关卡
    connect(dialog, &TwoActionNotifyDialog::actionTwoClicked, this, [this]() {
        qDebug() << "[showFailDialog] actionTwoClicked received";
        emit startClicked();
    });

    dialog->exec();
    delete dialog;
}


void SaveAppleView::setupUi()
{
    setObjectName(QStringLiteral("saveapple_view"));
    setMinimumSize(1024, 720);

    m_status_container = new QWidget(this);
    m_status_layout = new QHBoxLayout(m_status_container);
    m_status_layout->setContentsMargins(18, 10, 18, 10);
    m_status_layout->setSpacing(20);

    m_score_label = new QLabel(QStringLiteral("Score: 0"), m_status_container);
    m_progress_label = new QLabel(QStringLiteral("Success: 0 / 0"), m_status_container);
    m_state_label = new QLabel(QStringLiteral("State: Initial"), m_status_container);
    m_difficulty_label = new QLabel(QStringLiteral("Difficulty: 1"), m_status_container);
    m_max_miss_label = new QLabel(QStringLiteral("Max Miss: 0"), m_status_container);

    const QString status_label_style = QStringLiteral(
        "QLabel {"
        "  color: #ffffff;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "  background: transparent;"
        "}"
    );

    m_score_label->setStyleSheet(status_label_style);
    m_progress_label->setStyleSheet(status_label_style);
    m_state_label->setStyleSheet(status_label_style);
    m_difficulty_label->setStyleSheet(status_label_style);
    m_max_miss_label->setStyleSheet(status_label_style);

    m_status_container->setStyleSheet(QStringLiteral(
        "QWidget {"
        "  background: rgba(8, 20, 40, 150);"
        "  border: 1px solid rgba(255, 255, 255, 110);"
        "  border-radius: 16px;"
        "}"
    ));

    m_status_layout->addWidget(m_score_label);
    m_status_layout->addStretch();
    m_status_layout->addWidget(m_progress_label);
    m_status_layout->addStretch();
    m_status_layout->addWidget(m_state_label);
    m_status_layout->addStretch();
    m_status_layout->addWidget(m_difficulty_label);
    m_status_layout->addStretch();
    m_status_layout->addWidget(m_max_miss_label);

    m_start_button = new SpriteButton(this);
    m_pause_resume_button = new SpriteButton(this);
    m_stop_button = new SpriteButton(this);
    m_settings_button = new SpriteButton(this);
    m_exit_button = new SpriteButton(this);


    m_start_button->setSprite(m_start_button_pixmap);
    m_pause_resume_button->setSprite(m_pause_button_pixmap);
    m_stop_button->setSprite(m_stop_button_pixmap);
    m_settings_button->setSprite(m_settings_button_pixmap);
    m_exit_button->setSprite(m_exit_button_pixmap); // 先拿已有图测试显示




    updateSceneRect();
    updateOverlayGeometries();
    updateButtonGeometries();
}

void SaveAppleView::setupConnections()
{
    connect(m_start_button, &QPushButton::clicked, this, &SaveAppleView::startClicked);
    connect(m_pause_resume_button, &QPushButton::clicked, this, &SaveAppleView::pauseResumeClicked);
    connect(m_stop_button, &QPushButton::clicked, this, &SaveAppleView::stopClicked);
    connect(m_settings_button, &QPushButton::clicked, this, &SaveAppleView::settingsClicked);

    connect(m_exit_button, &QPushButton::clicked, this, &SaveAppleView::exitClicked);
}

void SaveAppleView::updateSceneRect()
{
    m_scene_rect = rect().adjusted(
        kSceneLeftMargin,
        kSceneTopMargin,
        -kSceneRightMargin,
        -kSceneBottomMargin);

    const QSize view_size = size();

    m_basket_rect = rectFromRatio(view_size, kBasketLayout);
    m_saved_apples_rect = rectFromRatio(view_size, kSavedApplesLayout);

    m_apple_radius = qBound(
        kAppleRadiusMin,
        static_cast<int>(width() * kAppleRadiusRatio),
        kAppleRadiusMax);

    update();
}



void SaveAppleView::updateOverlayGeometries()
{
    if (m_status_container != nullptr)
    {
        m_status_container->setGeometry(
            kStatusLeft,
            kStatusTop,
            width() - (kStatusLeft * 2),
            kStatusHeight);
    }
}

void SaveAppleView::updateButtonGeometries()
{
    const QSize view_size = size();

    if (m_start_button != nullptr)
    {
        m_start_button->setGeometry(geometryFromRatio(view_size, kStartButtonLayout));
    }

    if (m_pause_resume_button != nullptr)
    {
        m_pause_resume_button->setGeometry(geometryFromRatio(view_size, kPauseButtonLayout));
    }

    if (m_stop_button != nullptr)
    {
        m_stop_button->setGeometry(geometryFromRatio(view_size, kStopButtonLayout));
    }

    if (m_settings_button != nullptr)
    {
        m_settings_button->setGeometry(geometryFromRatio(view_size, kSettingsButtonLayout));
    }

    if (m_exit_button != nullptr)
    {
        m_exit_button->setGeometry(geometryFromRatio(view_size, kExitButtonLayout));
    }
}



QString SaveAppleView::gameStateText() const
{
    if (m_model == nullptr)
    {
        return QStringLiteral("Initial");
    }

    switch (m_model->gameState())
    {
    case GameState::Initial:
        return QStringLiteral("Initial");
    case GameState::Playing:
        return QStringLiteral("Playing");
    case GameState::Paused:
        return QStringLiteral("Paused");
    case GameState::End:
        return QStringLiteral("End");
    default:
        return QStringLiteral("Unknown");
    }
}

void SaveAppleView::drawScene(QPainter& painter)
{
    drawSceneBackground(painter);
    // drawFailLine(painter);
    drawBasket(painter);
    drawSavedApples(painter);
    drawApples(painter);
}

void SaveAppleView::drawSceneBackground(QPainter& painter)
{
    if (m_scene_rect.isEmpty())
    {
        return;
    }

    painter.save();

    if (!m_background_pixmap.isNull())
    {
        painter.drawPixmap(rect(), m_background_pixmap);
    }
    else
    {
        painter.fillRect(rect(), QColor(32, 49, 85));
    }

    painter.restore();
}

void SaveAppleView::drawFailLine(QPainter& painter)
{
    if (!m_fail_line_visible || m_model == nullptr || m_scene_rect.isEmpty())
    {
        return;
    }

    const int fail_line_y = m_scene_rect.top() + m_model->failLineY();
    if (fail_line_y < m_scene_rect.top() || fail_line_y > m_scene_rect.bottom())
    {
        return;
    }

    painter.save();

    QPen pen(QColor(255, 90, 90), 2, Qt::DashLine);
    painter.setPen(pen);
    painter.drawLine(30, fail_line_y, width() - 30, fail_line_y);

    painter.setPen(QColor(255, 255, 255));
    painter.setFont(QFont(QStringLiteral("Arial"), 10, QFont::Bold));
    painter.drawText(36, fail_line_y - 8, QStringLiteral("Fail Line"));

    painter.restore();
}

void SaveAppleView::drawBasket(QPainter& painter)
{
    if (m_basket_rect.isEmpty())
    {
        return;
    }

    painter.save();

    if (!m_basket_pixmap.isNull())
    {
        painter.drawPixmap(m_basket_rect, m_basket_pixmap);
    }
    else
    {
        painter.setPen(QPen(QColor(80, 40, 10), 2));
        painter.setBrush(QColor(173, 120, 64));
        painter.drawRoundedRect(m_basket_rect, 10, 10);
    }

    painter.restore();
}

void SaveAppleView::drawSavedApples(QPainter& painter)
{
    if (m_model == nullptr || m_saved_apples_rect.isEmpty())
    {
        return;
    }

    const int success_count = m_model->successCount();
    if (success_count <= 0)
    {
        return;
    }

    const int draw_count = qMin(success_count, 20);

    const int saved_apple_size = qBound(
        kSavedAppleMinSize,
        static_cast<int>(width() * kSavedAppleSizeRatio),
        kSavedAppleMaxSize);

    const int saved_apple_spacing = qBound(
        kSavedAppleMinSpacing,
        static_cast<int>(width() * kSavedAppleSpacingRatio),
        kSavedAppleMaxSpacing);

    // 一行最多可容纳多少个苹果
    const int available_width = m_saved_apples_rect.width();
    const int apples_per_row = qMax(
        1,
        (available_width + saved_apple_spacing) / (saved_apple_size + saved_apple_spacing));

    // 需要多少行
    const int row_count = (draw_count + apples_per_row - 1) / apples_per_row;

    // 换行时顶行放余数个，下面各行铺满（底层更宽，像堆在篮子里）
    const int remainder = draw_count % apples_per_row;

    // 行间距，这里先和列间距保持一致
    const int row_spacing = saved_apple_spacing;

    // 整个内容块高度
    const int content_height = row_count * saved_apple_size
        + (row_count - 1) * row_spacing;

    // 垂直居中起点
    const int start_y = m_saved_apples_rect.top()
        + (m_saved_apples_rect.height() - content_height) / 2;

    painter.save();

    for (int i = 0; i < draw_count; ++i)
    {
        int row = 0;
        int col = 0;
        if (remainder > 0)
        {
            if (i < remainder)
            {
                row = 0;
                col = i;
            }
            else
            {
                const int j = i - remainder;
                row = 1 + j / apples_per_row;
                col = j % apples_per_row;
            }
        }
        else
        {
            row = i / apples_per_row;
            col = i % apples_per_row;
        }

        const int apples_in_this_row = (remainder > 0 && row == 0)
            ? remainder
            : apples_per_row;

        // 当前行总宽度
        const int row_width = apples_in_this_row * saved_apple_size
            + (apples_in_this_row - 1) * saved_apple_spacing;

        // 当前行水平居中
        const int start_x = m_saved_apples_rect.left()
            + (m_saved_apples_rect.width() - row_width) / 2;

        const int x = start_x + col * (saved_apple_size + saved_apple_spacing);
        const int y = start_y + row * (saved_apple_size + row_spacing);

        const QRect apple_rect(x, y, saved_apple_size, saved_apple_size);

        if (!m_small_apple_pixmap.isNull())
        {
            painter.drawPixmap(apple_rect, m_small_apple_pixmap);
        }
        else
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 90, 90));
            painter.drawEllipse(apple_rect);
        }
    }

    painter.restore();
}


void SaveAppleView::drawApples(QPainter& painter)
{
    if (m_model == nullptr || m_scene_rect.isEmpty())
    {
        return;
    }

    const QList<AppleModel*>& apples = m_model->activeApples();

    for (AppleModel* apple : apples)
    {
        if (apple == nullptr || !apple->isActive())
        {
            continue;
        }

        const int diameter = m_apple_radius * 2;
        const int left = apple->x() - m_apple_radius;
        const int top = apple->y() - m_apple_radius;
        const QRect apple_rect(left, top, diameter, diameter);

        if (!rect().adjusted(-diameter, -diameter, diameter, diameter).intersects(apple_rect))
        {
            continue;
        }

        painter.save();

        if (!m_apple_pixmap.isNull())
        {
            painter.drawPixmap(apple_rect, m_apple_pixmap);
        }
        else
        {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(255, 80, 80));
            painter.drawEllipse(apple_rect);
        }

        if (apple->isHit())
        {
            painter.setPen(QPen(QColor(255, 240, 120), 3));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(apple_rect.adjusted(2, 2, -2, -2));
        }

        drawAppleLetter(painter, apple_rect, apple->letter());
        painter.restore();
    }
}

void SaveAppleView::drawAppleLetter(QPainter& painter, const QRect& apple_rect, const QChar& letter)
{
    QFont font = painter.font();
    font.setBold(true);
    font.setPointSize(qMax(16, apple_rect.height() / 3));
    painter.setFont(font);

    const QString text(letter);

    painter.setPen(QPen(QColor(0, 0, 0, 210), 3));
    painter.drawText(apple_rect.adjusted(1, 2, 1, 2), Qt::AlignCenter, text);

    painter.setPen(QColor(255, 255, 255));
    painter.drawText(apple_rect, Qt::AlignCenter, text);
}
