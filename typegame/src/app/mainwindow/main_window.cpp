/* ------------------------------------------------------------------
 // 文件名     : main_window.cpp
 // 创建者     : 3263297739@qq.com
 // 创建时间   : 2026-04-13
 // 功能描述   : 主窗口实现
------------------------------------------------------------------ */

#include "main_window.h"

#include <QApplication>
#include <QFont>
#include <QGraphicsDropShadowEffect>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QMessageBox>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QPolygon>
#include <QResizeEvent>
#include <QWidget>
#include <QtGlobal>

#include <functional>
#include <utility>

#include "app/factory/game_factory.h"
#include "app/mainwindow/game_window.h"
#include "app/resources/asset_paths.h"

namespace
{
void drawCoverPixmap(QPainter& painter, const QRect& dest, const QPixmap& pixmap)
{
    if (pixmap.isNull() || dest.width() <= 0 || dest.height() <= 0) {
        return;
    }

    const QSize destSize = dest.size();
    const QPixmap scaled = pixmap.scaled(destSize,
                                         Qt::KeepAspectRatioByExpanding,
                                         Qt::SmoothTransformation);
    const int x = (scaled.width() - destSize.width()) / 2;
    const int y = (scaled.height() - destSize.height()) / 2;
    painter.drawPixmap(dest,
                       scaled,
                       QRect(x, y, destSize.width(), destSize.height()));
}

class HomeCardWidget final : public QWidget
{
public:
    HomeCardWidget(QString title,
                   QPixmap icon,
                   bool enabled,
                   std::function<void()> onClicked,
                   QWidget* parent = nullptr)
        : QWidget(parent)
        , m_title(std::move(title))
        , m_icon(std::move(icon))
        , m_enabled(enabled)
        , m_onClicked(std::move(onClicked))
    {
        setAutoFillBackground(false);
        setAttribute(Qt::WA_TranslucentBackground);
        setAttribute(Qt::WA_Hover, true);
        setCursor(m_enabled ? Qt::PointingHandCursor : Qt::ArrowCursor);

        m_shadow = new QGraphicsDropShadowEffect(this);
        updateShadow();
        setGraphicsEffect(m_shadow);
    }

protected:
    void enterEvent(QEvent* event) override
    {
        QWidget::enterEvent(event);

        m_hovered = true;
        updateShadow();
        update();
    }

    void leaveEvent(QEvent* event) override
    {
        QWidget::leaveEvent(event);

        m_hovered = false;
        updateShadow();
        update();
    }

    void mousePressEvent(QMouseEvent* event) override
    {
        if (m_enabled && event != nullptr && event->button() == Qt::LeftButton) {
            if (m_onClicked) {
                m_onClicked();
            }
            event->accept();
            return;
        }

        QWidget::mousePressEvent(event);
    }

    void paintEvent(QPaintEvent* event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::TextAntialiasing, true);

        painter.fillRect(rect(), Qt::transparent);

        const qreal outerRadius = 10.0;
        const qreal innerFrameRadius = 6.0;
        const int framePad = 3;
        const int gapBelowImage = qMax(4, qRound(height() * 0.028));
        const int titleBand = qBound(26, qRound(height() * 0.15), 38);
        const int hMargin = qMax(8, qRound(width() * 0.06));
        const int topContent = qMax(8, qRound(height() * 0.04));

        const QRect imageFrameRect(hMargin,
                                   topContent,
                                   width() - 2 * hMargin,
                                   height() - topContent - gapBelowImage - titleBand);

        const QRect innerImageRect = imageFrameRect.adjusted(framePad, framePad, -framePad, -framePad);

        const QRectF outerRect = rect().adjusted(4, 4, -4, -4);
        const bool showOuterCard = m_hovered;

        if (showOuterCard) {
            QColor outerFill = m_enabled ? QColor(255, 255, 255, 245) : QColor(248, 250, 252, 240);
            QColor outerBorder = m_enabled ? QColor(64, 150, 255, 210) : QColor(160, 170, 182, 200);
            painter.setPen(QPen(outerBorder, m_enabled ? 2.0 : 1.5));
            painter.setBrush(outerFill);
            painter.drawRoundedRect(outerRect, outerRadius, outerRadius);
        }

        painter.setPen(QPen(QColor(210, 218, 228), 1.0));
        painter.setBrush(Qt::white);
        painter.drawRoundedRect(QRectF(imageFrameRect), innerFrameRadius, innerFrameRadius);

        QPainterPath innerClip;
        innerClip.addRoundedRect(QRectF(innerImageRect),
                                 qMax(2.0, innerFrameRadius - 1.0),
                                 qMax(2.0, innerFrameRadius - 1.0));
        painter.save();
        painter.setClipPath(innerClip);

        if (!m_icon.isNull()) {
            painter.setOpacity(m_enabled ? 1.0 : 0.38);
            drawCoverPixmap(painter, innerImageRect, m_icon);
            painter.setOpacity(1.0);
        } else {
            painter.setPen(Qt::NoPen);
            painter.setBrush(m_enabled ? QColor(75, 157, 255) : QColor(170, 178, 188));
            painter.drawEllipse(innerImageRect);
        }

