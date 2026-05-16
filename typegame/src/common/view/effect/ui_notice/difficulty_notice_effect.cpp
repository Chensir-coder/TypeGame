#include "common/view/effect/ui_notice/difficulty_notice_effect.h"

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QPainter>
#include <QPen>
#include <QtGlobal>

namespace
{
constexpr qreal kLifeSeconds = 2.0;
constexpr qreal kFadeInRatio = 0.12;
constexpr qreal kFadeOutStart = 0.72;

qreal smoothStep(qreal x)
{
    x = qBound<qreal>(0.0, x, 1.0);
    return x * x * (3.0 - 2.0 * x);
}

qreal easeOutBack(qreal x)
{
    x = qBound<qreal>(0.0, x, 1.0);
    constexpr qreal c1 = 1.70158;
    constexpr qreal c3 = c1 + 1.0;
    const qreal p = x - 1.0;
    return 1.0 + c3 * p * p * p + c1 * p * p;
}

qreal noticeOpacity(qreal t)
{
    t = qBound<qreal>(0.0, t, 1.0);
    if (t < kFadeInRatio) {
        return smoothStep(t / qMax<qreal>(0.001, kFadeInRatio));
    }
    if (t > kFadeOutStart) {
        const qreal u = (t - kFadeOutStart)
                        / qMax<qreal>(0.001, 1.0 - kFadeOutStart);
        return 1.0 - smoothStep(u);
    }
    return 1.0;
}

int toAlpha(qreal opacity)
{
    return qBound(0, qRound(opacity * 255.0), 255);
}

qreal entryScale(qreal t)
{
    t = qBound<qreal>(0.0, t, 1.0);
    constexpr qreal kWindow = 0.14;
    if (t >= kWindow) {
        return 1.0;
    }
    const qreal u = t / kWindow;
    return 0.88 + 0.12 * easeOutBack(u);
}
} // namespace

DifficultyNoticeEffect::DifficultyNoticeEffect(const QPointF& logical_center,
                                               const QSizeF& logical_size,
                                               const QString& message)
{
    m_logicalPos = logical_center;
    m_logicalSize = logical_size;
    m_message = message;
    m_lifeTime = kLifeSeconds;
}

void DifficultyNoticeEffect::update(qreal dt)
{
    if (!m_alive || dt <= 0.0) {
        return;
    }

    m_elapsed += dt;
    if (m_elapsed >= m_lifeTime) {
        m_alive = false;
    }
}

void DifficultyNoticeEffect::paint(QPainter* painter,
                                   qreal viewScale,
                                   const QPointF& cameraOffset) const
{
    if (!m_alive || painter == nullptr || m_message.isEmpty()) {
        return;
    }

    const qreal t = qBound<qreal>(
        0.0,
        m_elapsed / qMax<qreal>(0.001, m_lifeTime),
        1.0
    );
    const qreal opacity = noticeOpacity(t);
    const qreal scale = entryScale(t);

    const QPointF screenCenter = (m_logicalPos - cameraOffset) * viewScale;
    const QSizeF screenSize = m_logicalSize * viewScale;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->translate(screenCenter);
    painter->scale(scale, scale);

    QFont font;
    font.setFamily(QStringLiteral("Segoe UI"));
    font.setBold(true);
    font.setLetterSpacing(QFont::PercentageSpacing, 105.0);
    const int px = qBound(
        26,
        qRound(screenSize.height() * 0.52),
        78
    );
    font.setPixelSize(px);
    painter->setFont(font);

    const QFontMetrics fm(font);
    const qreal textWidth = static_cast<qreal>(fm.horizontalAdvance(m_message));
    const qreal textH = static_cast<qreal>(fm.height());
    const qreal x = -textWidth / 2.0;
    const qreal y = fm.ascent() - textH / 2.0;

    const int a = toAlpha(opacity);
    const qreal off = qMax(2.0, 2.5 * viewScale);

    /* 单层位移阴影（一次 drawText） */
    painter->setPen(QColor(0, 0, 0, qBound(0, qRound(static_cast<qreal>(a) * 0.45), 255)));
    painter->drawText(QPointF(x + off, y + off), m_message);

    /* 横向渐变标题（一次 drawText） */
    QLinearGradient grad(QPointF(x, 0), QPointF(x + textWidth, 0));
    grad.setColorAt(0.0, QColor(255, 190, 70, a));
    grad.setColorAt(0.55, QColor(255, 248, 220, a));
    grad.setColorAt(1.0, QColor(70, 210, 255, a));
    painter->setPen(QPen(QBrush(grad), 1.0));
    painter->drawText(QPointF(x, y), m_message);

    /* 一条装饰下划线，强调「升级」感 */
    const qreal lineY = y + fm.descent() + 4.0 * viewScale;
    QPen linePen(QColor(120, 230, 255, qBound(0, qRound(static_cast<qreal>(a) * 0.85), 255)));
    linePen.setWidthF(qMax(2.0, 2.5 * viewScale));
    linePen.setCapStyle(Qt::FlatCap);
    painter->setPen(linePen);
    const qreal margin = textWidth * 0.08;
    painter->drawLine(
        QPointF(x + margin, lineY),
        QPointF(x + textWidth - margin, lineY)
    );

    painter->restore();
}
