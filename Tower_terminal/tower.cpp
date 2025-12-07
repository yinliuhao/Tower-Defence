#include <QPixmap>
#include <QString>
#include <global.h>
#include <structVector.h>
#include <cmath>  // 用于sqrt计算距离
#include "tower.h"
#include "monster.h"
#include "map.h"
#include <cmath>  // Qt数学库（计算角度）
#include <QDebug>
#include <QPainter>
#include <QRectF>
#include <QColor>
#include<QPointF>
#include "utils.h"

Tower::Tower()
    : towerType(TOWER_NONE),
    isPlaced(false),
    isActive(false),
    level(1)
{
    setTowerType(TOWER1);
    // 初始化计时器
    attackTimer = new QTimer(this);
    // 绑定计时器超时信号到攻击槽函数
    connect(attackTimer, &QTimer::timeout, this, &Tower::onAttackTimerTimeout);
}

Tower::~Tower()
{
    // 释放计时器资源
    attackTimer->stop();
    attackTimer->deleteLater();
}

// 计时器触发的攻击逻辑
void Tower::onAttackTimerTimeout()
{
    // 在这里编写塔的攻击逻辑（例如：检测范围内敌人、发射子弹等）
    qDebug() << "Tower attacked! (Interval: " << attackTimer->interval() << "ms)";
}

void Tower::setAttackInterval(int ms)
{
    attackTimer->setInterval(ms);
    attackSpeed = ms;
}

// 2. setTowerType函数实现
void Tower::setTowerType(towertype type) {
    towerType = type;
    level = 1;
    switch (type) {
    case TOWER1:
        maxHealth = 200; health = 200; buildCost = 100;
        damage = 25; attackRange = 200; attackSpeed = 1.5;
        setAttackInterval(500);
        setPixmap(":/picture/tower/archer.png");
        break;
    case TOWER2:
        maxHealth = 300; health = 300; buildCost = 150;
        damage = 60; attackRange = 150; attackSpeed = 3.0;
        setAttackInterval(3000);
        setPixmap(":/images/tower2.png");
        break;
    case TOWER3:
        maxHealth = 150; health = 150; buildCost = 120;
        damage = 35; attackRange = 120; attackSpeed = 2.0;
        setAttackInterval(3000);
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
    setAttackInterval(attackSpeed * 0.9);

    //更新照片
}

// 4. put函数实现
void Tower::put(int x, int y) {
    if (isPlaced) return;
    gMap->placeTower(QPointF(x, y));
    isPlaced = true;
    isActive = true;
    //放置逻辑
}

// 5. remove函数实现
void Tower::remove() {
    if (!isPlaced) return;
    emit clearTower(buildCost);
    isPlaced = false;
    isActive = false;
}

// 6. selectAttackTarget函数实现
// const Monster* Tower::selectAttackTarget(const std::vector<Monster>& monsters) const {
//     if (!isActive || towerType == TOWER_NONE) return nullptr;

//     const Monster* closestTarget = nullptr;
//     float minDistance = std::numeric_limits<float>::max();

//     for (const auto& monster : monsters) {
//         if (!monster.isDead()) {  // 注意这里用 . 而不是 ->
//             // 获取怪物位置
//             Vector2 monsterPos = monster.getPosition();

//             // 计算与塔的距离
//             float dx = monsterPos.x - x;
//             float dy = monsterPos.y - y;
//             float distance = std::sqrt(dx * dx + dy * dy);

//             // 如果怪物在攻击范围内且距离更近
//             if (distance <= attackRange && distance < minDistance) {
//                 minDistance = distance;
//                 closestTarget = &monster;  // 注意这里取地址
//             }
//         }
//     }
//     return closestTarget;
// }

// 7. fire函数实现
// void Tower::fire(BulletManager& bullet, const Monster* target)const {
//     if (!target || !isActive) return;

//     // 获取目标怪物位置
//     BulletManager bltm;
//     Bullet* newBullet = bullet.createBullet();
//     newBullet->setDamage(damage);      // 设置伤害
//     newBullet->getTargetPosition();
//     newBullet->getPosition();
//     newBullet->isActive();
// }

// 8. setPixmap函数实现
// void Tower::setPixmap(const QString& path) {
//     pixmapPath = path;
//     if (!towerPixmap.load(path)) {
//         std::cerr << "Failed to load tower pixmap: " << path.toStdString() << std::endl;
//     }
//}

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
    //补充逻辑
}

// 11. isAlive函数实现
bool Tower::isAlive() const {
    return health > 0;
}

// 16. getBuildCost函数实现
int Tower::getBuildCost() const {
    return buildCost;
}

// 17. getTowerType函数实现
towertype Tower::getTowerType() const {
    return towerType;
}
void Tower::rotateToTarget(const Monster* target) {
    if (!target || !isActive) {
        return;
    }
    // 1. 获取坐标（保持不变）
    QPointF towerPos(towerPosition.x + TILESIZE / 2, towerPosition.y + TILESIZE / 2);
    Vector2 monsterVec = target->getPosition();
    QPointF monsterPos(monsterVec.x, monsterVec.y);
    // 2. 计算向量（保持不变）
    qreal dx = monsterPos.x() - towerPos.x();
    qreal dy = monsterPos.y() - towerPos.y();
    // 3. 替换Qt数学函数为标准C++函数
    //atan2(dy, dx) 是标准cmath函数，返回弧度
    double angleRad = atan2(dy, dx);
    // 弧度转角度：弧度 * (180.0 / M_PI)
    qreal angleDeg = angleRad * (180.0 / M_PI);
    // 角度修正（保持不变，根据图片初始朝向调整）
    angleDeg -= 90;
    // 4. 更新旋转（保持不变）
    updateRotation(angleDeg);
}

void Tower::updateRotation(qreal angle) {
    currentRotation = angle;
    update();
}

// 核心：仅显示塔图片的 paint 函数
void Tower::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {

    // 1. 基础绘图优化（平滑显示图片）
    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    // 2. 计算图片绘制位置（以塔中心为原点）
    QRectF pixmapRect(-TILESIZE / 2,
                      -TILESIZE / 2,
                      TILESIZE,
                      TILESIZE);

    // （可选）如需保留旋转功能则保留以下3行，不需要则删除
    painter->save();
    painter->rotate(currentRotation);  // 应用旋转角度（头文件需保留 m_rotation 变量）

    // 4. 绘制塔图片（核心功能）
    painter->drawPixmap(pixmapRect.topLeft(), towerPixmap);

    // （可选）旋转功能对应的恢复状态（删除旋转则删除此行）
    painter->restore();
}

// 必须重写：定义绘图区域（避免图片被裁剪）
QRectF Tower::boundingRect() const {
    if (!towerPixmap.isNull()) {
        // 以图片实际尺寸为绘图区域（中心在原点）
        return QRectF(-TILESIZE / 2,
                      -TILESIZE / 2,
                      TILESIZE,
                      TILESIZE);
    }
    // 图片加载失败时的默认区域
    return QRectF(-25, -25, 50, 50);
}
