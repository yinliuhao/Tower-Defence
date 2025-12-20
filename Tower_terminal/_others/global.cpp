#include "_others/global.h"
#include "_monster/monster.h"
#include "_tower/explorationmarker.h"

Map* gMap;   //  真正定义（分配内存）
MonsterSpawnerTower* monsterSpawner;//怪物生成器
Player * me;
ResourceManager * resourceManager;
BuildManager * buildManager;
ExplorationMarker * explorationmarker;
