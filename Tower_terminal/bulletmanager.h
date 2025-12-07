#ifndef BULLETMANAGER_H
#define BULLETMANAGER_H

#include "bullet.h"
#include <vector>

class BulletManager : public QObject
{
    Q_OBJECT

public:
    BulletManager(QObject* parent = nullptr);
    ~BulletManager();

    // 创建子弹
    Bullet* createBullet(const Vector2& fromPos, const Vector2& targetPos,
                         float speed = BULLET_SPEED, QGraphicsScene* scene = nullptr);

    // 更新所有子弹
    void updateBullets();

    // 清理
    void clearAll();

    // 获取子弹信息
    const std::vector<Bullet*>& getBullets() const { return bullets_; }
    size_t getBulletCount() const { return bullets_.size(); }

    // 查找活跃子弹
    std::vector<Bullet*> getActiveBullets() const;

    // 设置子弹属性
    void setBulletSpeed(float speed) { bulletSpeed_ = speed; }
    float getBulletSpeed() const { return bulletSpeed_; }

signals:
    void bulletCreated(Bullet* bullet);
    void bulletHit(Bullet* bullet, const Vector2& position);

private:
    std::vector<Bullet*> bullets_;
    float bulletSpeed_;
};

#endif // BULLETMANAGER_H
