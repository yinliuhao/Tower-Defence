#include "_bullet/areadamagebullet.h"
#include "_monster/monster.h"
#include "_others/global.h"
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

void AreaDamageBullet::takeDamage()
{
    // Bullet 基类已经做过 hasHit / dying / target 判定
    if (!target) {
        destroySelf();
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

    destroySelf();
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

    // 为了避免访问可能已悬空的 grid 单元，改为遍历 monsterSpawner->monsters 的副本
    std::vector<Monster*> all = monsterSpawner->monsters;
    for (Monster* m : all) {
        if (!m) continue;
        QPointer<Monster> safeM(m);
        if (!safeM) continue;
        if (safeM->isDead() || safeM->isDying()) continue;

        if (center.distanceTo(safeM->getPosition()) <= radius) {
            result.push_back(safeM);
        }
    }

    return result;
}
