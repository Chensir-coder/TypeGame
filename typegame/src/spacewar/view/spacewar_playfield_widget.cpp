#include "spacewar_playfield_widget.h"

#include <QDateTime>
#include <QDebug>
#include <QFont>
#include <QFontMetrics>
#include <QLinearGradient>
#include <QPainter>
#include <QPaintEvent>
#include <QRadialGradient>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QString>
#include <QtGlobal>

#include <cmath>
#include <memory>

#include "app/resources/asset_paths.h"
#include "common/core/game_state.h"
#include "common/service/config/configure_settings.h"
#include "common/typing/typing_target_entity.h"
#include "common/view/effect/enemy_player_collision_effect.h"
#include "common/view/effect/effectitem.h"
#include "common/view/effect/reward_expire_effect.h"
#include "common/view/effect/ui_notice/difficulty_notice_effect.h"
#include "common/view/util/pixmapslicehelper.h"
#include "spacewar/model/entity/bullet_entity.h"
#include "spacewar/model/entity/enemy_entity.h"
#include "spacewar/model/entity/player_entity.h"
#include "spacewar/model/entity/reward_word_entity.h"
#include "spacewar/model/spacewar_model.h"

namespace
{
constexpr int kExplosionAtlasCols = 3;
constexpr int kExplosionAtlasRows = 3;

int spriteFramePixmapIndex(int entity_frame_index, int entity_frame_count, int pixmap_count)
{
    if (pixmap_count <= 0) {
        return 0;
    }
    const int period = (entity_frame_count > 0) ? entity_frame_count : pixmap_count;
    int i = entity_frame_index % period;
    if (i < 0) {
        i += period;
    }
    return i % pixmap_count;
}

QRect pixmapRectFromLogicalRect(const QRectF& logical_rect, qreal scale_x, qreal scale_y)
{
    return QRect(
        qRound(logical_rect.left() * scale_x),
        qRound(logical_rect.top() * scale_y),
        qMax(1, qRound(logical_rect.width() * scale_x)),
        qMax(1, qRound(logical_rect.height() * scale_y)));
}

} // namespace

SpaceWarPlayfieldWidget::SpaceWarPlayfieldWidget(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
    setMinimumHeight(ConfigureSettings::instance().spacewar().ui.playfieldMinHeight);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    reloadAssets();
    resampleBackgroundScrollSpeed();
}

void SpaceWarPlayfieldWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    m_backgroundTile = QPixmap();
    m_backgroundTileCacheWidth = 0;
}

void SpaceWarPlayfieldWidget::setGameModel(SpaceWarModel* model)
{
    if (m_model == model) {
        return;
    }
    m_model = model;
    m_effects.clear();
    update();
}

void SpaceWarPlayfieldWidget::tick(int delta_ms)
{
    advanceAnimationMs(delta_ms);
    update();
}

void SpaceWarPlayfieldWidget::advanceAnimationMs(int delta_ms)
{
    if (delta_ms > 0) {
        advanceBackgroundScrollMs(delta_ms);
    }
    if (delta_ms > 0 && !m_effects.isEmpty()) {
        m_effects.update(static_cast<qreal>(delta_ms) / 1000.0);
    }
}

