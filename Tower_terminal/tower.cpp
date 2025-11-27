#include<iostream>
#include"tower.h"
#include"map.h"
#include"monster.h"
#include"bullet.h"

using namespace std;

tower::tower(towertype type,
             float towerhealth,
             int towergold,
             int level,
             double towerdamage,
             double attackRange,
             double attackSpeed,
             Vector2 towerposition,
             bool isPlaced/*,
             bullet towerbullet*/)
    :type(type),
    towerhealth(towerhealth),
    towergold(towergold),
    level(level),
    towerdamage(towerdamage),
    attackRange(attackRange),
    attackSpeed(attackSpeed),
    towerposition(towerposition),
    isPlaced(isPlaced)/*,
    towerbullet(towerbullet)*/
{}







// 1. 放置塔到地图函数实现
void tower::put(Map& map, Vector2 towerposition) {


    //



}

// 2. 设置塔种类函数实现
void tower::setTowerType(towertype type) {



}

// 3. 选择攻击对象函数实现
void tower::selectAttackTarget(vector<monster>amonster) {






}


// 4. 铲除塔函数实现
void tower::remove(Map& map, int towergold) {



}

// 5. 选择设置子弹函数实现（利用包含关系）
void tower::createBullet(bullet towerbullet) {






}

void tower::showtower(Vector2 towerposition){




}


void tower::upgrade(towertype type) {
    level++;
    if (level < 4) {
        towerdamage += towerdamage;
        attackRange *= 1.5;
        attackSpeed *= 0.9;
    }
    else {
        cout << "已是最高等级";
    }

}

