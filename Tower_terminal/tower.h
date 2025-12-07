// Tower.h
#pragma once
#include <QPixmap>
#include <QString>
#include <vector>
#include "bulletManager.h"
#include<QGraphicsPixmapItem>
#include<QTimer>
#include <QTransform>   // 用于图片旋转
#include <QPointF>

enum towertype {
    TOWER1,
    TOWER2,
    TOWER3,
    TOWER_NONE,
};

class Monster;
class Map;
class Bullet;

class Tower : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

private:
    towertype towerType;
    int health;
    int maxHealth;
    int buildCost;
    int damage;
    double attackRange;
    double attackSpeed;
    bool isPlaced;
    bool isActive;
    int level;
    QPixmap towerPixmap;
    // 新增：旋转相关变量
    Vector2 towerPosition;
    qreal currentRotation;  // 当前旋转角度（弧度/角度）
    QPointF towerCenter;    // 塔的中心坐标（旋转锚点）
    QTimer *attackTimer; // 攻击计时器

public:
    // 新增：旋转核心函数
    void rotateToTarget(const Monster* target);  // 朝向目标怪物旋转
    void updateRotation(qreal angleDeg);         // 更新旋转角度并刷新图片

    Tower();
    ~Tower() override;
    // 设置攻击间隔（单位：毫秒）
    void setAttackInterval(int ms);
    QRectF boundingRect() const override;

signals:
    void clearTower(int buildMoney);

private slots:
    // 计时器触发的槽函数（用于执行攻击逻辑）
    void onAttackTimerTimeout();
    void setTowerType(towertype type);
    void upgrade();
    void put(int x, int y);
    void remove();
    const Monster* selectAttackTarget(const std::vector<Monster>& monsters) const;
    void fire(BulletManager& bullet, const Monster* target) const;
    void setPixmap(const QString& path);
    int getLevel() const;
    void takeDamage(int dmg);
    bool isAlive() const;
    int getBuildCost() const;
    towertype getTowerType() const;
    void paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) override;
    friend class Bullet;
};
