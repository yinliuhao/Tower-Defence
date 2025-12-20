#include "_monster/monsterSpawner.h"
#include "_monster/monster.h"
#include "_monster/normalmonster.h"
#include "_monster/flymonsters.h"
#include <QPainter>
#include <QDebug>
#include <QTime>
#include "_others/utils.h"
#include <QRandomGenerator>
#include "_others/global.h"
#include <QObject>
#include <QGraphicsScene>
#include <QTextStream>
#include <QDateTime>
#include <algorithm>

// 怪物生成塔类实现
MonsterSpawnerTower::MonsterSpawnerTower(QGraphicsItem* parent)
    : QObject(), QGraphicsPixmapItem(parent)
    , currentWave(1)
    , monstersInCurrentWave(0)
    , totalMonstersInWave(0)
    , spawnInterval(SPAWNINTERVAL)  // 初始3秒生成一个怪物
    , waveInterval(WAVEINTERVAL)    // 每30秒一波
    , spawnTimer(nullptr)
    , waveTimer(nullptr)
    , animationTimer(nullptr)       // 计时器指针初始化为空
    , moveTimer(nullptr)            // 移动计时器指针初始化为空
    , attackTimer(nullptr)
{
    // 设置变换原点
    setTransformOriginPoint(16, 16);

    // 设置z值
    setZValue(1);

    // 初始化
    initialize();
    initializeTimers();
}

// 析构函数
MonsterSpawnerTower::~MonsterSpawnerTower()
{
    stopSpawning();
    delete spawnTimer;
    delete waveTimer;
    
    // 清理待删除的怪物
    for (Monster* m : toDeleteMonsters) {
        if (m) {
            delete m;
        }
    }
    toDeleteMonsters.clear();
}

// ==================== 计时器相关 ====================

// 初始化计时器系统
void MonsterSpawnerTower::initializeTimers()
{
    // 创建动画计时器，每150毫秒触发一次动画更新
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &MonsterSpawnerTower::updateAllAnimationFrame);

    // 创建移动计时器，每80毫秒触发一次位置更新
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &MonsterSpawnerTower::updateAllMonstersPosition);

    // 创建攻击计时器
    attackTimer = new QTimer(this);
    connect(attackTimer, &QTimer::timeout, this, &MonsterSpawnerTower::performAllAttack);

    // 启动计时器
    animationTimer->start(MONSTERDELTA);  // 150ms间隔
    moveTimer->start(MONSTERMOVE);        // 80ms间隔
    attackTimer->start(ATTACKDELAY);      // 攻击间隔
    // 创建清理定时器，定期删除待删除怪物
    QTimer* cleanupTimer = new QTimer(this);
    connect(cleanupTimer, &QTimer::timeout, this, [this]() {
        if (!toDeleteMonsters.empty()) {
            qDebug() << "[MonsterSpawner] cleanupTimer deleting" << toDeleteMonsters.size() << "monsters";
        }
        for (Monster* m : toDeleteMonsters) {
            if (m) {
                qDebug() << "[MonsterSpawner] deleteLater:" << static_cast<void*>(m);
                m->deleteLater();
            }
        }
        toDeleteMonsters.clear();
    });
    cleanupTimer->start(1000);            // 每秒清理一次
}

// ==================== 初始化相关 ====================

// 初始化生成器
void MonsterSpawnerTower::initialize()
{
    // 计算第一波的怪物总数
    totalMonstersInWave = 5 + currentWave;  // 基础5个 + 波数
    monstersInCurrentWave = 0;

    // 创建生成计时器
    spawnTimer = new QTimer(this);
    connect(spawnTimer, &QTimer::timeout, this, &MonsterSpawnerTower::spawnMonster);

    // 创建波次计时器
    waveTimer = new QTimer(this);
    connect(waveTimer, &QTimer::timeout, this, &MonsterSpawnerTower::updateWave);
}

// ==================== 生成控制 ====================

