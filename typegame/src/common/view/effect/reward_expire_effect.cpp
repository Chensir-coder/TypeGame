#include "common/view/effect/reward_expire_effect.h"

#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QPolygonF>
#include <QRadialGradient>
#include <QtGlobal>
#include <QtMath>

namespace
{
constexpr qreal kPi = 3.14159265358979323846;
constexpr qreal kTwoPi = kPi * 2.0;

qreal smoothStep(qreal x)
{
    x = qBound<qreal>(0.0, x, 1.0);
    return x * x * (3.0 - 2.0 * x);
}

qreal easeOutCubic(qreal x)
{
    x = qBound<qreal>(0.0, x, 1.0);
    const qreal inv = 1.0 - x;
    return 1.0 - inv * inv * inv;
}

qreal easeOutBack(qreal x)
{
    x = qBound<qreal>(0.0, x, 1.0);
    constexpr qreal c1 = 1.70158;
    constexpr qreal c3 = c1 + 1.0;
    const qreal p = x - 1.0;
    return 1.0 + c3 * p * p * p + c1 * p * p;
}

int alpha(qreal value)
{
    return qBound(0, qRound(value), 255);
}
} // namespace

RewardExpireEffect::RewardExpireEffect(const QPointF& logical_center,
                                       const QSizeF& logical_size,
                                       const QString& word)
{
    m_logicalPos = logical_center;
    m_logicalSize = logical_size;
    m_word = word.toUpper();

    // 原来是 0.72，现在延长到 1.5 秒
    m_lifeTime = 1.5;
}

void RewardExpireEffect::update(qreal dt)
{
    if (!m_alive || dt <= 0.0) {
        return;
    }

    m_elapsed += dt;
    if (m_elapsed >= m_lifeTime) {
        m_alive = false;
    }
}

