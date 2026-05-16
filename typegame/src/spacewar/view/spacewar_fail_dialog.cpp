#include "spacewar/view/spacewar_fail_dialog.h"

#include "common/view/sprite_button.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

namespace
{
constexpr int kDialogWidth = 560;
constexpr int kDialogHeight = 300;
constexpr int kButtonWidth = 132;
constexpr int kButtonHeight = 60;
}

SpaceWarFailDialog::SpaceWarFailDialog(const QPixmap& exit_button_pixmap,
                                       const QPixmap& retry_button_pixmap,
                                       QWidget* parent)
    : QDialog(parent)
    , m_button_one_pixmap(exit_button_pixmap)
    , m_button_two_pixmap(retry_button_pixmap)
{
    setObjectName(QStringLiteral("spacewar_fail_dialog"));
    setModal(true);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setFixedSize(kDialogWidth, kDialogHeight);
    setAutoFillBackground(false);
    setupUi();
}

void SpaceWarFailDialog::setMessage(const QString& message)
{
    if (m_message_label != nullptr) {
        m_message_label->setText(message);
    }
}

void SpaceWarFailDialog::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const QRect outer = rect().adjusted(10, 10, -10, -10);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 72));
    painter.drawRoundedRect(outer.translated(0, 5), 18, 18);

    QLinearGradient panel_grad(outer.topLeft(), outer.bottomLeft());
    panel_grad.setColorAt(0.0, QColor(28, 44, 82));
    panel_grad.setColorAt(0.55, QColor(14, 22, 48));
    panel_grad.setColorAt(1.0, QColor(6, 10, 26));

    painter.setBrush(panel_grad);
    painter.setPen(QPen(QColor(120, 210, 255, 210), 2));
    painter.drawRoundedRect(outer, 18, 18);

    painter.setPen(QPen(QColor(255, 255, 255, 28), 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(outer.adjusted(5, 5, -5, -5), 14, 14);

    const int n = 18;
    painter.setPen(Qt::NoPen);
    for (int i = 0; i < n; ++i) {
        const int x = 24 + (i * 9973) % (width() - 48);
        const int y = 16 + (i * 7919) % (height() / 2);
        const int s = 1 + (i % 3);
        painter.setBrush(QColor(255, 255, 255, 35 + (i % 5) * 12));
        painter.drawEllipse(QPoint(x, y), s, s);
    }
}

void SpaceWarFailDialog::setupUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(36, 28, 36, 22);
    root->setSpacing(12);

    m_message_label = new QLabel(QStringLiteral("游戏结束"), this);
    m_message_label->setAlignment(Qt::AlignCenter);
    m_message_label->setWordWrap(true);
    m_message_label->setStyleSheet(QStringLiteral(
        "QLabel {"
        "  color: #e8f6ff;"
        "  font-size: 22px;"
        "  font-weight: 700;"
        "  background: transparent;"
        "  border: none;"
        "  padding: 8px 12px;"
        "}"));

    auto* button_row = new QWidget(this);
    button_row->setAutoFillBackground(false);
    button_row->setStyleSheet(QStringLiteral("QWidget { background: transparent; }"));
    auto* button_layout = new QHBoxLayout(button_row);
    button_layout->setContentsMargins(0, 0, 0, 0);
    button_layout->setSpacing(24);

    m_button_one = new SpriteButton(button_row);
    m_button_two = new SpriteButton(button_row);
    m_button_one->setFixedSize(kButtonWidth, kButtonHeight);
    m_button_two->setFixedSize(kButtonWidth, kButtonHeight);
    m_button_one->setSprite(m_button_one_pixmap);
    m_button_two->setSprite(m_button_two_pixmap);

    button_layout->addStretch(1);
    button_layout->addWidget(m_button_one, 0, Qt::AlignVCenter);
    button_layout->addWidget(m_button_two, 0, Qt::AlignVCenter);
    button_layout->addStretch(1);

    root->addStretch(1);
    root->addWidget(m_message_label);
    root->addStretch(1);
    root->addWidget(button_row);

    connect(m_button_one, &QPushButton::clicked, this, [this]() {
        emit actionOneClicked();
        accept();
    });
    connect(m_button_two, &QPushButton::clicked, this, [this]() {
        emit actionTwoClicked();
        accept();
    });
}