// 开始生成怪物
void MonsterSpawnerTower::startSpawning()
{
    spawnTimer->start(static_cast<int>(spawnInterval * 1000));
    waveTimer->start(static_cast<int>(waveInterval * 1000));

    //qDebug() << "怪物生成塔已启动！第" << currentWave << "波开始";
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

// ==================== 怪物生成 ====================

// 生成单个怪物
void MonsterSpawnerTower::spawnMonster()
{
    // 检查当前波次怪物数量是否已满
    if (monstersInCurrentWave >= totalMonstersInWave) {
        spawnTimer->stop();
        return;
    }

    // 根据当前波数决定生成的怪物类型
    Monster* newMonster = nullptr;
    QRandomGenerator* random = QRandomGenerator::global();
    int monsterType = random->bounded(100);

    // 根据波数和随机值生成不同类型怪物
    if (currentWave >= 1 && monsterType < 20) {
        newMonster = new BatToPlayer();          // 1波后，20%概率
    }
    else if (currentWave >= 2 && monsterType < 50) {
        newMonster = new BatToCamp();            // 2波后，50%概率
    }
    else if (currentWave >= 5 && monsterType < 20) {
        newMonster = new Monster3();             // 5波后，20%概率
    }
    else if (currentWave >= 5 && monsterType < 40) {
        newMonster = new Monster2();             // 5波后，40%概率
    }
    else {
        newMonster = new Monster1();             // 默认怪物1
    }

    // 如果成功创建怪物
    if (newMonster) {
        //connect(newMonster, &Monster::died, this, &MonsterSpawnerTower::onMonsterDied);
        emit monsterSpawned(newMonster);
        monsters.push_back(newMonster);
        addMonsterToGrid(newMonster);
        monstersInCurrentWave++;
    }
}

// ==================== 网格管理 ====================

// 将怪物添加到网格
void MonsterSpawnerTower::addMonsterToGrid(Monster* m)
{
    QPoint m_grid = gMap->pixelToGrid(QPointF(m->x(), m->y()));
    int gx = m_grid.x();
    int gy = m_grid.y();

    // 网格尺寸：X 方向 MAPWIDTH/TILESIZE(=90)，Y 方向 MAPHEIGHT/TILESIZE(=60)
    if (gx < 0 || gx >= MAPWIDTH / TILESIZE || gy < 0 || gy >= MAPHEIGHT / TILESIZE) {
        return;
    }

    grid[gx][gy].push_back(m);
}

// 从网格移除怪物
void MonsterSpawnerTower::removeMonsterFromGrid(Monster* m)
{
    QPoint m_grid = gMap->pixelToGrid(QPointF(m->x(), m->y()));
    int gx = m_grid.x();
    int gy = m_grid.y();

    if (gx < 0 || gx >= MAPWIDTH / TILESIZE || gy < 0 || gy >= MAPHEIGHT / TILESIZE) {
        return;
    }

    auto& vec = grid[gx][gy];
    vec.erase(std::remove(vec.begin(), vec.end(), m), vec.end());
}

// ==================== 怪物状态更新 ====================

// 更新所有怪物位置
void MonsterSpawnerTower::updateAllMonstersPosition()
{
    for (auto it = monsters.begin(); it != monsters.end();)
    {
        Monster* m = *it;

        if(!m){
            it = monsters.erase(it);
            continue;
        }
        else if (m->isDead()) {
            // 1. 从网格清除 (双重保险，防止 onMonsterDied 没清理干净)
            if (m) removeMonsterFromGrid(m);

            // 2. 从场景移除（不立即销毁，让QTimer完成后再删除）
            if (m && m->scene()) {
                m->scene()->removeItem(m);
            }

            // 3. 从列表中移除，并让迭代器指向下一个元素
            it = monsters.erase(it);
            
            // 4. 加入待删除列表（稍后在initializeTimers中定期清理）
            qDebug() << "[MonsterSpawner] scheduling for delete:" << static_cast<void*>(m) << "hp=" << m->getHealth();
            toDeleteMonsters.push_back(m);

            // 跳过本次循环的后续逻辑，直接处理下一个怪物
            continue;
        }
        else if(m->isDying()) {
            it++;
            continue;
        }

        // 记录旧网格位置
        QPoint old_grid = gMap->pixelToGrid(QPointF(m->x(), m->y()));
        int oldX = old_grid.x();
        int oldY = old_grid.y();

        // 移动怪物
        m->moveToNextPosition();

        // 获取新网格位置
        QPoint new_grid = gMap->pixelToGrid(QPointF(m->x(), m->y()));
        int newX = new_grid.x();
        int newY = new_grid.y();

        // 如果位置改变，更新网格
        if (newX != oldX || newY != oldY)
        {
            removeMonsterFromGrid(m);
            addMonsterToGrid(m);
        }

        ++it;
    }
}

// 更新所有怪物动画帧
void MonsterSpawnerTower::updateAllAnimationFrame()
{
    // 创建副本以避免容器在迭代时被修改导致的崩溃
    std::vector<Monster*> monstersCopy = monsters;
    
    for (Monster* m : monstersCopy)
    {
        if (!m || m->isDead() || m->isDying()) {
            continue;
        }
        m->updateAnimationFrame();
    }
}

// 执行所有怪物攻击
void MonsterSpawnerTower::performAllAttack()
{
    // 创建副本以避免容器在迭代时被修改导致的崩溃
    std::vector<Monster*> monstersCopy = monsters;
    
    for (Monster* m : monstersCopy)
    {
        // 严格的安全检查
        if (!m || m->isDead() || m->isDying()) {
            continue;
        }

        if (m->isAttacking()) {
            m->performAttack();
        }
    }
}

// ==================== 波次管理 ====================

// 更新波次
void MonsterSpawnerTower::updateWave()
{
    currentWave++;
    // 计算新一波的怪物数量（线性增长）
    totalMonstersInWave = static_cast<int>(5 + currentWave * 1.5f);
    monstersInCurrentWave = 0;

    // 缩短生成间隔（最小0.5秒）
    spawnInterval = qMax(0.5f, 3.0f - (currentWave - 1) * 0.2f);

    // 如果生成计时器已停止，重新启动
    if (spawnTimer && !spawnTimer->isActive()) {
        spawnTimer->start(static_cast<int>(spawnInterval * 1000));
    }

    // 发射波数更新信号
    emit waveChanged(currentWave);

    qDebug() << "=== 第" << currentWave << "波开始 ===";
    qDebug() << "怪物总数：" << totalMonstersInWave;
    qDebug() << "生成间隔：" << spawnInterval << "秒";
}
