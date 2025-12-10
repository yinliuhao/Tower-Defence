// Tower.h
#pragma once
#include <QPixmap>
#include <QString>
#include "bulletmanager.h"
#include<QGraphicsPixmapItem>
#include<QTimer>
#include <QTransform>   // 用于图片旋转
#include <QPointF>

class Monster;
class Bullet;

enum class TowerType {
    TOWER1,
    TOWER2,
    TOWER3,
    TOWER_NONE,
};

class Tower : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

private:
    // ----------- 基础属性 -----------
    TowerType towerType;

    int health;
    int maxHealth;
    int buildCost;
    double attackRange;
    double attackInterval;
    int level;

    // ----------- 坐标旋转相关 -----------
    Vector2 towerGridPosition;
    qreal currentRotation;   //弧度制
    Vector2 towerPixalCenter;

    // ----------- 子弹属性 -----------
    int bulletDamage;
    int bulletSpeed;

    // ----------- 状态 -----------
    bool isPlaced;
    bool isActive;

    // ----------- 图像 -----------
    QPixmap towerPixmap;

    // ----------- 外部组件 -----------
    QTimer *attackTimer;
    BulletManager* manager;

public:
    explicit Tower(TowerType type);
    ~Tower() override;

    void setAttackInterval(int ms);
    void put(int gridX, int gridY);
    void remove();
    void upgrade();

    void takeDamage(int dmg);
    bool isAlive() const;

    TowerType getTowerType() const;
    int getLevel() const;
    int getBuildCost() const;
    int getHealth() const;
    int getMaxHealth() const;

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    void rotateToTarget(const Monster* target);
    void updateRotation(qreal angleRad);
    void loadPixmap(const QString& path);

    const Monster* selectAttackTarget() const;
    void fire(const Monster* target) const;

private slots:
    void onAttackTimerTimeout();

signals:
    void clearTower(int buildMoney);
};
