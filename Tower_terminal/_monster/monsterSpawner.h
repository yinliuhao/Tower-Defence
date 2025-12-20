#ifndef MONSTERSPAWNER_H
#define MONSTERSPAWNER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <vector>
#include "_others/utils.h"

// 前向声明
class Monster;

/**
 * 怪物生成塔类
 * 负责怪物的波次生成、位置更新、动画更新、攻击逻辑等核心功能
 */
class MonsterSpawnerTower : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    // 构造/析构函数
    MonsterSpawnerTower(QGraphicsItem* parent = nullptr);
    ~MonsterSpawnerTower();

    // 生成控制接口
    void startSpawning();      // 开始生成怪物
    void stopSpawning();       // 停止生成怪物

    // === 对外访问接口 ===
    int getCurrentWave() const { return currentWave; }

    // 怪物容器（全局存储）
    std::vector<Monster*> monsters;

    // 怪物网格索引：[X坐标][Y坐标]，下标范围 [0, MAPWIDTH/TILESIZE) × [0, MAPHEIGHT/TILESIZE)
    std::vector<Monster*> grid[MAPWIDTH / TILESIZE][MAPHEIGHT / TILESIZE];

signals:
    void monsterSpawned(Monster* monster);  // 怪物生成信号（用于Game类添加到场景）
    void waveChanged(int wave);             // 波数更新信号

private slots:
    // 核心逻辑槽函数
    void spawnMonster();                    // 生成单个怪物
    void updateWave();                      // 更新波数和难度
    void updateAllMonstersPosition();       // 每帧更新所有怪物位置
    void updateAllAnimationFrame();         // 更新所有怪物动画帧
    void performAllAttack();                // 执行所有怪物攻击逻辑
    //void onMonsterDied();                   // 处理怪物死亡事件

private:
    // 初始化函数
    void initialize();                      // 初始化生成器核心配置
    void addMonsterToGrid(Monster* m);      // 将怪物添加到网格索引
    void removeMonsterFromGrid(Monster* m); // 从网格索引移除怪物
    void initializeTimers();                // 初始化所有计时器

    // 波次控制参数
    int currentWave;                        // 当前波数
    int monstersInCurrentWave;              // 当前波已生成的怪物数量
    int totalMonstersInWave;                // 当前波总怪物数量

    // 待删除怪物列表
    std::vector<Monster*> toDeleteMonsters; // 等待删除的怪物列表

    // 难度调节参数
    float spawnInterval;                    // 怪物生成间隔（单位：秒）
    float waveInterval;                     // 波次间隔（单位：秒）

    // 计时器对象
    QTimer* spawnTimer;                     // 怪物生成计时器
    QTimer* waveTimer;                      // 波次更新计时器
    QTimer* animationTimer;                 // 动画帧更新计时器
    QTimer* moveTimer;                      // 怪物移动计时器
    QTimer* attackTimer;                    // 怪物攻击计时器
};

#endif // MONSTERSPAWNER_H
