#ifndef AREADAMAGEBULLET_H
#define AREADAMAGEBULLET_H

#include "bullet.h"

class AreaDamageBullet : public Bullet
{
    Q_OBJECT
public:
    AreaDamageBullet(const Vector2& startPos,
                     Monster* target,
                     TowerType towerType,
                     int towerLevel = 1,
                     float speed = BULLET_SPEED,
                     float damage = BULLET_DAMAGE,
                     float radius = 0.f,
                     QGraphicsItem* parent = nullptr);

protected:
    void hitTarget() override;

private:
    QVector<Monster*> findMonstersInArea(const Vector2& center, float radius);

private:
    float areaRadius_ = 0.f;
};

#endif // AREADAMAGEBULLET_H
