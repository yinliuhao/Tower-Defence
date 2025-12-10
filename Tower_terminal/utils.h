#ifndef UTILS_H
#define UTILS_H

//游戏界面
#define WIDTH 1152
#define HEIGHT 648

//地图
#define TILESIZE 32
#define WIDTHNUM 33
#define HEIGHTNUM 16
#define MAPWIDTH 2880
#define MAPHEIGHT 1920

//按钮
#define BTNWIDTH 107
#define BTNHEIGHT 107

//UI
#define INITMONEY 100
#define INITCAMPHEALTH 100
#define INITPLAYERHEALTH 100
#define INITWAVE 10
#define INITWOOD 0
#define INITSTONE 0
#define INITBRONZE 0
#define INITSILVER 0
#define INITGOLD 0

#define CAMPHEALTH 0
#define PLAYERHEALTH 1
#define MONEY 2
#define WAVE 3
#define WOOD 4
#define STONE 5
#define BRONZE 6
#define SILVER 7
#define GOLD 8

#define BARWIDTH (5 * TILESIZE + BTNHEIGHT)
#define BARHEIGHT (6 * TILESIZE)
#define DIGIT 2
#define BARNUM 3

#define NUMWIDTH 27
#define NUMHEIGHT 46
#define STATUWIDTH 50
#define STATUHEIGHT 50

#define STATUX 24
#define STATUY 16
#define GAPY 54

#define NUMX 90
#define NUMY 20
#define NUMGAPX 31

//图层
#define MAPZVALUE -10
#define TOWERZVALUE 10
#define MONSTERZVALUE 5
#define PLAYERZVALUE 5
#define SOURCEZVALUE -10

//玩家
#define PLAYERWIDTH TILESIZE
#define PLAYERHEIGHT TILESIZE
#define PLAYERSPEED 150.0
#define ROLLSPEED 500.0

//怪物
#define MONSTER_ATTACK_INTERVAL 2500
//monster1
#define MONSTER1_HEALTH 100.0
#define MONSTER1_SPEED 200.0
#define MONSTER1_GOLD 10
#define MONSTER1_DAMAGE 1
//monster2
#define MONSTER2_HEALTH 200.0
#define MONSTER2_SPEED 40.0
#define MONSTER2_GOLD 20
#define MONSTER2_DAMAGE 2
//monster3
#define MONSTER3_HEALTH 400.0
#define MONSTER3_SPEED 20.0
#define MONSTER3_GOLD 40
#define MONSTER3_DAMAGE 3
//monster4
#define MONSTER4_HEALTH 400.0
#define MONSTER4_SPEED 20.0
#define MONSTER4_GOLD 40
#define MONSTER4_DAMAGE 3

//怪物管理器
#define SPAWNINTERVAL 3.0
#define WAVEINTERVAL 30.0

//计时器——玩家
#define SPEEDDELTA 8   //玩家位置更新间隔 单位：ms
#define WAKEDELTA 80   //玩家行走动画更新间隔
#define ROLLDELTA 15   //玩家翻滚动画更新间隔
#define ROLLTIMES 3  //每次按shift键人物翻滚效果圈数

//计时器——怪物
#define MONSTERDELTA 150
#define MONSTERMOVE 20
#define ATTACKDELAY 1500
#define ATTACKDELTA 100
#endif // UTILS_H
