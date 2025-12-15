// areadamagebullet.h
#ifndef AREADAMAGEBULLET_H
#define AREADAMAGEBULLET_H

#include "bullet.h"

class AreaDamageBullet : public Bullet
{
    Q_OBJECT
public:
    AreaDamageBullet(const Vector2& startPos, Monster* target, TowerType towerType, int towerLevel = 1,
           float speed = BULLET_SPEED, float damage = BULLET_DAMAGE, float radius = 0, QGraphicsItem* parent = nullptr);
    void hitTarget() override;

private:
    float areaRadius_;
    QVector<Monster*> findMonstersInArea(const Vector2& center, float radius);
};

#endif // AREADAMAGEBULLET_H
