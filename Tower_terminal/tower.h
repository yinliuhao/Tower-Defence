#pragma once
#include<vector>

#include<structVector.h>

using namespace std;
enum towertype {
    TOWER1,
    TOWER2,
    TOWER3,
    TOWER4,
};

class monster;
class Map;
class bullet;


class tower {
private:

    // 塔基础属性（与initTowerAttr函数中的属性对应）
    towertype type;            // 塔的种类
    float towerhealth;         // 塔的血量
    int towergold;             // 安置塔所需金币
    int level;
    double towerdamage;        //tower创建子弹是利用该数据给子弹的伤害赋值
    double attackRange;        // 塔的攻击范围（半径）
    double attackSpeed;        // 塔的射速（攻击间隔，秒/次）
    Vector2 towerposition;     // 塔在地图上的坐标
    bool isPlaced;             // 塔是否已放置到地图

    /*bullet towerbullet; */     //塔包含子弹，塔可以生成子弹

public:
    tower(){};
    tower(towertype type,
          float towerhealth,
          int towergold,
          int level,
          double towerdamage,
          double attackRange,
          double attackSpeed,
          Vector2 towerposition,
          bool isPlaced/*,
        bullet towerbullet*/);


    //升级塔
    void upgrade(towertype type);
    //  寻找攻击目标
    void selectAttackTarget(vector<monster>amonster);//塔不拥有monster所有依赖或友元monster，这是怪物的友元函数，访问怪物
    //设置塔种类
    void setTowerType(towertype type);//应该会被put()调用
    //放置塔
    void put(Map& map, Vector2 towerposition);//map的友元函数，可以访问map,在map里写friend，在tower里实现
    //绘制塔
    void showtower(Vector2 towerposition);
    //铲除塔
    void remove(Map& map, int towergold);//map的友元函数，可以访问map,在map里写friend，在tower里实现

    void createBullet(bullet towerbullet);//创建子弹，调用子弹的函数
};
