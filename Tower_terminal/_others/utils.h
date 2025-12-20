#ifndef UTILS_H
#define UTILS_H

// ---------------------------
// 游戏核心配置 (Game Core Configuration)
// ---------------------------

// 游戏界面
#define WIDTH 1152
#define HEIGHT 648

// ---------------------------
// 地图和瓦片配置 (Map and Tile Configuration)
// ---------------------------

// 地图
#define TILESIZE 32
#define WIDTHNUM 33
#define HEIGHTNUM 16
#define MAPWIDTH 2880
#define MAPHEIGHT 1920

// ---------------------------
// UI 配置 (User Interface Configuration)
// ---------------------------

// 按钮
#define BTNWIDTH 107
#define BTNHEIGHT 107

// UI
#define INITCAMPHEALTH 80
#define INITPLAYERHEALTH 20
#define INITWAVE 0
#define INITWOOD 99
#define INITSTONE 99
#define INITBRONZE 50
#define INITSILVER 50
#define INITGOLD 50

#define UIWIDTH (6 * TILESIZE + BTNHEIGHT)
#define UIHEIGHT (8 * TILESIZE)
#define DIGIT 2
#define BARNUM 8

#define NUMWIDTH 27
#define NUMHEIGHT 46
#define STATUWIDTH 48
#define STATUHEIGHT 48

#define STATUX 24
#define STATUY 16
#define GAPY 54

#define NUMX 90
#define NUMY 20
#define NUMGAPX 31

// ---------------------------
// 图层 Z 值 (Z-Value Layering)
// ---------------------------

// 图层
#define MAPZVALUE -10
#define TOWERZVALUE 10
#define BULLETZVALUE 10
#define MONSTERZVALUE 5
#define PLAYERZVALUE 12
#define RESOURCEZVALUE -5
#define HOMEZVALUE -5
#define MARKERZVALUE 0
#define PREVIEWTOWERZVALUE 1000

// ---------------------------
// 玩家配置 (Player Configuration)
// ---------------------------

// 玩家
#define PLAYERWIDTH TILESIZE
#define PLAYERHEIGHT TILESIZE
#define WALKSPEED 150.0
#define ROLLSPEED 500.0
#define ROLLTIMES 1.5f

// 计时器——玩家
#define POSITION_DELTA 8    // 玩家位置更新间隔 单位：ms
#define PICTURE_DELTA 5     // 玩家动画更新间隔 单位：ms
#define WALK_FRAME_INTERVAL 80  // 行走80ms 一帧
#define ROLL_FRAME_INTERVAL 50    // 滑行50ms 一帧
#define CUT_FRAME_INTERVAL 100  // 砍树100ms 一帧
#define DIG_FRAME_INTERVAL 100  // 采石100ms 一帧
#define FIND_TARGET_INTERVAL 100

// ---------------------------
// 怪物配置 (Monster Configuration)
// ---------------------------

// 怪物
#define MONSTER_ATTACK_INTERVAL 2500

// monster1
#define MONSTER1_HEALTH 100.0
#define MONSTER1_SPEED 150.0
#define MONSTER1_GOLD 10
#define MONSTER1_DAMAGE 1

// monster2
#define MONSTER2_HEALTH 200.0
#define MONSTER2_SPEED 40.0
#define MONSTER2_GOLD 20
#define MONSTER2_DAMAGE 2

// monster3
#define MONSTER3_HEALTH 400.0
#define MONSTER3_SPEED 20.0
#define MONSTER3_GOLD 40
#define MONSTER3_DAMAGE 3

// monster4
#define MONSTER4_HEALTH 400.0
#define MONSTER4_SPEED 20.0
#define MONSTER4_GOLD 40
#define MONSTER4_DAMAGE 3

//bat
#define BAT_HEALTH 400.0
#define BAT_SPEED 50.0
#define BAT_GOLD 40
#define BAT_DAMAGE 3

// 怪物管理器
#define SPAWNINTERVAL 3.0
#define WAVEINTERVAL 30.0

// 计时器——怪物
#define MONSTERDELTA 150
#define MONSTERMOVE 20
#define ATTACKDELAY 2000
#define ATTACKDELTA 100

