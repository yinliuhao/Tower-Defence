// areadamagebullet.h
#ifndef AREADAMAGEBULLET_H
#define AREADAMAGEBULLET_H

#include "bullet.h"

class AreaDamageBullet : public Bullet
{
    Q_OBJECT
public:
    AreaDamageBullet(const Vector2& startPos, const Monster* target, TowerType towerType,
                     int towerLevel, float speed, float damage, float areaRadius,
                     QGraphicsItem* parent = nullptr);

    void hitTarget() override;

private:
    float areaRadius_;
    QVector<Monster*> findMonstersInArea(const Vector2& center, float radius);
};

#endif // AREADAMAGEBULLET_H
