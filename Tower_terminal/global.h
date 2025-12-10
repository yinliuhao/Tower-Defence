#ifndef GLOBALS_H
#define GLOBALS_H

#include "map.h"   // 记得包含 Map 类的头文件
#include "monsterSpawner.h"
#include "monster.h"
#include "player.h"

extern Map* gMap;   //  只声明，不定义！
extern MonsterSpawnerTower* monsterSpawner;//怪物生成器
extern Player * me;

#endif
