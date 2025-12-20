#ifndef AREADAMAGEBULLET_H
#define AREADAMAGEBULLET_H

#include "_bullet/bullet.h"
#include "_others/utils.h"
#include "_monster/monster.h"
#include "_others/structVector.h"

class AreaDamageBullet : public Bullet
{
    Q_OBJECT
public:
    AreaDamageBullet(const Vector2& startPos,
                     Monster* target,
                     TowerType towerType,
                     int towerLevel = 1,
                     float speed = BULLET_SPEED_DEFAULT,
                     float damage = BULLET_DAMAGE_DEFAULT,
                     float radius = 0.f,
                     QGraphicsItem* parent = nullptr);

protected:
    void takeDamage() override;

private:
    QVector<Monster*> findMonstersInArea(const Vector2& center, float radius);

private:
    float areaRadius_ = 0.f;
};

#endif // AREADAMAGEBULLET_H
