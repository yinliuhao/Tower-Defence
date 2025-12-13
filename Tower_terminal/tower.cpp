#include <QPixmap>
#include <QGraphicsScene>
#include <QString>
#include <global.h>
#include <structVector.h>
#include <vector>
#include <cmath>  // 用于sqrt计算距离
#include "tower.h"
#include "monster.h"
#include <cmath>  // Qt数学库（计算角度）
#include <QDebug>
#include <QPainter>
#include <QRectF>
#include <QColor>
#include<QPointF>
#include "utils.h"
#include "bulletmanager.h"

Tower::Tower(TowerType type)
    : towerType(type),
    level(1),
    isPlaced(false),
    isActive(false),
    attackTimer(new QTimer(this)),
    manager(nullptr)
{
    // 初始化计时器
    attackTimer = new QTimer(this);
    // 绑定计时器超时信号到攻击槽函数
    connect(attackTimer, &QTimer::timeout, this, &Tower::onAttackTimerTimeout);

    switch(towerType)
    {
    case TowerType::TOWER1:
      maxHealth = health = TOWER1_MAHEALTH;
      buildCost =  TOWER1_BUILCOST;
      attackRange = TOWER1_ATTACKRANGE;
      attackInterval =  TOWER1_ATTACKINTERVAL;
      bulletDamage = 10;  // 添加初始化
      bulletSpeed = 8;    // 添加初始化
      loadPixmap(":/picture/tower/archer_level1.png");
      break;
    case TowerType::TOWER2:
        maxHealth = health = TOWER2_MAHEALTH;
        buildCost =  TOWER2_BUILCOST;
        attackRange = TOWER2_ATTACKRANGE;
        attackInterval =  TOWER2_ATTACKINTERVAL;
        bulletDamage = 15;  // 添加初始化
        bulletSpeed = 10;   // 添加初始化
        loadPixmap(":/tower/tower2.png");
        break;
    case TowerType::TOWER3:
        maxHealth = health = TOWER3_MAHEALTH;
        buildCost =  TOWER3_BUILCOST;
        attackRange = TOWER3_ATTACKRANGE;
        attackInterval =  TOWER3_ATTACKINTERVAL;
        bulletDamage = 20;  // 添加初始化
        bulletSpeed = 12;   // 添加初始化
        loadPixmap(":/tower/tower3.png");
        break;
    default:
        maxHealth = health = 0;
        buildCost =  0;
        attackRange = 0;
        attackInterval = 0;
        bulletDamage = 0;   // 添加初始化
        bulletSpeed = 0;    // 添加初始化
    }
}

Tower::~Tower()
{
    // 释放计时器资源
    attackTimer->stop();
    attackTimer->deleteLater();
}

void Tower::setAttackInterval(int ms)
{
    attackInterval = ms;
    if (!attackTimer->isActive())
        attackTimer->start(attackInterval);
}

// --------------------------------------------------------
void Tower::put(int gridX, int gridY)
{
    towerGridPosition = Vector2(gridX, gridY);
    QPointF pixalPosition = gMap->gridToPixel(towerGridPosition.x, towerGridPosition.y);
    towerPixalCenter.x = pixalPosition.x();
    towerPixalCenter.y = pixalPosition.y();
    setPos(towerPixalCenter.x, towerPixalCenter.y);

    isPlaced = true;
    isActive = true;

    setAttackInterval(attackInterval);
}

// --------------------------------------------------------
void Tower::remove()
{
    isActive = false;
    attackTimer->stop();

    emit clearTower(buildCost / 2); // 返还一半金钱
}

// --------------------------------------------------------
void Tower::upgrade()
{
    level++;
    maxHealth += 20;
    health = maxHealth;

    bulletDamage += 5;
    attackRange += 10;

    // 更换高级图片
    switch(towerType)
    {
    case TowerType::TOWER1:
        loadPixmap(QString(":/picture/tower/tower1_level1.png"));
        break;
    case TowerType::TOWER2:
        loadPixmap(":/tower/tower2.png");
        break;
    case TowerType::TOWER3:
        loadPixmap(":/tower/tower3.png");
        break;
    default:
        break;
    }
}

// --------------------------------------------------------
void Tower::takeDamage(int dmg)
{
    health -= dmg;
    if (health < 0)
    {
        health = 0;
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();  // 安全延迟删除
        return;
    }
}