void SpaceWarPlayfieldWidget::reloadAssets()
{
    const auto& ss = ConfigureSettings::instance().spacewar().spriteSheets;

    m_background.load(UiAssetPaths::SpaceWar::background());
    m_backgroundTile = QPixmap();
    m_backgroundTileCacheWidth = 0;

    m_enemy0_sheet.load(UiAssetPaths::SpaceWar::enemy0());
    m_enemy4_sheet.load(UiAssetPaths::SpaceWar::enemy4());
    if (!m_enemy0_sheet.isNull()) {
        m_enemy0_frames =
            PixmapSliceHelper::slice(m_enemy0_sheet, ss.enemyCols, ss.enemyRows);
        if (m_enemy0_frames.isEmpty()) {
            m_enemy0_frames.push_back(m_enemy0_sheet);
        }
    }
    if (!m_enemy4_sheet.isNull()) {
        m_enemy4_frames =
            PixmapSliceHelper::slice(m_enemy4_sheet, ss.enemyCols, ss.enemyRows);
        if (m_enemy4_frames.isEmpty()) {
            m_enemy4_frames.push_back(m_enemy4_sheet);
        }
    }

    m_ship_sheet.load(UiAssetPaths::SpaceWar::ship());
    if (!m_ship_sheet.isNull()) {
        m_ship_frames =
            PixmapSliceHelper::slice(m_ship_sheet, ss.playerCols, ss.playerRows);
        if (m_ship_frames.isEmpty()) {
            m_ship_frames.push_back(m_ship_sheet);
        }
    }

    m_bullet_pix.load(UiAssetPaths::SpaceWar::bomb());

    m_reward_sheet.load(UiAssetPaths::SpaceWar::reward());
    if (!m_reward_sheet.isNull()) {
        m_reward_frames = PixmapSliceHelper::slice(
            m_reward_sheet, ss.rewardCols, ss.rewardRows);
        if (m_reward_frames.isEmpty()) {
            m_reward_frames.push_back(m_reward_sheet);
        }
    }

    m_explosion_sheet.load(UiAssetPaths::SpaceWar::explosion0());
    m_explosion_frames.clear();
    if (!m_explosion_sheet.isNull()) {
        /* 爆炸图集固定 3×3 共 9 帧，1 秒内播完由 EffectItem 的 lifeTime / frameInterval 控制 */
        m_explosion_frames = PixmapSliceHelper::slice(
            m_explosion_sheet, kExplosionAtlasCols, kExplosionAtlasRows);
        if (m_explosion_frames.isEmpty()) {
            m_explosion_frames.push_back(m_explosion_sheet);
        }
    }
}

void SpaceWarPlayfieldWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QElapsedTimer paintTimer;
    paintTimer.start();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    painter.setRenderHint(QPainter::TextAntialiasing, true);

    drawBackground(painter);

    if (m_model == nullptr) {
        drawShip(painter, 1.0, 1.0);
        logPaintCost(paintTimer.elapsed());
        return;
    }

    const qreal scene_w = qMax<qreal>(1.0, static_cast<qreal>(m_model->config().sceneWidth()));
    const qreal scene_h = qMax<qreal>(1.0, static_cast<qreal>(m_model->config().sceneHeight()));
    const qreal scale_x = static_cast<qreal>(width()) / scene_w;
    const qreal scale_y = static_cast<qreal>(height()) / scene_h;

    drawEnemies(painter, scale_x, scale_y);
    drawBullets(painter, scale_x, scale_y);
    drawReward(painter, scale_x, scale_y);
    drawExplosionEffects(painter, scale_x, scale_y);
    drawShip(painter, scale_x, scale_y);
    drawHud(painter);

    if (m_model->gameState() == GameState::Paused) {
        drawPausedOverlay(painter);
    } else if (m_model->gameState() == GameState::End) {
        drawGameOverOverlay(painter);
    }

    logPaintCost(paintTimer.elapsed());
}

void SpaceWarPlayfieldWidget::logPaintCost(qint64 paint_elapsed_ms)
{
    if (!m_paint_perf_timer.isValid()) {
        m_paint_perf_timer.start();
    }

    m_paint_total_ms += paint_elapsed_ms;
    m_paint_max_ms = qMax(m_paint_max_ms, paint_elapsed_ms);
    ++m_paint_frame_count;

    if (paint_elapsed_ms >= 20) {
        qDebug() << "[spacewar perf] slow paint"
                 << "paintMs=" << paint_elapsed_ms
                 << "widgetSize=" << size();
    }

    if (m_paint_perf_timer.elapsed() >= 1000 && m_paint_frame_count > 0) {
        qDebug() << "[spacewar perf] paint summary"
                 << "paintCalls=" << m_paint_frame_count
                 << "avgPaintMs="
                 << (static_cast<double>(m_paint_total_ms) / m_paint_frame_count)
                 << "maxPaintMs=" << m_paint_max_ms
                 << "widgetSize=" << size();

        m_paint_perf_timer.restart();
        m_paint_total_ms = 0;
        m_paint_max_ms = 0;
        m_paint_frame_count = 0;
    }
}

