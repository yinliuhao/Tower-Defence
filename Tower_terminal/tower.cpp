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
#include <QPointF>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "utils.h"
#include "bulletmanager.h"

Tower::Tower(TowerType type)
    : towerType(type),
    level(1),
    currentRotation(90),
    isPlaced(false),
    isActive(false),
    currentFrame(0)
{   
    switch(towerType)
    {
    case TowerType::TOWER1:
      maxHealth = health = TOWER1_MAHEALTH;
      buildCost =  TOWER1_BUILCOST;
      attackRange = TOWER1_ATTACKRANGE;
      attackInterval =  TOWER1_ATTACKINTERVAL;
      bulletDamage = 50;  // 添加初始化
      bulletSpeed = 8;    // 添加初始化
      totalFrame = 4;
      break;
    case TowerType::TOWER2:
        maxHealth = health = TOWER2_MAHEALTH;
        buildCost =  TOWER2_BUILCOST;
        attackRange = TOWER2_ATTACKRANGE;
        attackInterval =  TOWER2_ATTACKINTERVAL;
        bulletDamage = 15;  // 添加初始化
        bulletSpeed = 10;   // 添加初始化
        totalFrame = 1;
        break;
    case TowerType::TOWER3:
        maxHealth = health = TOWER3_MAHEALTH;
        buildCost =  TOWER3_BUILCOST;
        attackRange = TOWER3_ATTACKRANGE;
        attackInterval =  TOWER3_ATTACKINTERVAL;
        bulletDamage = 20;  // 添加初始化
        bulletSpeed = 4;   // 添加初始化
        totalFrame = 1;
        break;
    default:
        maxHealth = health = 0;
        buildCost =  0;
        attackRange = 0;
        attackInterval = 0;
        bulletDamage = 0;   // 添加初始化
        bulletSpeed = 0;    // 添加初始化
        totalFrame = 0;   
    }

    manager = new BulletManager(bulletSpeed, bulletDamage, this);

    // 初始化计时器
    trackTimer = new QTimer(this);
    trackTimer->setInterval(30);
    connect(trackTimer, &QTimer::timeout, this, &Tower::onTrackTimerTimeout);

    attackTimer = new QTimer(this);
    attackTimer->setInterval(attackInterval);
    connect(attackTimer, &QTimer::timeout, this, &Tower::onAttackTimerTimeout);

    frameTimer = new QTimer(this);
    frameTimer->setInterval(100);
    connect(frameTimer, &QTimer::timeout, this, &Tower::onFrameTimerTimeout);

    //qDebug() << "[Tower] ctor called, type =" << int(type);
    loadPixmap();

    updateRotation(currentRotation);
}

Tower::~Tower()
{
    // 释放计时器资源
    attackTimer->stop();
    attackTimer->deleteLater();

    frameTimer->stop();
    frameTimer->deleteLater();

    trackTimer->stop();
    trackTimer->deleteLater();
}

void Tower::setAttackInterval(int ms)
{
    attackInterval = ms;
    attackTimer->stop();
    attackTimer->start(attackInterval);
}

// --------------------------------------------------------
void Tower::put(int gridX, int gridY)
{
    //qDebug() << "[Tower] put called:"
    //         << "grid =" << gridX << gridY;

    towerGridPosition = Vector2(gridX, gridY);
    QPointF pixalPosition = gMap->gridToPixel(towerGridPosition.x, towerGridPosition.y);
    towerPixalCenter.x = pixalPosition.x();
    towerPixalCenter.y = pixalPosition.y();
    setPos(towerPixalCenter.x - TILESIZE / 2.0, towerPixalCenter.y - TILESIZE / 2.0);
    setZValue(TOWERZVALUE);

    isPlaced = true;
    isActive = true;

    setAttackInterval(attackInterval);
    trackTimer->start();

    //qDebug() << "[Tower] put finished, active =" << isActive;
}

// --------------------------------------------------------
void Tower::remove()
{
    isActive = false;

    attackTimer->stop();
    frameTimer->stop();
    trackTimer->stop();

    emit clearTower(towerType);
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
        towerPixmap = QPixmap(QString(":/picture/tower/archer_level%1.png").arg(level));
        break;
    case TowerType::TOWER2:
        towerPixmap = QPixmap(QString(":/picture/tower/cannon_level%1.png").arg(level));
        break;
    case TowerType::TOWER3:
        towerPixmap = QPixmap(QString(":/picture/tower/mortar_level%1.png").arg(level));
        break;
    default:
        break;
    }

    basePixmap = QPixmap(QString(":/picture/tower/base_level%1.png").arg(level));

    if(towerType == TowerType::TOWER1){
        framePixmap.clear();
        for(int i = 0; i < totalFrame; i++){
            framePixmap.push_back(QPixmap(QString(":/picture/tower/archer_level%1").arg(level)
                                          + QString("_frame%1.png").arg(i + 1)));
        }
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
BulletManager* Tower::getManager() { return manager; }

// --------------------------------------------------------
// 图形区域
// --------------------------------------------------------
QRectF Tower::boundingRect() const
{
    return QRectF(0, 0, TILESIZE, TILESIZE);
    //qDebug() << "[Tower] boundingRect called";
}

// --------------------------------------------------------
void Tower::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*)
{
    if(currentFrame == 0){
        p->drawPixmap(0, 0, towerPixmap);
    }
    else{
        p->drawPixmap(0, 0, framePixmap[currentFrame - 1]);
    }

    //qDebug() << "[Tower] paint called, frame =" << currentFrame;
}