        painter.restore();

        if (!m_enabled) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(220, 224, 230, 85));
            painter.drawRoundedRect(QRectF(imageFrameRect), innerFrameRadius, innerFrameRadius);
        }

        QFont titleFont = painter.font();
        titleFont.setPixelSize(qBound(13, qRound(width() * 0.088), 19));
        titleFont.setBold(true);
        painter.setFont(titleFont);

        QColor titleColor;
        if (!m_enabled) {
            titleColor = QColor(145, 152, 162);
        } else if (m_hovered) {
            titleColor = QColor(30, 108, 210);
        } else {
            titleColor = QColor(48, 62, 78);
        }

        painter.setPen(titleColor);
        painter.drawText(QRect(hMargin,
                               height() - titleBand,
                               width() - 2 * hMargin,
                               titleBand),
                         Qt::AlignCenter,
                         m_title);
    }

private:
    void updateShadow()
    {
        if (m_shadow == nullptr) {
            return;
        }

        if (m_hovered) {
            m_shadow->setBlurRadius(26);
            m_shadow->setOffset(0, 7);
            m_shadow->setColor(QColor(55, 130, 220, 95));
        } else {
            m_shadow->setBlurRadius(0);
            m_shadow->setOffset(0, 0);
            m_shadow->setColor(QColor(0, 0, 0, 0));
        }
    }

private:
    QString m_title;
    QPixmap m_icon;
    bool m_enabled = true;
    bool m_hovered = false;
    std::function<void()> m_onClicked;
    QGraphicsDropShadowEffect* m_shadow = nullptr;
};

class HomeWidget final : public QWidget
{
public:
    HomeWidget(std::function<void()> onSaveApple,
               std::function<void()> onSpaceWar,
               QWidget* parent = nullptr)
        : QWidget(parent)
    {
        setObjectName(QStringLiteral("home_widget"));
        setMinimumSize(960, 540);
        setAttribute(Qt::WA_OpaquePaintEvent, true);

        const QPixmap comingSoonIcon(UiAssetPaths::Apple::normalApple());
        const QPixmap appleBackground(UiAssetPaths::Apple::background());
        const QPixmap spaceWarBackground(UiAssetPaths::SpaceWar::mainMenuBackground());

        m_cards.append(new HomeCardWidget(QStringLiteral("速度训练"),
                                          comingSoonIcon,
                                          false,
                                          nullptr,
                                          this));
        m_cards.append(new HomeCardWidget(QStringLiteral("进阶挑战"),
                                          comingSoonIcon,
                                          false,
                                          nullptr,
                                          this));
        m_cards.append(new HomeCardWidget(QStringLiteral("拯救苹果"),
                                          appleBackground,
                                          true,
                                          std::move(onSaveApple),
                                          this));
        m_cards.append(new HomeCardWidget(QStringLiteral("飞机大战"),
                                          spaceWarBackground,
                                          true,
                                          std::move(onSpaceWar),
                                          this));

        updateCardLayout();
    }

protected:
    void resizeEvent(QResizeEvent* event) override
    {
        QWidget::resizeEvent(event);
        updateCardLayout();
    }

    void paintEvent(QPaintEvent* event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::TextAntialiasing, true);

        drawBackground(painter);
        drawHeader(painter);
        drawHeroText(painter);
        drawFooter(painter);
    }

