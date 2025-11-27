#include <iostream>
#include"monster.h"
#include<algorithm>
#include"bullet.h"
#include"map.h"

using namespace std;


monster::monster(monstertype type,
                 float monsterhealth,
                 float monsterspeed,
                 int monstergold,
                 Vector2 monsterposition,
                 vector<Vector2>path,
                 bool reachedEnd)
    :type(type),
    monsterhealth(monsterhealth),
    monsterspeed(monsterspeed),
    monstergold(monstergold),
    monsterposition(monsterposition),
    path(path),
    reachedEnd(reachedEnd){}

monster::~monster() {}

void monster::setpath(monster amonster,Map &m)//map的友元函数，要在map里定义友元friend，用来构建monsterposition、path
{

    //map考虑线性生成地图，给转折点标号，根据标号和转折点坐标setpath


}//需要map里定义了转折点再写


void monster::walking(float monsterspeed,Vector2 monsterposition,vector<Vector2>path,bool reachedEnd)
{



    //我的想法是先判断monsterposition在哪两个path间，根据path判断水平还是竖直移动，根据path的顺序和坐标判断移动方向




}


void monster::showmonster(float monsterspeed,Vector2 monsterposition)//绘制怪物
{

    //利用monsterspeed和monsterposition绘制怪物，可能调用walking来判断方向


}



void monster::changestate(float monsterhealth,bool reachedEnd)//调整怪物生死状态，到达终点状态
{
    if(monsterhealth<=0) {
        monster::~monster();
    }
    else if(reachedEnd){
        monster::~monster();
    }

    //不知道可不可以用析构函数直接删除怪物，这里可以加死亡动画

}
