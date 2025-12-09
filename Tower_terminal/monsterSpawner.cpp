
#include "monsterSpawner.h"
#include "monster.h"
#include <QPainter>
#include <QDebug>
#include <QTime>
#include "utils.h"
#include <QRandomGenerator>

// 构造函数
MonsterSpawnerTower::MonsterSpawnerTower(QGraphicsItem* parent)
    : QObject(), QGraphicsPixmapItem(parent),
    currentWave(1),
    monstersInCurrentWave(0),
    totalMonstersInWave(0),
    spawnInterval(SPAWNINTERVAL),  // 初始3秒生成一个怪物
    waveInterval(WAVEINTERVAL), // 每30秒一波
    spawnTimer(nullptr),
    waveTimer(nullptr)
{
    setTransformOriginPoint(16, 16);

    // 设置z值
    setZValue(1);

    // 加载图片和初始化

    initialize();
}

// 析构函数
MonsterSpawnerTower::~MonsterSpawnerTower()
{
    stopSpawning();
    delete spawnTimer;
    delete waveTimer;
}




// 初始化生成器
void MonsterSpawnerTower::initialize()
{
     // 计算第一波的怪物总数
    totalMonstersInWave = 5 + currentWave;// 基础5个 + 波数
    monstersInCurrentWave = 0;

    spawnTimer = new QTimer(this);
    connect(spawnTimer, &QTimer::timeout, this, &MonsterSpawnerTower::spawnMonster);

    waveTimer = new QTimer(this);
    connect(waveTimer, &QTimer::timeout, this, &MonsterSpawnerTower::updateWave);
}

// 开始生成怪物
void MonsterSpawnerTower::startSpawning()
{
    spawnTimer->start(static_cast<int>(spawnInterval * 1000));
    waveTimer->start(static_cast<int>(waveInterval * 1000));

    qDebug() << "怪物生成塔已启动！第" << currentWave << "波开始";
}

// 停止生成怪物
void MonsterSpawnerTower::stopSpawning()
{
    if (spawnTimer && spawnTimer->isActive()) {
        spawnTimer->stop();
    }
    if (waveTimer && waveTimer->isActive()) {
        waveTimer->stop();
    }
}

// 生成单个怪物
void MonsterSpawnerTower::spawnMonster()
{
    if (monstersInCurrentWave >= totalMonstersInWave) {
        spawnTimer->stop();
        return;
    }
 // 根据当前波数决定生成的怪物类型
    Monster* newMonster = nullptr;
    QRandomGenerator* random = QRandomGenerator::global();
    int monsterType = random->bounded(100);
    // 10波后，20%概率生成怪物3
    if (currentWave >= 10 && monsterType < 20) {
        newMonster = new Monster3();
     // 5波后，40%概率生成怪物2
    } else if (currentWave >= 5 && monsterType < 40) {
        newMonster = new Monster2();
    } else {
        newMonster = new Monster1();
    }

    if (newMonster) {
        emit monsterSpawned(newMonster);
        monstersInCurrentWave++;
    }
}

// 更新波数和难度
void MonsterSpawnerTower::updateWave()
{
    currentWave++;
    // 计算新一波的怪物数量
    totalMonstersInWave = static_cast<int>(5 + currentWave * 1.5f);// 线性增长
    monstersInCurrentWave = 0;
    spawnInterval = qMax(0.5f, 3.0f - (currentWave - 1) * 0.2f);
     // 如果之前停止了生成计时器，重新开始
    if (spawnTimer && !spawnTimer->isActive()) {
        spawnTimer->start(static_cast<int>(spawnInterval * 1000));
    }
    // 发射波数更新信号
    emit waveChanged(currentWave);

    qDebug() << "=== 第" << currentWave << "波开始 ===";
    qDebug() << "怪物总数：" << totalMonstersInWave;
    qDebug() << "生成间隔：" << spawnInterval << "秒";
}
