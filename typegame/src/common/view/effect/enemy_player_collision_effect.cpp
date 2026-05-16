#include "common/view/effect/enemy_player_collision_effect.h"

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

qreal easeOutQuart(qreal x)
{
    x = qBound<qreal>(0.0, x, 1.0);
    const qreal inv = 1.0 - x;
    return 1.0 - inv * inv * inv * inv;
}

int alpha(qreal value)
{
    return qBound(0, qRound(value), 255);
}

QPointF polar(qreal angle, qreal distance, qreal yScale = 1.0)
{
    return QPointF(qCos(angle) * distance,
                   qSin(angle) * distance * yScale);
}

QPointF perpendicular(const QPointF& v)
{
    return QPointF(-v.y(), v.x());
}

QPolygonF makeShard(qreal radius)
{
    QPolygonF shard;
    shard << QPointF(radius * 1.7, 0.0)
          << QPointF(-radius * 0.6, -radius * 0.75)
          << QPointF(-radius * 1.1, 0.0)
          << QPointF(-radius * 0.4, radius * 0.72);
    return shard;
}
} // namespace

EnemyPlayerCollisionEffect::EnemyPlayerCollisionEffect(const QPointF& logical_center,
                                                       const QSizeF& logical_size,
                                                       qreal impact_angle_degrees)
{
    m_logicalPos = logical_center;
    m_logicalSize = logical_size;
    m_impactAngleDegrees = impact_angle_degrees;

    // 敌机撞击玩家：短促、猛烈，1 秒结束
    m_lifeTime = 1.0;
}

void EnemyPlayerCollisionEffect::update(qreal dt)
{
    if (!m_alive || dt <= 0.0) {
        return;
    }

    m_elapsed += dt;

    // 保留父类运动能力，外部 setVelocity() 仍然可用
    updateMovement(dt);
    onUpdate(dt);

    if (!m_infiniteLife && m_elapsed >= m_lifeTime) {
        m_alive = false;
        onLifeEnd();
    }
}