void RewardExpireEffect::paint(QPainter* painter,
                               qreal viewScale,
                               const QPointF& cameraOffset) const
{
    if (!m_alive || painter == nullptr) {
        return;
    }

    const qreal t = qBound<qreal>(
        0.0,
        m_elapsed / qMax<qreal>(0.001, m_lifeTime),
        1.0
    );

    // 前 70% 基本保持亮度，最后 30% 快速淡出，比线性 fade 更有冲击感
    const qreal fade = (t < 0.70)
        ? 1.0
        : 1.0 - smoothStep((t - 0.70) / 0.30);

    const qreal appear = smoothStep(t / 0.14);
    const qreal ease = easeOutCubic(t);
    const qreal pulse = 0.5 + 0.5 * qSin(t * kTwoPi * 3.0);

    const QPointF screenCenter = (m_logicalPos - cameraOffset) * viewScale;
    const QSizeF screenSize = m_logicalSize * viewScale;
    const qreal baseRadius = qMax(screenSize.width(), screenSize.height()) * 0.52;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->translate(screenCenter);

    // 使用叠加混合，让光效更亮
    painter->setCompositionMode(QPainter::CompositionMode_Plus);

    // 1. 核心能量光晕
    {
        const qreal glowRadius = baseRadius * (0.58 + pulse * 0.16);
        QRadialGradient gradient(QPointF(0.0, 0.0), glowRadius);
        gradient.setColorAt(0.00, QColor(255, 248, 180, alpha(95.0 * fade * appear)));
        gradient.setColorAt(0.38, QColor(105, 235, 255, alpha(60.0 * fade * appear)));
        gradient.setColorAt(1.00, QColor(105, 235, 255, 0));

        painter->setPen(Qt::NoPen);
        painter->setBrush(gradient);
        painter->drawEllipse(QPointF(0.0, 0.0), glowRadius, glowRadius * 0.72);
    }

    // 2. 双层冲击波
    for (int i = 0; i < 2; ++i) {
        const qreal localT = qBound<qreal>(0.0, (t - i * 0.12) / 0.74, 1.0);
        if (localT <= 0.0 || localT >= 1.0) {
            continue;
        }

        const qreal waveEase = easeOutCubic(localT);
        const qreal waveFade = (1.0 - localT) * fade;
        const qreal radius = baseRadius * (0.32 + waveEase * (1.25 + i * 0.25));

        QPen pen(
            i == 0
                ? QColor(100, 230, 255, alpha(145.0 * waveFade))
                : QColor(255, 230, 100, alpha(120.0 * waveFade))
        );
        pen.setWidthF(qMax<qreal>(1.2, (3.8 - i) * viewScale * (1.0 - localT * 0.45)));
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QPointF(0.0, 0.0), radius, radius * 0.62);
    }

    // 3. 旋转能量弧
    for (int i = 0; i < 3; ++i) {
        const qreal radius = baseRadius * (0.54 + 0.19 * i + 0.10 * ease);
        QRectF arcRect(
            -radius,
            -radius * 0.66,
            radius * 2.0,
            radius * 1.32
        );

        QPen pen(
            i % 2 == 0
                ? QColor(105, 232, 255, alpha((170.0 - i * 32.0) * fade))
                : QColor(255, 232, 96, alpha((150.0 - i * 30.0) * fade))
        );
        pen.setWidthF(qMax<qreal>(1.4, (3.4 - i * 0.45) * viewScale));
        pen.setCapStyle(Qt::RoundCap);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        const int startAngle = qRound(
            (t * 420.0 * (i % 2 == 0 ? 1.0 : -1.0) + i * 118.0) * 16.0
        );
        const int spanAngle = qRound(
            (72.0 + 22.0 * qSin(t * kTwoPi * 2.0 + i)) * 16.0
        );

        painter->drawArc(arcRect, startAngle, spanAngle);
    }

    // 4. 射线爆发
    {
        const int rayCount = 18;
        for (int i = 0; i < rayCount; ++i) {
            const qreal angle = (static_cast<qreal>(i) / rayCount) * kTwoPi
                                + static_cast<qreal>(m_word.size()) * 0.11;
            const qreal rayFade = fade * (1.0 - smoothStep(t));
            const qreal inner = baseRadius * (0.18 + ease * 0.16);
            const qreal outer = baseRadius * (0.42 + ease * (0.70 + 0.08 * (i % 3)));

            QPen pen(
                i % 2 == 0
                    ? QColor(120, 238, 255, alpha(125.0 * rayFade))
                    : QColor(255, 238, 120, alpha(105.0 * rayFade))
            );
            pen.setWidthF(qMax<qreal>(1.0, 1.8 * viewScale));
            pen.setCapStyle(Qt::RoundCap);
            painter->setPen(pen);

            const QPointF p1(qCos(angle) * inner, qSin(angle) * inner * 0.68);
            const QPointF p2(qCos(angle) * outer, qSin(angle) * outer * 0.68 - ease * baseRadius * 0.16);
            painter->drawLine(p1, p2);
        }
    }

    // 5. 粒子和碎片
    painter->setPen(Qt::NoPen);

    const int particleCount = 24;
    for (int i = 0; i < particleCount; ++i) {
        const qreal delay = 0.018 * (i % 6);
        const qreal localT = qBound<qreal>(0.0, (t - delay) / 0.82, 1.0);
        if (localT <= 0.0) {
            continue;
        }

        const qreal particleEase = easeOutCubic(localT);
        const qreal particleFade = fade * (1.0 - smoothStep(localT * 0.92));
        if (particleFade <= 0.01) {
            continue;
        }

        const qreal angle = (static_cast<qreal>(i) / particleCount) * kTwoPi
                            + static_cast<qreal>(m_word.size()) * 0.17
                            + qSin(i * 1.37) * 0.18;

        const qreal distance = baseRadius * (
            0.16 + particleEase * (0.88 + 0.07 * (i % 5))
        );

        const QPointF pos(
            qCos(angle) * distance,
            qSin(angle) * distance * 0.72 - particleEase * baseRadius * (0.20 + 0.03 * (i % 2))
        );

        const qreal radius = qMax<qreal>(1.5, (2.5 + (i % 4)) * viewScale)
                             * (1.0 - localT * 0.45);

        const QColor color = (i % 2 == 0)
            ? QColor(105, 235, 255, alpha(225.0 * particleFade))
            : QColor(255, 232, 96, alpha(215.0 * particleFade));

        painter->save();
        painter->translate(pos);
        painter->rotate(angle * 180.0 / kPi + t * 220.0);

        painter->setBrush(color);

        if (i % 3 == 0) {
            QPolygonF diamond;
            diamond << QPointF(0.0, -radius * 1.8)
                    << QPointF(radius * 0.85, 0.0)
                    << QPointF(0.0, radius * 1.8)
                    << QPointF(-radius * 0.85, 0.0);
            painter->drawPolygon(diamond);
        } else {
            painter->drawEllipse(
                QRectF(
                    -radius * 1.8,
                    -radius * 0.55,
                    radius * 3.6,
                    radius * 1.1
                )
            );
        }

        painter->restore();
    }

    // 6. 中心闪光十字
    {
        const qreal flash = qMax<qreal>(0.0, 1.0 - t / 0.26) * appear;
        if (flash > 0.0) {
            QPen pen(QColor(255, 255, 230, alpha(190.0 * flash)));
            pen.setWidthF(qMax<qreal>(1.2, 2.6 * viewScale));
            pen.setCapStyle(Qt::RoundCap);
            painter->setPen(pen);

            const qreal len = baseRadius * (0.28 + 0.18 * pulse);
            painter->drawLine(QPointF(-len, 0.0), QPointF(len, 0.0));
            painter->drawLine(QPointF(0.0, -len * 0.72), QPointF(0.0, len * 0.72));
        }
    }

    // 文字建议用普通 alpha 混合，避免过度发白
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    // 7. 文字弹出、发光、上浮
    if (!m_word.isEmpty()) {
        const qreal textAppear = easeOutBack((t - 0.05) / 0.28);
        const qreal textFade = (t < 0.78)
            ? 1.0
            : 1.0 - smoothStep((t - 0.78) / 0.22);

        const qreal wordAlpha = qBound<qreal>(0.0, textAppear, 1.15)
                                * textFade
                                * fade;

        QFont font;
        font.setFamily(QStringLiteral("Segoe UI"));
        font.setBold(true);
        font.setPixelSize(qBound(16, qRound(screenSize.height() * 0.34), 42));
        painter->setFont(font);

        const QFontMetrics fm(font);
        const qreal wordWidth = static_cast<qreal>(fm.horizontalAdvance(m_word));

        qreal x = -wordWidth / 2.0;
        const qreal lift = baseRadius * (0.16 + ease * 0.46);

        for (int i = 0; i < m_word.size(); ++i) {
            const QString ch(m_word.at(i));
            const qreal charWidth = static_cast<qreal>(fm.horizontalAdvance(ch));

            const qreal centerIndex = (m_word.size() - 1) / 2.0;
            const qreal indexOffset = static_cast<qreal>(i) - centerIndex;

            const qreal spread = indexOffset * ease * 10.0 * viewScale;
            const qreal bob = qSin(t * 10.0 + i * 1.73) * 3.8 * viewScale * (1.0 - t);
            const qreal y = -lift + bob;

            const QPointF pos(x + spread, y);

            painter->save();
            painter->translate(pos);
            painter->rotate(qSin(t * 8.0 + i * 1.37) * 5.5 * (1.0 - t));

            // 外发光
            painter->setPen(QColor(80, 220, 255, alpha(70.0 * wordAlpha)));
            painter->drawText(QPointF(-2.0 * viewScale, 0.0), ch);
            painter->drawText(QPointF(2.0 * viewScale, 0.0), ch);
            painter->drawText(QPointF(0.0, -2.0 * viewScale), ch);
            painter->drawText(QPointF(0.0, 2.0 * viewScale), ch);

            // 阴影
            painter->setPen(QColor(0, 0, 0, alpha(145.0 * wordAlpha)));
            painter->drawText(QPointF(1.4 * viewScale, 1.4 * viewScale), ch);

            // 主文字
            painter->setPen(QColor(255, 236, 92, alpha(245.0 * wordAlpha)));
            painter->drawText(QPointF(0.0, 0.0), ch);

            // 高光
            painter->setPen(QColor(255, 255, 220, alpha(105.0 * wordAlpha)));
            painter->drawText(QPointF(0.0, -1.0 * viewScale), ch);

            painter->restore();

            x += charWidth;
        }
    }

    painter->restore();
}