void SpaceWarPlayfieldWidget::rebuildBackgroundCache()
{
    if (m_background.isNull() || width() <= 0) {
        m_backgroundTile = QPixmap();
        m_backgroundTileCacheWidth = 0;
        return;
    }

    if (!m_backgroundTile.isNull() && m_backgroundTileCacheWidth == width()) {
        return;
    }

    m_backgroundTile = m_background.scaledToWidth(width(), Qt::SmoothTransformation);
    m_backgroundTileCacheWidth = width();
}

void SpaceWarPlayfieldWidget::resampleBackgroundScrollSpeed()
{
    QRandomGenerator* rng = QRandomGenerator::global();
    m_bgSpeedPps = static_cast<qreal>(rng->bounded(32, 88));
    m_bgSpeedResampleMsRemaining += rng->bounded(2200, 5200);
}

void SpaceWarPlayfieldWidget::advanceBackgroundScrollMs(int delta_ms)
{
    if (m_background.isNull() || delta_ms <= 0) {
        return;
    }

    m_bgSpeedResampleMsRemaining -= delta_ms;
    while (m_bgSpeedResampleMsRemaining <= 0) {
        resampleBackgroundScrollSpeed();
    }

    m_bgScrollPx += m_bgSpeedPps * static_cast<qreal>(delta_ms) / 1000.0;
}

void SpaceWarPlayfieldWidget::drawBackground(QPainter& painter)
{
    if (!m_background.isNull()) {
        rebuildBackgroundCache();
        const int tile_h = m_backgroundTile.height();
        if (!m_backgroundTile.isNull() && tile_h > 0) {
            const qreal th = static_cast<qreal>(tile_h);
            qreal phase = std::fmod(m_bgScrollPx, th);
            if (phase < 0.0) {
                phase += th;
            }
            qreal y = phase - th;
            const qreal bottom = static_cast<qreal>(height()) + th;
            while (y < bottom) {
                painter.drawPixmap(0, qRound(y), m_backgroundTile);
                y += th;
            }
        }
    } else {
        QLinearGradient grad(0, 0, 0, height());
        grad.setColorAt(0.0, QColor(7, 11, 32));
        grad.setColorAt(1.0, QColor(20, 26, 70));
        painter.fillRect(rect(), grad);
    }
}

