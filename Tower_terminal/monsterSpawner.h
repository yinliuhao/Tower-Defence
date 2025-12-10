#ifndef MONSTERSPAWNER_H
#define MONSTERSPAWNER_H


#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <vector>
#include "utils.h"

// 前向声明
class Monster;

class MonsterSpawnerTower : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    MonsterSpawnerTower(QGraphicsItem* parent = nullptr);
    ~MonsterSpawnerTower();

    void startSpawning();      // 开始生成怪物
    void stopSpawning();       // 停止生成怪物

   // === 对外调用 ===
    int getCurrentWave() const { return currentWave; }
    void cleanupMonsters();       // 删除死亡怪物

    // 全部怪物（真正保存的容器）
    std::vector<Monster*> monsters;

signals:
    // 信号：怪物被生成（用于在Game类中添加到场景）
    void monsterSpawned(Monster* monster);
    // 信号：波数更新
    void waveChanged(int wave);

private slots:
    void spawnMonster();       // 生成单个怪物
    void updateWave();         // 更新波数和难度
    void updateAllMonstersPosition();     // 每帧调用
    void updateAllAnimationFrame();
    void performAllAttack();

private:
    void initialize();         // 初始化生成器
    void addMonsterToGrid(Monster* m);
    void removeMonsterFromGrid(Monster* m);
    void initializeTimers();

    // 空间网格（60×90）
    std::vector<Monster*> grid[MAPHEIGHT / TILESIZE][MAPWIDTH / TILESIZE];

    // 生成器属性
    int currentWave;           // 当前波数
    int monstersInCurrentWave; // 当前波已生成的怪物数
    int totalMonstersInWave;   // 当前波总怪物数

    // 难度参数
    float spawnInterval;       // 生成间隔（秒）
    float waveInterval;        // 波间隔（秒）

    // 计时器
    QTimer* spawnTimer;        // 生成计时器
    QTimer* waveTimer;         // 波更新计时器
    QTimer* animationTimer;    // 动画更新计时器
    QTimer* moveTimer;         // 位置移动计时器
    QTimer* attackTimer;       // 攻击计时器
};

#endif // MONSTERSPAWNER_H

