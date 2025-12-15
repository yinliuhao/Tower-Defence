#include "bulletmanager.h"
#include <QGraphicsScene>
#include <QDebug>
#include <algorithm>
#include "areadamagebullet.h"  // 添加包含

BulletManager::BulletManager(float speed, float damage, QObject* parent)
: QObject(parent)
{
    this->bulletDamage_ = damage;
    this->bulletSpeed_ = speed;
}

// 添加析构函数声明到头文件后，实现如下：
BulletManager::~BulletManager()
{
    clearAll();
}

// 修正createBullet函数调用
Bullet* BulletManager::createBullet(const Vector2& fromPos, Monster* target,
                                    TowerType towerType, int towerLevel,
                                    float speed, float damage, QGraphicsItem* parent)
{
    float actualSpeed = (speed > 0) ? speed : bulletSpeed_;
    float actualDamage = (damage > 0) ? damage : bulletDamage_;

    Bullet* bullet = nullptr;

    switch (towerType) {
    case TowerType::TOWER3:
        bullet = new AreaDamageBullet(fromPos, target, towerType, towerLevel,
                                      actualSpeed, actualDamage, 150.0f, parent);
        break;
    default:
        bullet = new Bullet(fromPos, target, towerType, towerLevel,
                            actualSpeed, actualDamage, parent);
        break;
    }

    bullets_.push_back(bullet);
    connect(bullet, &Bullet::hit, this, [this, bullet]() {
        emit bulletHit(bullet->getTarget(), bullet->getDamage());
        this->removeBullet(bullet);
    });

    emit bulletCreated(bullet);
    return bullet;
}

void BulletManager::updateBullets()
{
    // 移除不活跃的子弹（备用机制）
    auto it = bullets_.begin();
    while (it != bullets_.end()) {
        Bullet* bullet = *it;
        if (!bullet->isActive()) {
            // 从场景中移除
            if (bullet->scene()) {
                bullet->scene()->removeItem(bullet);
            }
            delete bullet;
            it = bullets_.erase(it);
        } else {
            ++it;
        }
    }
}

void BulletManager::clearAll()
{
    for (Bullet* bullet : bullets_) {
        if (bullet->scene()) {
            bullet->scene()->removeItem(bullet);
        }
        delete bullet;
    }
    bullets_.clear();
}

void BulletManager::removeBullet(Bullet* bullet)
{
    auto it = std::find(bullets_.begin(), bullets_.end(), bullet);
    if (it != bullets_.end()) {
        // 删除对象并从列表中移除
        delete bullet;
        bullets_.erase(it);

        qDebug() << "子弹已移除，剩余子弹数量:" << bullets_.size();
    }
}