// ---------------------------
// 炮塔配置 (Tower Configuration)
// ---------------------------

// tower1
#define TOWER1_MAHEALTH 100
#define TOWER1_BUILCOST 50
#define TOWER1_ATTACKRANGE 450
#define TOWER1_ATTACKINTERVAL 500
#define TOWER1_BULLET_DAMAGE 50
#define TOWER1_BULLET_SPEED 15
#define TOWER1_BUILD_WOOD 3
#define TOWER1_BUILD_STONE 3

// tower2
#define TOWER2_MAHEALTH 100
#define TOWER2_BUILCOST 50
#define TOWER2_ATTACKRANGE 150
#define TOWER2_ATTACKINTERVAL 1500
#define TOWER2_BULLET_DAMAGE 100
#define TOWER2_BULLET_SPEED 10
#define TOWER2_BUILD_WOOD 5
#define TOWER2_BUILD_STONE 5

// tower3
#define TOWER3_MAHEALTH 100
#define TOWER3_BUILCOST 50
#define TOWER3_ATTACKRANGE 300
#define TOWER3_ATTACKINTERVAL 3000
#define TOWER3_BULLET_DAMAGE 100
#define TOWER3_BULLET_SPEED 5
#define OUTER_DIVIDE_INNER 3
#define TOWER3_BUILD_WOOD 8
#define TOWER3_BUILD_STONE 8

// 计时器——炮塔
#define TRACK_TIMER_INTERVAL 30
#define FRAME_TIMER_INTERVAL 100

// ---------------------------
// 子弹配置 (Bullet Configuration)
// ---------------------------

// 子弹
#define BULLET_TOLERANCE 5.0
#define BULLET_SPEED_DEFAULT 8.0f
#define BULLET_DAMAGE_DEFAULT 5.0f

#define BULLET_PICTURE_WIDTH 25
#define BULLET_PICTURE_HEIGHT 20

// 计时器——子弹
#define UPDATE_INTERVAL_MS 16
#define BULLET_FRAME_DELAY 25

// ---------------------------
// 资源配置 (Resource Configuration)
// ---------------------------

// 资源
#define BIG_STONE_TIME 60
#define TREE_TIME 40


// ---------------------------
// 枚举定义 (Enum Definitions)
// ---------------------------

// 格子状态枚举
enum TileState {    // 描述格子的特殊性质
    TILE_ELSE = 0,
    TILE_BLOCKED = 1,   // 障碍物
    TILE_BUILDABLE = 2, // 可建造
    TILE_LADDER = 3,    // 梯子或台阶
    TILE_START = 4,     // 怪物起点
    TILE_END = 5        // 怪物终点
};

// 图层枚举
enum TileLayer {
    TILE_FIRST = 3,     // 第一高，雪山 山
    TILE_SECOND = 2,    // 第二高，草地 沙漠 苔原
    TILE_THIRD = 1,     // 第三高，沼泽 水池
    TILE_FUORTH = 0,    // 第四高，酸液
};

enum StartPoint {
    GRASS,
    SWAMP,
    SNOW,
    SDESERT
};

enum class MonsterType {
    MONSTER1,   ///< 怪物类型1
    MONSTER2,   ///< 怪物类型2
    MONSTER3,   ///< 怪物类型3
    MONSTER4,    ///< 怪物类型4
    BAT_TO_PLAYER,
    BAT_TO_CAMP
};

enum class MonsterState{
    MOVING,
    ATTACKING,
    DYING,
    DEAD
};

enum class PlayerState
{
    WALKING,
    ROLLING,
    CUTTING,
    DIGGING
};

enum UiResource
{
    CAMPHEALTH = 0,
    PLAYERHEALTH = 1,
    WAVE = 2,
    WOOD = 3,
    BRONZE = 4,
    SILVER = 5,
    GOLD = 6,
    STONE = 7,
};

enum class ResourceType
{
    GRASS_STONE,
    SNOW_STONE,
    SWAMP_STONE,
    GRASS_TREE,
    SWAMP_TREE
};

enum class TowerType {
    TOWER1,
    TOWER2,
    TOWER3,
    TOWER_NONE,
};

#endif // UTILS_H