// --------------------------------------------------------
// 加载图片并自动设置旋转中心
// --------------------------------------------------------
void Tower::loadPixmap()
{
    switch(towerType)
    {
    case TowerType::TOWER1:
        towerPixmap = QPixmap(QString(":/picture/tower/archer_level1.png"));
        break;
    case TowerType::TOWER2:
        towerPixmap = QPixmap(QString(":/picture/tower/cannon_level1.png"));
        break;
    case TowerType::TOWER3:
        towerPixmap = QPixmap(QString(":/picture/tower/mortar_level1.png"));
    default:
        break;
    }

    if(towerType == TowerType::TOWER1){
        for(int i = 0; i < totalFrame; i++){
            framePixmap.push_back(QPixmap(QString(":/picture/tower/archer_level1_frame%1.png").arg(i + 1)));
        }
    }

    basePixmap = QPixmap(QString(":/picture/tower/base_level1.png"));
    //qDebug() << "[Tower] loadPixmap:"
    //         << "towerPixmap isNull =" << towerPixmap.isNull();
    setPixmap(towerPixmap);
    setTransformOriginPoint(QPointF(TILESIZE / 2.0, TILESIZE / 2.0));
}

// --------------------------------------------------------
// 自动选择目标怪物（最近的或列表第一个）
// --------------------------------------------------------
Monster* Tower::selectAttackTarget()
{
    if (!isActive || !isPlaced) return nullptr;

    double tx = towerPixalCenter.x;
    double ty = towerPixalCenter.y;

    int gx = towerGridPosition.x;
    int gy = towerGridPosition.y;

    int bigGrid = attackRange / TILESIZE;

    int minX = qMax(0, gx - bigGrid);
    int maxX = qMin(MAPWIDTH / TILESIZE - 1, gx + bigGrid);
    int minY = qMax(0, gy - bigGrid);
    int maxY = qMin(MAPHEIGHT / TILESIZE - 1, gy + bigGrid);
    Monster* best = nullptr;

    if(towerType != TowerType::TOWER3)
    {
        double bestDist = 999999.0;

        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {

                const std::vector<Monster*> &cell = monsterSpawner->grid[x][y];  // 你存的怪物列表

                for (Monster* m : cell) {
                    if (!m) continue;
                    // 只选择仍在场景中、仍存活且处于移动状态的怪物
                    if (!m->scene()) continue;
                    if (m->isDead()) continue;
                    if (m->getMonsterState() != MonsterState::MOVING) continue;

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
    }
    else
    {
        double bestDist = 0.0;
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {

                const std::vector<Monster*> &cell = monsterSpawner->grid[x][y];  // 你存的怪物列表

                for (Monster* m : cell) {
                    if (!m) continue;
                    if (!m->scene()) continue;
                    if (m->isDead()) continue;
                    if (m->getMonsterState() != MonsterState::MOVING) continue;

                    double dx = m->x() - tx;
                    double dy = m->y() - ty;
                    double dist = std::sqrt(dx*dx + dy*dy);

                    if (dist < attackRange && dist > attackRange / 3.0 && dist > bestDist) {
                        best = m;
                        bestDist = dist;
                    }
                }
            }
        }
    }

    // #region agent log
    /*if (best) {
        try {
            QFile file(".cursor/debug.log");
            if (file.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream ts(&file);
                ts << "{\"sessionId\":\"debug-session\","
                      "\"runId\":\"post-fix\","
                      "\"hypothesisId\":\"H5\","
                      "\"location\":\"tower.cpp:264\","
                      "\"message\":\"Tower::selectAttackTarget choose\","
                      "\"data\":{\"tower\":\"" << this
                   << "\",\"target\":\"" << best
                   << "\",\"hp\":" << best->getHealth()
                   << ",\"state\":" << int(best->getMonsterState())
                   << ",\"hasScene\":" << (best->scene() ? "true" : "false")
                   << "},"
                      "\"timestamp\":" << static_cast<long long>(QDateTime::currentMSecsSinceEpoch())
                   << "}\n";
            }
        } catch (...) {
        }
    }*/
    // #endregion

    return best;
}

// --------------------------------------------------------
// 旋转塔朝目标怪物
// --------------------------------------------------------
void Tower::rotateToTarget(Monster* target)
{
    if (!target) return;

    QPointF tpos = target->pos();

    qreal dx = tpos.x() - towerPixalCenter.x;
    qreal dy = tpos.y() - towerPixalCenter.y;

    qreal angle = qRadiansToDegrees(qAtan2(dy, dx));

    updateRotation(angle);
}

// --------------------------------------------------------
void Tower::updateRotation(qreal angle)
{
    currentRotation = angle;
    setRotation(angle);
}

// --------------------------------------------------------
// 发射子弹
// --------------------------------------------------------
void Tower::fire(Monster* target) const
{
    if (!manager || !target) return;

    qreal rad = qDegreesToRadians(currentRotation);

    double dx = std::cos(rad);
    double dy = std::sin(rad);

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

    Monster* target = selectAttackTarget();
    if (!target) return;

    if (totalFrame == 1) {
        // 无攻击动画：立刻开火
        fire(target);
    } else {
        // 有攻击动画：开始播帧
        currentFrame = 0;
        frameTimer->start();
    }
}

void Tower::onFrameTimerTimeout()
{
    currentFrame++;

    if (currentFrame == totalFrame)
    {
        Monster* t = selectAttackTarget();
        if (t) {
            fire(t);   // 方向已经是最新的
        }
    }

    if (currentFrame >= totalFrame)
    {
        frameTimer->stop();
        currentFrame = 0;
        update();
    }
}

void Tower::onTrackTimerTimeout()
{
    Monster* t = selectAttackTarget();

    if (!t && frameTimer->isActive()) {
        //qDebug() << "[Tower] interrupt attack animation, no target";
        frameTimer->stop();
        currentFrame = 0;
        update();
        return;
    }

    if (t) {
        rotateToTarget(t);
    }
}
