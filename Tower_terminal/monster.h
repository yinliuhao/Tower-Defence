#pragma once
#include<vector>
#include<structVector.h>

using namespace std;


enum class monstertype {   //怪物种类
    monster1,
    monster2,
    monster3,
    monster4
};

//暂时删除冻结等状态，简化，避免太乱

class Map;
class bullet;

//--------怪物主类-------
class monster {
private:
    //---基础属性---
    monstertype type;       //
    float monsterhealth;    //
    float monsterspeed;     //
    int monstergold;        //击败掉落金钱
    //---移动相关---
    Vector2 monsterposition;//当前位置坐标
    vector<Vector2>path;    //路径点
    bool reachedEnd;        //是否到达终点

public:
    monster(){};
    monster(monstertype type,
            float monsterhealth,
            float monsterspeed,
            int monstergold,
            Vector2 monsterposition,
            vector<Vector2>path,
            bool reachedEnd) ;

    ~monster();

    void setpath(monster amonster,Map &m);//map的友元函数，要在map里定义友元friend，用来构建monsterposition、path

    void walking(float monsterspeed,Vector2 monsterposition,vector<Vector2>path,bool reachedEnd);

    void showmonster(float monsterspeed,Vector2 monsterposition);//绘制怪物

    void changestate(float monsterhealth,bool reachedEnd);//调整怪物生死状态，到达终点状态

    friend void bulletfly(Vector2 bulletposition,Vector2 targetposition,Vector2 monsterposition,float bulletspeed);//子弹追踪monster

    friend void damagethemonster(float monsterhealth);//怪物的友元函数，是子弹的函数，子弹命中怪物时访问血量


};
