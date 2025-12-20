#include "areadamagebullet.h"
#include "monster.h"
#include "global.h"
#include <QDebug>
#include <cmath>
#include <QGraphicsScene>  // 添加包含
// 修正构造函数实现
AreaDamageBullet::AreaDamageBullet(const Vector2& startPos, const Monster* target, TowerType towerType,
                                   int towerLevel, float speed, float damage, float areaRadius, QGraphicsItem* parent)
    : Bullet(startPos, target, towerType, towerLevel, speed, damage, parent),
    areaRadius_(areaRadius)
{
}

void AreaDamageBullet::hitTarget()
{
    // 使用基类的getter方法访问私有成员
    if (!isActive() || !getTarget()) return;

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

    // 调用基类的命中处理
    Bullet::hitTarget();  // 调用基类实现
}

QVector<Monster*> AreaDamageBullet::findMonstersInArea(const Vector2& center, float radius)
{
    QVector<Monster*> result;
    if (!monsterSpawner) return result;
    // Avoid accessing grid directly (may contain stale pointers). Scan the monsters list.
    int gridRadius = static_cast<int>(radius / TILESIZE) + 1; (void)gridRadius;
    std::vector<Monster*> all = monsterSpawner->monsters;
    for (Monster* monster : all) {
        if (!monster) continue;
        QPointer<Monster> safeM(monster);
        if (!safeM) continue;
        if (safeM->isDead() || safeM->isDying()) continue;
        Vector2 monsterPos = safeM->getPosition();
        float distance = center.distanceTo(monsterPos);
        if (distance <= radius) result.append(safeM);
    }

    return result;
}
