#include "areadamagebullet.h"
#include "monster.h"
#include "global.h"
#include <cmath>

AreaDamageBullet::AreaDamageBullet(const Vector2& startPos,
                                   Monster* target,
                                   TowerType towerType,
                                   int towerLevel,
                                   float speed,
                                   float damage,
                                   float radius,
                                   QGraphicsItem* parent)
    : Bullet(startPos, target, towerType, towerLevel, speed, damage, parent),
    areaRadius_(radius)
{
}

void AreaDamageBullet::hitTarget()
{
    // Bullet 基类已经做过 hasHit / dying / target 判定
    if (!target) {
        Bullet::hitTarget();
        return;
    }

    const Vector2 explosionCenter = position_;
    const QVector<Monster*> monsters =
        findMonstersInArea(explosionCenter, areaRadius_);

    for (Monster* monster : monsters) {
        if (!monster || monster->isDead()) continue;

        float dist = explosionCenter.distanceTo(monster->getPosition());
        if (dist > areaRadius_) continue;

        float factor = 1.0f - dist / areaRadius_;
        factor = std::max(0.1f, factor);   // 最低 10%

        monster->takeDamage(damage_ * factor);
    }

    // 🔑 把“子弹后续生命周期”交还给基类
    Bullet::hitTarget();
}

QVector<Monster*> AreaDamageBullet::findMonstersInArea(const Vector2& center,
                                                        float radius)
{
    QVector<Monster*> result;
    if (!monsterSpawner || !gMap) return result;

    QPoint gridPos = gMap->pixelToGrid(QPointF(center.x, center.y));
    int cx = gridPos.x();
    int cy = gridPos.y();

    int gridRadius = static_cast<int>(radius / TILESIZE) + 1;

    for (int dx = -gridRadius; dx <= gridRadius; ++dx) {
        for (int dy = -gridRadius; dy <= gridRadius; ++dy) {

            int gx = cx + dx;
            int gy = cy + dy;

            if (gx < 0 || gx >= 60 || gy < 0 || gy >= 90)
                continue;

            const auto& cell = monsterSpawner->grid[gx][gy];
            for (Monster* m : cell) {
                if (!m || m->isDead()) continue;

                if (center.distanceTo(m->getPosition()) <= radius)
                    result.push_back(m);
            }
        }
    }

    return result;
}