private:
    void updateCardLayout()
    {
        if (m_cards.isEmpty()) {
            return;
        }

        const int cardW = qBound(150, qRound(width() * 0.17), 190);
        const int cardH = cardW;
        const int spacing = qBound(28, qRound(width() * 0.055), 68);
        const int count = m_cards.size();
        const int totalW = count * cardW + (count - 1) * spacing;
        const int left = qMax(24, (width() - totalW) / 2);
        const int top = qBound(250, qRound(height() * 0.49), height() - cardH - 72);

        for (int i = 0; i < count; ++i) {
            m_cards.at(i)->setGeometry(left + i * (cardW + spacing), top, cardW, cardH);
        }
    }

    void drawBackground(QPainter& painter)
    {
        QLinearGradient page(0, 0, 0, height());
        page.setColorAt(0.0, QColor(235, 247, 255));
        page.setColorAt(0.36, QColor(224, 238, 249));
        page.setColorAt(1.0, QColor(210, 226, 240));
        painter.fillRect(rect(), page);

        QLinearGradient header(0, 0, width(), 112);
        header.setColorAt(0.0, QColor(58, 147, 246));
        header.setColorAt(0.55, QColor(82, 165, 250));
        header.setColorAt(1.0, QColor(43, 126, 227));
        painter.fillRect(QRect(0, 0, width(), 112), header);

        QLinearGradient ribbon(0, height() - 30, width(), height());
        ribbon.setColorAt(0.0, QColor(58, 148, 248));
        ribbon.setColorAt(1.0, QColor(38, 119, 226));
        painter.fillRect(QRect(0, height() - 30, width(), 30), ribbon);

        painter.setPen(QPen(QColor(74, 130, 198, 95), 1));
        painter.drawLine(0, 112, width(), 112);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 36));
        for (int i = 0; i < 8; ++i) {
            const int d = 24 + i * 7;
            painter.drawEllipse(QPointF(width() * (0.54 + i * 0.055), 36 + (i % 3) * 18), d, d);
        }
    }

    void drawHeader(QPainter& painter)
    {
        const QPoint logoCenter(82, 58);
        painter.setPen(Qt::NoPen);

        QPolygon orange;
        orange << QPoint(logoCenter.x() - 38, logoCenter.y())
               << QPoint(logoCenter.x() - 4, logoCenter.y() - 34)
               << QPoint(logoCenter.x() - 4, logoCenter.y() + 34);
        painter.setBrush(QColor(255, 129, 24));
        painter.drawPolygon(orange);

        QPolygon blue;
        blue << QPoint(logoCenter.x() + 4, logoCenter.y() - 38)
             << QPoint(logoCenter.x() + 34, logoCenter.y() - 12)
             << QPoint(logoCenter.x() + 4, logoCenter.y() + 16);
        painter.setBrush(QColor(63, 142, 242));
        painter.drawPolygon(blue);

        QPolygon green;
        green << QPoint(logoCenter.x() - 2, logoCenter.y() + 4)
              << QPoint(logoCenter.x() + 38, logoCenter.y() + 4)
              << QPoint(logoCenter.x() + 10, logoCenter.y() + 34);
        painter.setBrush(QColor(74, 190, 70));
        painter.drawPolygon(green);

        QFont titleFont(QStringLiteral("Microsoft YaHei"));
        titleFont.setPixelSize(34);
        painter.setFont(titleFont);
        painter.setPen(QColor(255, 255, 255));
        painter.drawText(QRect(140, 25, 420, 52),
                         Qt::AlignVCenter | Qt::AlignLeft,
                         QStringLiteral("打字游戏 TypeGame"));
    }

    void drawHeroText(QPainter& painter)
    {
        QFont titleFont(QStringLiteral("Microsoft YaHei"));
        titleFont.setPixelSize(qBound(26, width() / 36, 36));
        titleFont.setBold(false);
        painter.setFont(titleFont);

        const QString prefix = QStringLiteral("欢迎使用 ");
        const QString name = QStringLiteral("TypeGame");
        const int totalW =
            painter.fontMetrics().horizontalAdvance(prefix + name);
        const int startX = (width() - totalW) / 2;
        const int y = 176;

        painter.setPen(QColor(93, 103, 112));
        painter.drawText(startX, y, prefix);
        painter.setPen(QColor(255, 132, 0));
        painter.drawText(startX + painter.fontMetrics().horizontalAdvance(prefix), y, name);

        QFont versionFont(QStringLiteral("Microsoft YaHei"));
        versionFont.setPixelSize(16);
        versionFont.setBold(true);
        painter.setFont(versionFont);
        const QRect badge(startX + totalW + 10, y - 32, 50, 26);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 132, 0));
        painter.drawRoundedRect(badge, 3, 3);
        painter.setPen(Qt::white);
        painter.drawText(badge, Qt::AlignCenter, QStringLiteral("2026"));

        QFont subFont(QStringLiteral("Microsoft YaHei"));
        subFont.setPixelSize(17);
        painter.setFont(subFont);
        painter.setPen(QColor(145, 152, 160));
        painter.drawText(QRect(0, y + 18, width(), 32),
                         Qt::AlignCenter,
                         QStringLiteral("轻松练习打字，在游戏中提升速度与准确率"));
    }

    void drawFooter(QPainter& painter)
    {
        QFont font(QStringLiteral("Microsoft YaHei"));
        font.setPixelSize(12);
        painter.setFont(font);
        painter.setPen(QColor(255, 255, 255, 190));
        painter.drawText(QRect(20, height() - 28, width() - 40, 24),
                         Qt::AlignRight | Qt::AlignVCenter,
                         QStringLiteral("设置"));
    }

private:
    QVector<HomeCardWidget*> m_cards;
};
} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setupUi();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUi()
{
    resize(960, 540);
    setMinimumSize(960, 540);
    setWindowTitle(QStringLiteral("TypeGame"));

    m_home_widget = new HomeWidget(
        [this]() { onSaveAppleSelected(); },
        [this]() { onSpaceWarSelected(); },
        this);
    setCentralWidget(m_home_widget);
}

void MainWindow::onSaveAppleSelected()
{
    auto game = GameFactory::createGame(GameType::SaveApple);
    if (game == nullptr)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("该游戏暂未实现。"));
        return;
    }

    GameWindow* game_window = new GameWindow(std::move(game));
    game_window->show();
    m_game_windows.append(game_window);
}

void MainWindow::onSpaceWarSelected()
{
    auto game = GameFactory::createGame(GameType::SpaceWar);
    if (game == nullptr)
    {
        QMessageBox::information(this, QStringLiteral("提示"), QStringLiteral("该游戏暂未实现。"));
        return;
    }

    GameWindow* game_window = new GameWindow(std::move(game));
    game_window->show();
    m_game_windows.append(game_window);
}


