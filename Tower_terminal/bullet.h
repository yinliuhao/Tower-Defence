#ifndef BULLET_H
#define BULLET_H

#include<structVector.h>

class monster;
class tower;

// 子弹类
class bullet {
    friend monster;
private:
    Vector2 bulletposition;      // 当前位置
    Vector2 targetposition;      //由tower访问monster后初始化，接着子弹访问monster来改变
    float bulletdamage;          // 伤害值
    float bulletspeed;           // 移动速度

public:
    // 构造函数
    bullet(){};

    bullet(Vector2 bulletposition,
           Vector2 targetposition,
           float bulletdamage,
           float bulletspeed);

    ~bullet(){}

    // 更新子弹状态
    void bulletfly(Vector2 bulletposition,Vector2 targetposition,Vector2 monsterposition,float bulletspeed);

    void damagethemonster(float monsterhealth);//怪物的友元函数，是子弹的函数，子弹命中怪物时访问血量

    void showbullet();

};


// 特殊子弹类型：范围伤害子弹（继承bullet基类）
//class AoeBullet : public bullet {
//private:
//public:
//};

#endif // BULLET_H

