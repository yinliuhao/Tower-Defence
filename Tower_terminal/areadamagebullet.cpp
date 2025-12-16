#include "areadamagebullet.h"
#include "monster.h"
#include "global.h"
#include <QDebug>
#include <cmath>
#include <QGraphicsScene>  // 添加包含

AreaDamageBullet::AreaDamageBullet(const Vector2& startPos, Monster* target, TowerType towerType,
                 int towerLevel, float speed, float damage, float radius, QGraphicsItem* parent):
    Bullet(startPos, target, towerType, towerLevel, speed, damage, parent)
{
    areaRadius_ = radius;
}


void AreaDamageBullet::hitTarget()
{
    hasHit_ = true;
    active_ = false;

    if (updateTimer_ && updateTimer_->isActive()) {
        updateTimer_->stop();
    }

    Vector2 explosionCenter = getTarget()->getPosition();
    QVector<Monster*> monstersInArea = findMonstersInArea(explosionCenter, areaRadius_);

    for (Monster* monster : monstersInArea) {
        if (monster && !monster->isDead()) {
            float distance = explosionCenter.distanceTo(monster->getPosition());
            float damageMultiplier = 1.0f - (distance / areaRadius_);
            damageMultiplier = qMax(0.1f, damageMultiplier);

            float actualDamage = getDamage() * damageMultiplier;
            monster->takeDamage(actualDamage);

            qDebug() << "范围伤害：对怪物造成" << actualDamage << "伤害，距离：" << distance;
        }
    }
    startEffect();
    // 调用基类的命中处理
    Bullet::hitTarget();  // 调用基类实现
}

QVector<Monster*> AreaDamageBullet::findMonstersInArea(const Vector2& center, float radius)
{
    QVector<Monster*> result;
    if (!monsterSpawner) return result;

    QPoint gridPos = gMap->pixelToGrid(QPointF(center.x, center.y));
    int centerGridX = gridPos.x();
    int centerGridY = gridPos.y();

    int gridRadius = static_cast<int>(radius / TILESIZE) + 1;

    for (int dx = -gridRadius; dx <= gridRadius; ++dx) {
        for (int dy = -gridRadius; dy <= gridRadius; ++dy) {
            int searchGridX = centerGridX + dx;
            int searchGridY = centerGridY + dy;

            if (searchGridX >= 0 && searchGridX < MAPHEIGHT / TILESIZE &&
                searchGridY >= 0 && searchGridY < MAPWIDTH / TILESIZE) {

                const auto& monstersInCell = monsterSpawner->grid[searchGridX][searchGridY];
                for (Monster* monster : monstersInCell) {
                    if (monster && !monster->isDead()) {
                        Vector2 monsterPos = monster->getPosition();
                        float distance = center.distanceTo(monsterPos);
                        if (distance <= radius) {
                            result.append(monster);
                        }
                    }
                }
            }
        }
    }

    return result;
}
