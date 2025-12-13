#ifndef BULLETMANAGER_H
#define BULLETMANAGER_H

#include "bullet.h"
#include <vector>

class BulletManager : public QObject
{
    Q_OBJECT

public:
    BulletManager(float speed, float damage, QObject* parent = nullptr);
    virtual ~BulletManager();  // 添加虚析构函数声明

    // 修改createBullet函数，添加towerLevel参数
    Bullet* createBullet(const Vector2& startPos, const Monster* target, TowerType towerType,
                         int towerLevel = 1,  // 新增：炮塔等级
                         float speed = BULLET_SPEED, float damage = BULLET_DAMAGE, QGraphicsItem* parent = nullptr);

    // 更新所有子弹
    void updateBullets();

    // 清理
    void removeBullet(Bullet* bullet);
    void clearAll();

    // 获取子弹信息
    const std::vector<Bullet*>& getBullets() const { return bullets_; }
    size_t getBulletCount() const { return bullets_.size(); }

    // 设置子弹属性
    void setBulletSpeed(float speed) { bulletSpeed_ = speed; }
    void setBulletDamage(float damage) { bulletDamage_ = damage; }
    float getBulletSpeed() const { return bulletSpeed_; }
    float getBulletDamage() const { return bulletDamage_; }

signals:
    void bulletCreated(Bullet* bullet);
    void bulletHit(const Monster* target, float dam);

private:
    std::vector<Bullet*> bullets_;
    float bulletSpeed_;
    float bulletDamage_;
};

#endif // BULLETMANAGER_H
