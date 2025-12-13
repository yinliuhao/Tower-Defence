#include <QPixmap>
#include <QString>
#include <iostream>
#include <vector>
#include <cmath>  // 用于sqrt计算距离
#include "tower.h"
#include "monster.h"
#include "map.h"
#include "bullet.h"

// 1. 构造函数实现
Tower::Tower()
    : towerType(TOWER_NONE),
    health(0),
    maxHealth(0),
    buildCost(0),
    damage(0),
    attackRange(0),
    attackSpeed(0),
    x(0),
    y(0),
    isPlaced(false),
    isActive(false),
    level(1)
{}

// 2. setTowerType函数实现
void Tower::setTowerType(towertype type) {
    towerType = type;
    level = 1;
    switch (type) {
    case TOWER1:
        maxHealth = 200; health = 200; buildCost = 100;
        damage = 25; attackRange = 200; attackSpeed = 1.5;
        setPixmap(":/images/tower1.png");
        break;
    case TOWER2:
        maxHealth = 300; health = 300; buildCost = 150;
        damage = 60; attackRange = 150; attackSpeed = 3.0;
        setPixmap(":/images/tower2.png");
        break;
    case TOWER3:
        maxHealth = 150; health = 150; buildCost = 120;
        damage = 35; attackRange = 120; attackSpeed = 2.0;
        setPixmap(":/images/tower3.png");
        break;
    default:
        towerType = TOWER_NONE;
        break;
    }
}

// 3. upgrade函数实现
void Tower::upgrade() {
    if (towerType == TOWER_NONE || level >= 3) return;
    level++;
    maxHealth *= 1.5; health = maxHealth;
    damage *= 1.2;
    attackRange *= 1.1;
    attackSpeed *= 0.9;

    QString newPath = pixmapPath.left(pixmapPath.lastIndexOf('.'))
                      + QString::number(level)
                      + pixmapPath.right(pixmapPath.length() - pixmapPath.lastIndexOf('.'));
    setPixmap(newPath);
}

// 4. put函数实现
void Tower::put(Map& map) {
    if (isPlaced) return;
    if (map.canPlaceTower(x, y)) {
        map.markTowerPosition(x, y, this);
        isPlaced = true;
        isActive = true;
    } else {
        std::cerr << "Cannot place tower at (" << x << ", " << y << ")" << std::endl;
    }
}

// 5. remove函数实现
void Tower::remove(Map& map, int& playerGold) {
    if (!isPlaced) return;
    map.clearTowerPosition(x, y);
    playerGold += buildCost * 0.8;
    isPlaced = false;
    isActive = false;
}

// 6. selectAttackTarget函数实现
const Monster* Tower::selectAttackTarget(const std::vector<Monster>& monsters) const {
    if (!isActive || towerType == TOWER_NONE) return nullptr;

    const Monster* closestTarget = nullptr;
    float minDistance = std::numeric_limits<float>::max();

    for (const auto& monster : monsters) {
        if (!monster.isDead()) {  // 注意这里用 . 而不是 ->
            // 获取怪物位置
            Vector2 monsterPos = monster.getPosition();

            // 计算与塔的距离
            float dx = monsterPos.x - x;
            float dy = monsterPos.y - y;
            float distance = std::sqrt(dx * dx + dy * dy);

            // 如果怪物在攻击范围内且距离更近
            if (distance <= attackRange && distance < minDistance) {
                minDistance = distance;
                closestTarget = &monster;  // 注意这里取地址
            }
        }
    }

    return closestTarget;
}

// 7. fire函数实现
void Tower::fire(Bullet& bullet, const Monster* target) const {
    if (!target || !isActive) return;

    // 获取目标怪物位置
    Vector2 targetPos = target->getPosition();

    bullet.setDamage(damage);
    bullet.setStartPos(x, y);
    bullet.setTargetPos(targetPos.x, targetPos.y);
    bullet.setActive(true);
}

// 8. setPixmap函数实现
void Tower::setPixmap(const QString& path) {
    pixmapPath = path;
    if (!towerPixmap.load(path)) {
        std::cerr << "Failed to load tower pixmap: " << path.toStdString() << std::endl;
    }
}

// 9. getLevel函数实现
int Tower::getLevel() const {
    return level;
}

// 10. takeDamage函数实现
void Tower::takeDamage(int dmg) {
    health = std::max(0, health - dmg);
    if (health <= 0) {
        isActive = false;
    }
}

// 11. isAlive函数实现
bool Tower::isAlive() const {
    return health > 0;
}

// 12. getPixmap函数实现
QPixmap Tower::getPixmap() const {
    return towerPixmap;
}

// 13. setPos函数实现
void Tower::setPos(int x, int y) {
    this->x = x;
    this->y = y;
}

// 14. getX函数实现
int Tower::getX() const {
    return x;
}

// 15. getY函数实现
int Tower::getY() const {
    return y;
}

// 16. getBuildCost函数实现
int Tower::getBuildCost() const {
    return buildCost;
}

// 17. getTowerType函数实现
towertype Tower::getTowerType() const {
    return towerType;
}
