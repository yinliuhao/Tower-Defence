#include "bullet.h"

#include <cmath>//提供数学函数

// 更新子弹状态


bullet::bullet(Vector2 bulletposition,
               Vector2 targetposition,
               float bulletdamage,
               float bulletspeed)
    :bulletposition(bulletposition),
    targetposition(targetposition),
    bulletdamage(bulletdamage),
    bulletspeed(bulletspeed)
{}


// 更新子弹状态
void bullet::bulletfly(Vector2 bulletposition,Vector2 targetposition,Vector2 monsterposition,float bulletspeed)
{



}

void bullet::damagethemonster(float monsterhealth)
{




}



void bullet::showbullet()
{


}