void EnemyPlayerCollisionEffect::paint(QPainter* painter,
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

    const qreal appear = smoothStep(t / 0.06);
    const qreal ease = easeOutCubic(t);
    const qreal fade = (t < 0.76)
        ? 1.0
        : 1.0 - smoothStep((t - 0.76) / 0.24);
    const qreal flash = qMax<qreal>(0.0, 1.0 - smoothStep(t / 0.18));
    const qreal blast = qMax<qreal>(0.0, 1.0 - smoothStep(t / 0.42));

    const QPointF screenCenter = (m_logicalPos - cameraOffset) * viewScale;
    const QSizeF screenSize = m_logicalSize * viewScale;

    const qreal baseRadius = qMax<qreal>(
        qMax(screenSize.width(), screenSize.height()) * 0.54,
        20.0 * viewScale
    );

    const qreal impactAngle = qDegreesToRadians(m_impactAngleDegrees);
    const QPointF dir(qCos(impactAngle), qSin(impactAngle));
    const QPointF perp = perpendicular(dir);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setOpacity(m_opacity);
    painter->translate(screenCenter);
    painter->rotate(m_rotation);
    painter->scale(m_localScale, m_localScale);

    // 火焰和爆闪使用叠加混合，避免产生奖励特效那种蓝色能量感。
    painter->setCompositionMode(QPainter::CompositionMode_Plus);

    // 1. 飞机毁灭核心火球
    {
        const qreal coreRadius = baseRadius * (0.34 + flash * 0.28 + ease * 0.36);

        QRadialGradient gradient(QPointF(0.0, 0.0), coreRadius);
        gradient.setColorAt(0.00, QColor(255, 255, 235, alpha(240.0 * flash * appear)));
        gradient.setColorAt(0.20, QColor(255, 210, 82, alpha(225.0 * blast * fade * appear)));
        gradient.setColorAt(0.52, QColor(255, 92, 22, alpha(185.0 * fade * appear)));
        gradient.setColorAt(0.82, QColor(128, 24, 8, alpha(100.0 * fade * appear)));
        gradient.setColorAt(1.00, QColor(80, 18, 8, 0));

        painter->setPen(Qt::NoPen);
        painter->setBrush(gradient);
        painter->drawEllipse(QPointF(0.0, 0.0),
                             coreRadius,
                             coreRadius * 0.82);
    }

    // 2. 橙红冲击波，强调爆炸扩散而不是能量拾取
    for (int i = 0; i < 2; ++i) {
        const qreal localT = qBound<qreal>(0.0, (t - i * 0.07) / 0.58, 1.0);
        if (localT <= 0.0 || localT >= 1.0) {
            continue;
        }

        const qreal waveEase = easeOutQuart(localT);
        const qreal waveFade = (1.0 - smoothStep(localT)) * fade * appear;
        const qreal radius = baseRadius * (0.24 + waveEase * (1.32 + i * 0.28));

        QPen pen(
            i == 0
                ? QColor(255, 190, 70, alpha(190.0 * waveFade))
                : QColor(255, 72, 20, alpha(130.0 * waveFade))
        );
        pen.setWidthF(qMax<qreal>(1.2, (4.8 - i * 0.9) * viewScale * (1.0 - localT * 0.45)));
        pen.setCapStyle(Qt::RoundCap);

        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QPointF(0.0, 0.0),
                             radius,
                             radius * 0.72);
    }

    // 3. 爆炸射线：短促、破碎、向外炸开
    {
        const int rayCount = 20;

        for (int i = 0; i < rayCount; ++i) {
            const qreal delay = 0.008 * (i % 6);
            const qreal localT = qBound<qreal>(0.0, (t - delay) / 0.46, 1.0);
            if (localT <= 0.0 || localT >= 1.0) {
                continue;
            }

            const qreal rayFade = (1.0 - smoothStep(localT)) * fade;
            const qreal rayEase = easeOutQuart(localT);
            const qreal angle = impactAngle
                                + kPi
                                + (static_cast<qreal>(i) / rayCount - 0.5) * kPi * 1.62
                                + qSin(i * 2.11) * 0.42;
            const qreal inner = baseRadius * (0.10 + rayEase * 0.18);
            const qreal outer = baseRadius * (0.48 + rayEase * (0.88 + 0.06 * (i % 4)));
            const QPointF p1 = polar(angle, inner, 0.78);
            const QPointF p2 = polar(angle, outer, 0.78);

            QPen pen(
                i % 3 == 0
                    ? QColor(255, 246, 190, alpha(185.0 * rayFade))
                    : (i % 2 == 0
                        ? QColor(255, 150, 38, alpha(165.0 * rayFade))
                        : QColor(255, 74, 18, alpha(130.0 * rayFade)))
            );
            pen.setWidthF(qMax<qreal>(1.0, (3.0 - localT * 1.4) * viewScale));
            pen.setCapStyle(Qt::RoundCap);

            painter->setPen(pen);
            painter->drawLine(p1, p2);
        }
    }

    // 4. 不规则火舌，让中心更像燃烧的飞机残骸
    {
        const int flameCount = 9;

        for (int i = 0; i < flameCount; ++i) {
            const qreal localT = qBound<qreal>(0.0, (t - i * 0.012) / 0.62, 1.0);
            if (localT <= 0.0 || localT >= 1.0) {
                continue;
            }

            const qreal flameFade = (1.0 - smoothStep(localT)) * fade * appear;
            const qreal angle = (static_cast<qreal>(i) / flameCount) * kTwoPi
                                + impactAngle * 0.35
                                + qSin(i * 1.91) * 0.16;
            const qreal flameLength = baseRadius * (0.40 + easeOutCubic(localT) * (0.38 + 0.04 * (i % 3)));
            const qreal rootRadius = baseRadius * (0.10 + 0.018 * (i % 3));
            const QPointF root = polar(angle, baseRadius * 0.13, 0.80);
            const QPointF tip = polar(angle, flameLength, 0.80);
            const QPointF side = perpendicular(QPointF(qCos(angle), qSin(angle))) * rootRadius;

            QPolygonF flame;
            flame << root - side
                  << tip
                  << root + side;

            painter->setPen(Qt::NoPen);
            painter->setBrush(
                i % 2 == 0
                    ? QColor(255, 176, 38, alpha(122.0 * flameFade))
                    : QColor(255, 74, 18, alpha(112.0 * flameFade))
            );
            painter->drawPolygon(flame);
        }
    }

    // 5. 飞机金属碎片，沿撞击反方向和侧向炸开
    painter->setPen(Qt::NoPen);

    {
        const int shardCount = 22;

        for (int i = 0; i < shardCount; ++i) {
            const qreal delay = 0.014 * (i % 7);
            const qreal localT = qBound<qreal>(0.0, (t - delay) / 0.88, 1.0);
            if (localT <= 0.0) {
                continue;
            }

            const qreal shardEase = easeOutCubic(localT);
            const qreal shardFade = (1.0 - smoothStep(localT * 0.92)) * fade;
            if (shardFade <= 0.01) {
                continue;
            }

            const qreal spread = (static_cast<qreal>(i) / shardCount - 0.5) * kPi * 1.55;
            const qreal angle = impactAngle + kPi + spread + qSin(i * 1.73) * 0.28;

            const qreal distance = baseRadius * (
                0.14 + shardEase * (0.84 + 0.08 * (i % 5))
            );

            QPointF pos = polar(angle, distance, 0.72);
            pos += QPointF(0.0, shardEase * shardEase * baseRadius * 0.16);

            const qreal radius = qMax<qreal>(1.8, (3.0 + (i % 4)) * viewScale)
                                 * (1.0 - localT * 0.35);

            const QColor color =
                i % 4 == 0
                    ? QColor(170, 178, 184, alpha(185.0 * shardFade))
                    : (i % 2 == 0
                        ? QColor(255, 132, 36, alpha(215.0 * shardFade))
                        : QColor(94, 96, 104, alpha(165.0 * shardFade)));

            painter->save();
            painter->translate(pos);
            painter->rotate(angle * 180.0 / kPi + t * 720.0 + i * 17.0);
            painter->setBrush(color);
            painter->drawPolygon(makeShard(radius));
            painter->restore();
        }
    }

    // 6. 高温火花，速度比碎片更快、更亮
    {
        const int sparkCount = 38;

        for (int i = 0; i < sparkCount; ++i) {
            const qreal delay = 0.008 * (i % 7);
            const qreal localT = qBound<qreal>(0.0, (t - delay) / 0.68, 1.0);
            if (localT <= 0.0) {
                continue;
            }

            const qreal sparkEase = easeOutQuart(localT);
            const qreal sparkFade = (1.0 - smoothStep(localT)) * fade;
            if (sparkFade <= 0.01) {
                continue;
            }

            const qreal angle = impactAngle
                                + kPi
                                + (static_cast<qreal>(i) / sparkCount - 0.5) * kPi * 1.75
                                + qSin(i * 2.19) * 0.35;

            const qreal dist = baseRadius * (0.18 + sparkEase * (1.05 + 0.10 * (i % 4)));
            const QPointF end = polar(angle, dist, 0.72);
            const QPointF start = end - polar(angle, baseRadius * (0.08 + 0.03 * (i % 3)), 0.72);

            QPen pen(
                i % 2 == 0
                    ? QColor(255, 224, 92, alpha(220.0 * sparkFade))
                    : QColor(255, 92, 24, alpha(180.0 * sparkFade))
            );
            pen.setWidthF(qMax<qreal>(1.0, 1.8 * viewScale * (1.0 - localT * 0.25)));
            pen.setCapStyle(Qt::RoundCap);

            painter->setPen(pen);
            painter->drawLine(start, end);
        }
    }

    // 7. 中心白热爆闪
    {
        if (flash > 0.0) {
            QPen pen(QColor(255, 248, 218, alpha(235.0 * flash * appear)));
            pen.setWidthF(qMax<qreal>(1.5, 3.2 * viewScale));
            pen.setCapStyle(Qt::RoundCap);

            painter->setPen(pen);

            const qreal lenA = baseRadius * (0.26 + flash * 0.28);
            const qreal lenB = baseRadius * (0.18 + flash * 0.20);

            painter->drawLine(-dir * lenA, dir * lenA);
            painter->drawLine(-perp * lenB, perp * lenB);
        }
    }

    // 烟尘和暗部用普通混合，避免被 Plus 模式吞掉
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);

    // 8. 黑灰烟尘，后半段盖住火光，表现毁灭后的残烟
    {
        const qreal smokeT = qBound<qreal>(0.0, (t - 0.08) / 0.92, 1.0);
        const qreal smokeFade = smoothStep(smokeT) * (1.0 - smoothStep((t - 0.74) / 0.26));

        painter->setPen(Qt::NoPen);

        for (int i = 0; i < 12; ++i) {
            const qreal angle = impactAngle + kPi
                                + (static_cast<qreal>(i) / 12.0 - 0.5) * kPi * 1.26
                                + qSin(i * 1.41) * 0.22;

            const qreal distance = baseRadius * (0.08 + smokeT * (0.48 + 0.04 * (i % 3)));
            const QPointF pos = polar(angle, distance, 0.70)
                                + QPointF(0.0, smokeT * baseRadius * 0.12);

            const qreal rx = baseRadius * (0.11 + smokeT * 0.18 + 0.014 * (i % 3));
            const qreal ry = rx * (0.60 + 0.10 * (i % 2));

            painter->setBrush(QColor(28, 29, 32, alpha(118.0 * smokeFade * fade)));
            painter->drawEllipse(pos, rx, ry);
        }
    }

    // 9. 暗红余烬，替代原来的护盾裂纹收尾
    {
        const qreal emberFade = (1.0 - smoothStep((t - 0.20) / 0.76)) * fade;

        if (emberFade > 0.0) {
            painter->setPen(Qt::NoPen);

            const int emberCount = 10;
            for (int i = 0; i < emberCount; ++i) {
                const qreal localT = qBound<qreal>(0.0, (t - i * 0.02) / 0.80, 1.0);
                if (localT <= 0.0) {
                    continue;
                }

                const qreal angle = impactAngle + kPi
                                    + (static_cast<qreal>(i) / emberCount - 0.5) * kPi
                                    + qSin(i * 1.77) * 0.22;
                const qreal distance = baseRadius * (0.18 + easeOutCubic(localT) * (0.48 + 0.04 * (i % 3)));
                const qreal radius = qMax<qreal>(1.4, (2.0 + (i % 3)) * viewScale)
                                     * (1.0 - localT * 0.38);

                painter->setBrush(QColor(255, 82, 22, alpha(120.0 * emberFade * (1.0 - smoothStep(localT)))));
                painter->drawEllipse(polar(angle, distance, 0.76), radius, radius * 0.72);
            }
        }
    }

    painter->restore();
}
