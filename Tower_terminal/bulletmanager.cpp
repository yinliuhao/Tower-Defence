#include "bulletmanager.h"
#include <QGraphicsScene>
#include <QDebug>
#include <algorithm>

BulletManager::BulletManager(float speed, float damage, QObject* parent)
    : QObject(parent), bulletSpeed_(speed),bulletDamage_(damage)
{
}

BulletManager::~BulletManager()
{
    clearAll();
}

Bullet* BulletManager::createBullet(const Vector2& fromPos, const Monster* target,
                                    float speed, float damage, QGraphicsItem*)
{
    float actualSpeed = (speed > 0) ? speed : bulletSpeed_;

    Bullet* bullet = new Bullet(fromPos, target, actualSpeed, damage);
    bullets_.push_back(bullet);

    // 连接命中信号
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