// --------------------------------------------------------
bool Tower::isAlive() const
{
    return health > 0;
}

// --------------------------------------------------------
int Tower::getLevel() const          { return level; }
int Tower::getBuildCost() const      { return buildCost; }
int Tower::getHealth() const         { return health; }
int Tower::getMaxHealth() const      { return maxHealth; }
TowerType Tower::getTowerType() const { return towerType; }

// --------------------------------------------------------
// 图形区域
// --------------------------------------------------------
QRectF Tower::boundingRect() const
{
    return QRectF(0, 0, TILESIZE, TILESIZE);
}

// --------------------------------------------------------
void Tower::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*)
{
    p->save();
    p->drawPixmap(0, 0, towerPixmap);
    p->restore();
}

// --------------------------------------------------------
// 加载图片并自动设置旋转中心
// --------------------------------------------------------
void Tower::loadPixmap(const QString& path)
{
    towerPixmap = QPixmap(path);
    setPixmap(towerPixmap);   // 使用 QGraphicsPixmapItem 的接口
    setTransformOriginPoint(QPointF(towerPixalCenter.x, towerPixalCenter.y));
}

// --------------------------------------------------------
// 自动选择目标怪物（最近的或列表第一个）
// --------------------------------------------------------
const Monster* Tower::selectAttackTarget() const
{
    if (!isActive || !isPlaced) return nullptr;

    double tx = towerPixalCenter.x;
    double ty = towerPixalCenter.y;

    int gx = towerGridPosition.x;
    int gy = towerGridPosition.y;

    int rangeGrid = attackRange / TILESIZE;

    int minX = qMax(0, gx - rangeGrid);
    int maxX = qMin(MAPHEIGHT / TILESIZE - 1, gx + rangeGrid);
    int minY = qMax(0, gy - rangeGrid);
    int maxY = qMin(MAPWIDTH - 1, gy + rangeGrid);

    const Monster* best = nullptr;
    double bestDist = 999999.0;

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {

            const std::vector<Monster*> &cell = monsterSpawner->grid[x][y];  // 你存的怪物列表

            for (Monster* m : cell) {
                if (!m || m->getMonsterState() == MonsterState::DEAD) continue;

                double dx = m->x() - tx;
                double dy = m->y() - ty;
                double dist = std::sqrt(dx*dx + dy*dy);

                if (dist < attackRange && dist < bestDist) {
                    best = m;
                    bestDist = dist;
                }
            }
        }
    }
    return best;
}

// --------------------------------------------------------
// 旋转塔朝目标怪物
// --------------------------------------------------------
void Tower::rotateToTarget(const Monster* target)
{
    if (!target) return;

    QPointF tpos = target->pos();

    qreal dx = tpos.x() - towerPixalCenter.x;
    qreal dy = tpos.y() - towerPixalCenter.y;

    qreal angle = qRadiansToDegrees(qAtan2(dy, dx)) + 90; // 让图片朝上

    updateRotation(angle);
}

// --------------------------------------------------------
void Tower::updateRotation(qreal angleRad)
{
    currentRotation = angleRad;
    double angleDeg = angleRad * 180.0 / M_PI;               // 转换
    setRotation(angleDeg);
}

// --------------------------------------------------------
// 发射子弹
// --------------------------------------------------------
void Tower::fire(const Monster* target) const
{
    if (!manager || !target) return;

    double dx = std::cos(currentRotation);
    double dy = std::sin(currentRotation);

    // 计算子弹起始点 = 炮塔中心 + 朝向单位向量 * 偏移
    Vector2 startPos(
        towerPixalCenter.x + dx * TILESIZE / 2,
        towerPixalCenter.y + dy * TILESIZE / 2
        );

    // 修正：只调用一次，传递所有必需的参数
    manager->createBullet(startPos, target, towerType, level,
                          bulletSpeed, bulletDamage, nullptr);
}

// --------------------------------------------------------
// 攻击计时器事件
// --------------------------------------------------------
void Tower::onAttackTimerTimeout()
{
    if (!isActive || !isPlaced) return;

    const Monster* target = selectAttackTarget();

    if (target) {
        rotateToTarget(target);
        fire(target);
    }
}
