#include "two_action_notify_dialog.h"

#include "common/view/sprite_button.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QWidget>

namespace
{
constexpr int kDialogWidth = 640;
constexpr int kDialogHeight = 360;

constexpr int kButtonWidth = 132;
constexpr int kButtonHeight = 60;
}

TwoActionNotifyDialog::TwoActionNotifyDialog(const QPixmap& button_one_pixmap,
                                             const QPixmap& button_two_pixmap,
                                             QWidget* parent)
    : QDialog(parent)
    , m_button_one_pixmap(button_one_pixmap)
    , m_button_two_pixmap(button_two_pixmap)
{
    setupUi();
}

void TwoActionNotifyDialog::setMessage(const QString& message)
{
    if (m_message_label != nullptr)
    {
        m_message_label->setText(message);
    }
}

void TwoActionNotifyDialog::setBackgroundPixmap(const QPixmap& background_pixmap)
{
    m_background_pixmap = background_pixmap;
    update();
}

void TwoActionNotifyDialog::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (!m_background_pixmap.isNull())
    {
        painter.drawPixmap(rect(), m_background_pixmap);
    }
    else
    {
        painter.fillRect(rect(), QColor(255, 242, 220));
    }

    QDialog::paintEvent(event);
}

void TwoActionNotifyDialog::setupUi()
{
    setObjectName(QStringLiteral("two_action_notify_dialog"));
    setModal(true);
    resize(kDialogWidth, kDialogHeight);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground, false);

    QVBoxLayout* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(28, 24, 28, 24);
    main_layout->setSpacing(0);

    main_layout->addStretch();

    QWidget* message_container = new QWidget(this);
    QHBoxLayout* message_layout = new QHBoxLayout(message_container);
    message_layout->setContentsMargins(0, 0, 0, 0);
    message_layout->setSpacing(0);

    message_layout->addStretch(3);

    m_message_label = new QLabel(QStringLiteral("提示"), message_container);
    m_message_label->setAlignment(Qt::AlignCenter);
    m_message_label->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #FFF7E8;"
        "  font-size: 30px;"
        "  font-weight: 800;"
        "  background: rgba(60, 30, 10, 110);"
        "  border: 1px solid rgba(255, 235, 200, 120);"
        "  border-radius: 18px;"
        "  padding: 16px 28px;"
        "}"
    ));

    message_layout->addWidget(m_message_label, 0, Qt::AlignVCenter);
    message_layout->addStretch(1);

    main_layout->addWidget(message_container);
    main_layout->addStretch();

    QWidget* button_container = new QWidget(this);
    QHBoxLayout* button_layout = new QHBoxLayout(button_container);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(28);

    button_layout->addStretch();

    m_button_one = new SpriteButton(button_container);
    m_button_two = new SpriteButton(button_container);

    m_button_one->setFixedSize(kButtonWidth, kButtonHeight);
    m_button_two->setFixedSize(kButtonWidth, kButtonHeight);

    m_button_one->setSprite(m_button_one_pixmap);
    m_button_two->setSprite(m_button_two_pixmap);

    button_layout->addWidget(m_button_one);
    button_layout->addWidget(m_button_two);
    button_layout->addStretch();

    main_layout->addWidget(button_container);

    connect(m_button_one, &QPushButton::clicked, this, [this]() {
        emit actionOneClicked();
        accept();
    });

    connect(m_button_two, &QPushButton::clicked, this, [this]() {
        emit actionTwoClicked();
        accept();
    });
}