void SpaceWarPlayfieldWidget::drawEnemies(QPainter& painter, qreal scale_x, qreal scale_y)
{
    for (TypingTargetEntity* t : m_model->activeTargets()) {
        if (t == nullptr || !t->isActive() || !t->isVisible()) {
            continue;
        }
        EnemyEntity* enemy = static_cast<EnemyEntity*>(t);

        const QRectF lr = enemy->boundingRect();
        QRect pr = pixmapRectFromLogicalRect(lr, scale_x, scale_y);
        if (pr.isEmpty()) {
            continue;
        }

        const QVector<QPixmap>& frames = (enemy->variant() == EnemyEntity::Variant::Type4)
            ? m_enemy4_frames
            : m_enemy0_frames;

        QPixmap frame;
        if (!frames.isEmpty()) {
            const int idx = spriteFramePixmapIndex(
                enemy->spriteAnimationFrameIndex(),
                enemy->spriteAnimationFrameCount(),
                frames.size());
            frame = frames.at(qBound(0, idx, frames.size() - 1));
        }

        if (!frame.isNull()) {
            painter.drawPixmap(pr, frame);
        } else {
            painter.setBrush(QColor(220, 96, 96, 210));
            painter.setPen(QPen(QColor(255, 200, 200), 2));
            painter.drawEllipse(pr);
        }

        if (enemy->isLocked()) {
            QPen pen(QColor(255, 230, 80, 220));
            pen.setWidth(qMax(2, qRound(3.0 * scale_x)));
            painter.setPen(pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawRoundedRect(pr.adjusted(2, 2, -2, -2), 8, 8);
        }

        const QChar letter = enemy->letter();
        if (!letter.isNull()) {
            QFont font = painter.font();
            font.setFamily(QStringLiteral("Segoe UI"));
            font.setBold(true);
            const int side = qMin(pr.width(), pr.height());
            font.setPixelSize(qBound(16, side * 50 / 100, 60));
            painter.setFont(font);

            // Draw a hanging pendant instead of painting text directly on the aircraft.
            // This keeps the sprite details visible while preserving readability.
            const int badge_side = qBound(20, side * 58 / 100, 50);
            const int strap_len = qBound(6, side * 20 / 100, 16);
            const int gap = qBound(2, side * 6 / 100, 6);
            const int desired_top = pr.bottom() + gap + strap_len;
            const bool hang_upward = (desired_top + badge_side >= height() - 2);

            QPoint anchor(pr.center().x(),
                          hang_upward ? (pr.top() - gap) : (pr.bottom() + gap));
            QPoint pendant_center(
                anchor.x(),
                hang_upward ? (anchor.y() - strap_len - badge_side / 2)
                            : (anchor.y() + strap_len + badge_side / 2));
            QRect badge_rect(
                pendant_center.x() - badge_side / 2,
                pendant_center.y() - badge_side / 2,
                badge_side,
                badge_side);

            const qreal pulse = 0.5
                + 0.5 * std::sin(static_cast<qreal>(enemy->spriteAnimationFrameIndex()) * 0.5);
            const int badge_alpha = enemy->isLocked()
                ? qRound(168.0 + pulse * 52.0)
                : qRound(136.0 + pulse * 36.0);

            QPen strap_pen(enemy->isLocked() ? QColor(255, 228, 120, 225)
                                             : QColor(160, 210, 255, 205),
                           qMax(1, qRound(scale_x * 1.4)));
            painter.setPen(strap_pen);
            painter.setBrush(Qt::NoBrush);
            painter.drawLine(anchor,
                             QPoint(anchor.x(), hang_upward ? badge_rect.bottom() : badge_rect.top()));
            painter.setBrush(enemy->isLocked() ? QColor(255, 222, 110, 220)
                                               : QColor(170, 220, 255, 180));
            painter.drawEllipse(anchor, qMax(1, badge_side / 12), qMax(1, badge_side / 12));

            QLinearGradient badge_grad(
                badge_rect.left(), badge_rect.top(), badge_rect.left(), badge_rect.bottom());
            if (enemy->isLocked()) {
                badge_grad.setColorAt(0.0, QColor(115, 90, 28, badge_alpha));
                badge_grad.setColorAt(1.0, QColor(62, 48, 16, badge_alpha));
            } else {
                badge_grad.setColorAt(0.0, QColor(42, 90, 170, badge_alpha));
                badge_grad.setColorAt(1.0, QColor(24, 44, 98, badge_alpha));
            }
            painter.setBrush(badge_grad);
            painter.setPen(QPen(QColor(255, 255, 255, enemy->isLocked() ? 148 : 120), 1));
            painter.drawRoundedRect(badge_rect, 7, 7);

            QLinearGradient gloss(badge_rect.left(), badge_rect.top(),
                                  badge_rect.left(), badge_rect.center().y());
            gloss.setColorAt(0.0, QColor(255, 255, 255, enemy->isLocked() ? 90 : 72));
            gloss.setColorAt(1.0, QColor(255, 255, 255, 0));
            painter.setBrush(gloss);
            painter.setPen(Qt::NoPen);
            painter.drawRoundedRect(
                badge_rect.adjusted(2, 2, -2, -(badge_rect.height() / 2 + 1)), 6, 6);

            const QString letter_str(letter);
            const QColor stroke = enemy->isLocked()
                ? QColor(36, 18, 0, 235)
                : QColor(0, 10, 36, 220);
            painter.setPen(stroke);
            painter.drawText(badge_rect.translated(-1, 0), Qt::AlignCenter, letter_str);
            painter.drawText(badge_rect.translated(1, 0), Qt::AlignCenter, letter_str);
            painter.drawText(badge_rect.translated(0, -1), Qt::AlignCenter, letter_str);
            painter.drawText(badge_rect.translated(0, 1), Qt::AlignCenter, letter_str);

            const QColor fill = enemy->isLocked() ? QColor(255, 247, 142) : QColor(236, 244, 255);
            painter.setPen(fill);
            painter.drawText(badge_rect, Qt::AlignCenter, letter_str);
        }
    }
}

void SpaceWarPlayfieldWidget::drawBullets(QPainter& painter, qreal scale_x, qreal scale_y)
{
    for (BulletEntity* bullet : m_model->activeBullets()) {
        if (bullet == nullptr || !bullet->isActive() || !bullet->isVisible()) {
            continue;
        }

        const QRectF lr = bullet->boundingRect();
        QRect pr = pixmapRectFromLogicalRect(lr, scale_x, scale_y);
        if (pr.isEmpty()) {
            continue;
        }

        if (!m_bullet_pix.isNull()) {
            painter.drawPixmap(pr, m_bullet_pix);
        } else {
            painter.setBrush(QColor(255, 220, 100, 230));
            painter.setPen(QPen(QColor(255, 255, 200), 2));
            painter.drawEllipse(pr);
        }
    }
}

void SpaceWarPlayfieldWidget::drawReward(QPainter& painter, qreal scale_x, qreal scale_y)
{
    RewardWordEntity* reward = m_model->activeReward();
    if (reward == nullptr || !reward->isActive() || !reward->isVisible()) {
        return;
    }

    const QRectF lr = reward->boundingRect();
    QRect pr = pixmapRectFromLogicalRect(lr, scale_x, scale_y);
    if (pr.isEmpty()) {
        return;
    }

    QPixmap frame;
    if (!m_reward_frames.isEmpty()) {
        const qreal t = static_cast<qreal>(QDateTime::currentMSecsSinceEpoch() % 1000) / 1000.0;
        const int frameIndex =
            qBound(0, static_cast<int>(t * m_reward_frames.size()),
                   m_reward_frames.size() - 1);
        Q_UNUSED(frameIndex);
        frame = m_reward_frames.first();
    }

    if (!frame.isNull()) {
        painter.drawPixmap(pr, frame);
    } else {
        painter.setBrush(QColor(120, 220, 255, 200));
        painter.setPen(QPen(QColor(220, 255, 255), 2));
        painter.drawRoundedRect(pr, 14, 14);
    }

    const QString word = reward->word().toLower();
    const QString typed = reward->typedPart().toLower();
    const QString remain = word.mid(typed.size());

    QFont font = painter.font();
    font.setFamily(QStringLiteral("Segoe UI"));
    font.setBold(true);
    const int fs =
        qBound(14, qMin(pr.height() / 3, qMax(12, pr.width() / qMax(1, word.size() + 2))), 36);
    font.setPixelSize(fs);
    painter.setFont(font);

    const QFontMetrics fm(font);
    const int wordW = fm.horizontalAdvance(word);
    const int wordH = fm.height();

    const int panelPadX = qBound(10, pr.width() / 7, 18);
    const int panelPadY = qBound(6, pr.height() / 8, 12);
    const int panelGap = qBound(6, pr.height() / 10, 14);
    const int panelW = wordW + panelPadX * 2;
    const int panelH = wordH + panelPadY * 2 + 8;
    QRect panel(pr.center().x() - panelW / 2, pr.bottom() + panelGap, panelW, panelH);
    if (panel.bottom() >= height() - 2) {
        panel.moveTop(pr.top() - panelGap - panelH);
    }
    panel = panel.intersected(rect().adjusted(2, 2, -2, -2));
    if (panel.isEmpty()) {
        return;
    }

    QLinearGradient panelGrad(panel.left(), panel.top(), panel.left(), panel.bottom());
    panelGrad.setColorAt(0.0, QColor(20, 34, 70, 210));
    panelGrad.setColorAt(1.0, QColor(8, 14, 34, 194));
    painter.setBrush(panelGrad);
    painter.setPen(QPen(QColor(150, 230, 255, 165), 1));
    painter.drawRoundedRect(panel, 10, 10);

    const QRect panelTopGlow = panel.adjusted(2, 2, -2, -(panel.height() * 55 / 100));
    QLinearGradient glow(panelTopGlow.left(), panelTopGlow.top(),
                         panelTopGlow.left(), panelTopGlow.bottom());
    glow.setColorAt(0.0, QColor(255, 255, 255, 62));
    glow.setColorAt(1.0, QColor(255, 255, 255, 0));
    painter.setPen(Qt::NoPen);
    painter.setBrush(glow);
    painter.drawRoundedRect(panelTopGlow, 8, 8);

    const int progress_h = qBound(3, fs / 6, 6);
    QRect progressBg(panel.left() + panelPadX,
                     panel.bottom() - panelPadY - progress_h,
                     wordW, progress_h);
    painter.setBrush(QColor(255, 255, 255, 42));
    painter.drawRoundedRect(progressBg, progress_h / 2.0, progress_h / 2.0);

    const qreal typedRatio = word.isEmpty()
        ? 0.0
        : qBound<qreal>(0.0, static_cast<qreal>(typed.size()) / word.size(), 1.0);
    QRect progressFill = progressBg;
    progressFill.setWidth(qRound(progressBg.width() * typedRatio));
    if (progressFill.width() > 0) {
        QLinearGradient progressGrad(progressFill.left(), progressFill.top(),
                                     progressFill.right(), progressFill.top());
        progressGrad.setColorAt(0.0, QColor(255, 210, 88, 228));
        progressGrad.setColorAt(1.0, QColor(255, 128, 68, 228));
        painter.setBrush(progressGrad);
        painter.drawRoundedRect(progressFill, progress_h / 2.0, progress_h / 2.0);
    }

    const int textX = panel.left() + panelPadX;
    const int textY = panel.top() + panelPadY;
    QRect typedRect(textX, textY, fm.horizontalAdvance(typed), wordH);
    QRect remainRect(textX + fm.horizontalAdvance(typed), textY,
                     fm.horizontalAdvance(remain), wordH);

    painter.setPen(QColor(0, 0, 0, 190));
    painter.drawText(typedRect.translated(1, 1), Qt::AlignLeft | Qt::AlignVCenter, typed);
    painter.drawText(remainRect.translated(1, 1), Qt::AlignLeft | Qt::AlignVCenter, remain);

    painter.setPen(QColor(255, 232, 115));
    painter.drawText(typedRect, Qt::AlignLeft | Qt::AlignVCenter, typed);
    painter.setPen(QColor(208, 235, 255));
    painter.drawText(remainRect, Qt::AlignLeft | Qt::AlignVCenter, remain);
}

void SpaceWarPlayfieldWidget::drawExplosionEffects(QPainter& painter,
                                                   qreal scale_x,
                                                   qreal scale_y)
{
    if (m_effects.isEmpty()) {
        return;
    }

    const qreal view_scale = (scale_x + scale_y) * 0.5;
    m_effects.paint(&painter, view_scale);
}

void SpaceWarPlayfieldWidget::onEnemyHitByBullet(QChar letter)
{
    if (m_model == nullptr || letter.isNull() || m_explosion_frames.isEmpty()) {
        return;
    }

    const QChar key = letter.toUpper();
    EnemyEntity* chosen = nullptr;
    for (TypingTargetEntity* t : m_model->activeTargets()) {
        if (t == nullptr) {
            continue;
        }
        auto* enemy = static_cast<EnemyEntity*>(t);
        if (!enemy->isActive()) {
            continue;
        }
        if (enemy->letter().toUpper() != key) {
            continue;
        }
        if (enemy->isLocked()) {
            chosen = enemy;
            break;
        }
        if (chosen == nullptr) {
            chosen = enemy;
        }
    }

    if (chosen == nullptr) {
        return;
    }

    const QRectF br = chosen->boundingRect();
    const QPointF center = br.center();
    const QSizeF logical_size = br.size();

    const int n = m_explosion_frames.size();
    const qreal frame_interval =
        (n > 0) ? (1.0 / static_cast<qreal>(n)) : 1.0;

    auto effect = std::make_shared<EffectItem>(center,
                                                logical_size,
                                                m_explosion_frames,
                                                1.0,
                                                frame_interval,
                                                false);
    m_effects.addEffect(std::move(effect));
    update();
}

void SpaceWarPlayfieldWidget::onRewardExpired(const QPointF& center,
                                              const QSizeF& size,
                                              const QString& word)
{
    if (size.isEmpty()) {
        return;
    }

    auto effect = std::make_shared<RewardExpireEffect>(center, size, word);
    m_effects.addEffect(std::move(effect));
    update();
}

void SpaceWarPlayfieldWidget::onEnemyCollidedWithPlayer(const QPointF& enemyCenter,
                                                        const QSizeF& enemySize,
                                                        EnemyPlayerCollisionReason reason)
{
    if (enemySize.isEmpty()) {
        return;
    }

    auto effect = std::make_shared<EnemyPlayerCollisionEffect>(enemyCenter, enemySize);
    m_effects.addEffect(std::move(effect));

    if (reason == EnemyPlayerCollisionReason::BoundingOverlap && m_model != nullptr
        && m_model->player() != nullptr) {
        m_model->player()->triggerCollisionShieldFx();
    }
    update();
}

void SpaceWarPlayfieldWidget::onRewardCompleted(const QPointF& center,
                                                const QSizeF& size,
                                                const QString& word)
{
    if (size.isEmpty()) {
        return;
    }

    auto effect = std::make_shared<RewardExpireEffect>(center, size, word);
    m_effects.addEffect(std::move(effect));
    update();
}

void SpaceWarPlayfieldWidget::onDifficultyUpgraded(int level, int maxLevel)
{
    if (m_model == nullptr) {
        return;
    }

    const qreal scene_w =
        qMax<qreal>(1.0, static_cast<qreal>(m_model->config().sceneWidth()));
    const qreal scene_h =
        qMax<qreal>(1.0, static_cast<qreal>(m_model->config().sceneHeight()));

    const QPointF center(scene_w * 0.5, scene_h * 0.28);
    const QSizeF size(scene_w * 0.5, scene_h * 0.1);
    const QString message =
        QStringLiteral("难度升级 L%1/%2").arg(level).arg(maxLevel);

    auto effect = std::make_shared<DifficultyNoticeEffect>(center, size, message);
    m_effects.addEffect(std::move(effect));
    update();
}

void SpaceWarPlayfieldWidget::drawShip(QPainter& painter, qreal scale_x, qreal scale_y)
{
    PlayerEntity* player = (m_model != nullptr) ? m_model->player() : nullptr;
    if (player == nullptr) {
        return;
    }

    const QRectF lr = player->boundingRect();
    QRect pr = pixmapRectFromLogicalRect(lr, scale_x, scale_y);
    if (pr.isEmpty()) {
        return;
    }

    QPixmap frame;
    if (!m_ship_frames.isEmpty()) {
        const int idx = spriteFramePixmapIndex(
            player->currentFrameIndex(),
            player->frameCount(),
            m_ship_frames.size());
        frame = m_ship_frames.at(qBound(0, idx, m_ship_frames.size() - 1));
    } else if (!m_ship_sheet.isNull()) {
        frame = m_ship_sheet;
    }

    if (!frame.isNull()) {
        painter.drawPixmap(pr, frame);
    } else {
        painter.setBrush(QColor(120, 200, 255, 220));
        painter.setPen(QPen(QColor(220, 240, 255), 2));
        painter.drawRect(pr);
    }
    const qreal view_scale = (scale_x + scale_y) * 0.5;
    player->paint(&painter, QRectF(pr), view_scale);
}

void SpaceWarPlayfieldWidget::drawHud(QPainter& painter)
{
    if (m_model == nullptr || m_model->player() == nullptr) {
        return;
    }

    const int hp = m_model->playerHealth();
    const int maxHp = m_model->playerMaxHealth();
    const int score = m_model->score();
    const int difficulty = m_model->difficultyLevel();

    /* 顶部条 */
    QRect bar(12, 12, qMin(width() - 24, 520), 56);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 140));
    painter.drawRoundedRect(bar, 12, 12);

    QFont f = painter.font();
    f.setBold(true);
    f.setPixelSize(18);
    painter.setFont(f);
    painter.setPen(QColor(220, 240, 255));

    const QString stateText = (m_model->gameState() == GameState::Playing)
        ? QStringLiteral("PLAYING")
        : (m_model->gameState() == GameState::Paused
               ? QStringLiteral("PAUSED")
               : (m_model->gameState() == GameState::End ? QStringLiteral("END")
                                                        : QStringLiteral("READY")));

    painter.drawText(bar.adjusted(16, 0, -16, 0),
                     Qt::AlignVCenter | Qt::AlignLeft,
                     QStringLiteral("SCORE %1   LV %2   %3")
                         .arg(score, 5, 10, QChar('0'))
                         .arg(difficulty)
                         .arg(stateText));

    /* HP 条 */
    QRect hpBg(width() - 320, 12, 300, 30);
    painter.setBrush(QColor(0, 0, 0, 140));
    painter.drawRoundedRect(hpBg, 8, 8);

    const int innerPadding = 4;
    QRect hpInner = hpBg.adjusted(innerPadding, innerPadding, -innerPadding, -innerPadding);
    painter.setBrush(QColor(60, 80, 120, 200));
    painter.drawRoundedRect(hpInner, 6, 6);

    if (maxHp > 0) {
        const qreal ratio = static_cast<qreal>(hp) / static_cast<qreal>(maxHp);
        QRect hpFill(hpInner.left(), hpInner.top(),
                     qRound(hpInner.width() * qBound<qreal>(0.0, ratio, 1.0)),
                     hpInner.height());

        QColor color;
        if (ratio > 0.5) {
            color = QColor(80, 220, 120);
        } else if (ratio > 0.25) {
            color = QColor(255, 200, 80);
        } else {
            color = QColor(240, 90, 90);
        }

        painter.setBrush(color);
        painter.drawRoundedRect(hpFill, 6, 6);
    }

    QFont hpFont = painter.font();
    hpFont.setPixelSize(16);
    painter.setFont(hpFont);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(hpBg, Qt::AlignCenter,
                     QStringLiteral("HP  %1 / %2").arg(hp).arg(maxHp));
}

void SpaceWarPlayfieldWidget::drawPausedOverlay(QPainter& painter)
{
    painter.fillRect(rect(), QColor(0, 0, 0, 110));

    QFont f = painter.font();
    f.setBold(true);
    f.setPixelSize(64);
    painter.setFont(f);
    painter.setPen(QColor(255, 255, 255));
    painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("PAUSED"));
}

void SpaceWarPlayfieldWidget::drawGameOverOverlay(QPainter& painter)
{
    painter.fillRect(rect(), QColor(0, 0, 0, 130));

    QFont f = painter.font();
    f.setBold(true);
    f.setPixelSize(56);
    painter.setFont(f);
    painter.setPen(QColor(255, 200, 80));
    painter.drawText(rect(), Qt::AlignCenter, QStringLiteral("GAME OVER"));
}
