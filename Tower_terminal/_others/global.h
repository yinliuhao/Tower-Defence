#ifndef GLOBALS_H
#define GLOBALS_H

#include "_others/map.h"   // 记得包含 Map 类的头文件
#include "_monster/monsterSpawner.h"
#include "_monster/monster.h"
#include "_player_and_resource/player.h"
#include "_player_and_resource/resourcemanager.h"
#include "_tower/buildmanager.h"
#include "_tower/explorationmarker.h"

extern Map* gMap;   //  只声明，不定义！
extern MonsterSpawnerTower* monsterSpawner;//怪物生成器
extern ResourceManager * resourceManager;
extern Player * me;
extern BuildManager * buildManager;
extern ExplorationMarker * explorationmarker;

#endif
