#include <QGraphicsView>
#include <QString>
#include <QKeyEvent>
#include "gameWidght.h"
#include "ui_gameWidght.h"
#include "towerButton.h"
#include "monster.h"
#include "utils.h"
#include <QDebug>

Game::Game(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Game),
    width(WIDTH),
    height(HEIGHT),
    currentWave(1),
    monstersInWave(5),  // 第一波5个怪物
    monstersSpawned(0)
{
    ui->setupUi(this);
    setFixedSize(width, height);
    setFocusPolicy(Qt::StrongFocus);
    setFocus();

    // 1. 创建场景
    scene = new QGraphicsScene(this);

    // 2. 创建视图
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFixedSize(width, height);  // 配合地图大小

    // 3. 取消滚动区域边距（重要）
    view->setFrameStyle(QFrame::NoFrame);

    // 初始化地图
    Map *m = new Map();
    map.push_back(m);
    mapNum++;
    scene->addItem(map[0]);
    scene->setSceneRect(0, 0, map[0]->getWidth(), map[0]->getHeight());

    // 可以开启网格显示（用于调试）
    // 注释掉就不显示
    map[0]->setShowGrid(true);

    // 初始化炮塔按钮
    towerNum = 0;
    tower.push_back(new TowerButton("archer", this));
    towerNum++;
    tower.push_back(new TowerButton("cannon", this));
    towerNum++;
    tower.push_back(new TowerButton("mortar", this));
    towerNum++;

    // 隐藏炮塔按钮
    for(int i = 0; i < towerNum; i++)
    {
        tower[i]->move(width - BTNWIDTH, height - (towerNum - i) * BTNHEIGHT);
    }
    for(int i = 0; i < towerNum; i++)
    {
        tower[i]->hide();
    }
    buttonVisible = false;

    // 初始化UI界面
    myUI = new PlayerUI(this);

    // 隐藏UI界面
    myUI->move(width - myUI->getWidth(), 0);
    myUI->hide();
    uiVisible = false;

    // 初始化玩家
    me = new Player(map[0]);  // 将地图指针传递给Player

    // 显示玩家
    scene->addItem(me);

    // 玩家设置为视角中心
    viewTimer = new QTimer(this);
    connect(viewTimer, &QTimer::timeout, this, [&](){
        view->centerOn(me);
    });
    viewTimer->start(SPEEDDELTA);

    // 初始化怪物生成计时器
    monsterSpawnTimer = new QTimer(this);
    connect(monsterSpawnTimer, &QTimer::timeout, this, &Game::spawnMonster);
    monsterSpawnTimer->start(2000);  // 每2秒生成一个怪物

    // 初始化游戏状态更新计时器
    gameUpdateTimer = new QTimer(this);
    connect(gameUpdateTimer, &QTimer::timeout, this, &Game::updateGame);
    gameUpdateTimer->start(100);  // 每100ms更新一次游戏状态

    qDebug() << "游戏初始化完成，开始第" << currentWave << "波怪物";
}

Game::~Game()
{
    // 清理怪物
    for (Monster* monster : monsters) {
        if (monster) {
            delete monster;
        }
    }
    monsters.clear();

    // 清理地图
    for(int i = 0; i < mapNum; i++)
    {
        delete map[i];
        map[i] = nullptr;
    }

    // 停止计时器
    if (monsterSpawnTimer) {
        monsterSpawnTimer->stop();
        delete monsterSpawnTimer;
    }

    if (gameUpdateTimer) {
        gameUpdateTimer->stop();
        delete gameUpdateTimer;
    }

    if (viewTimer) {
        viewTimer->stop();
        delete viewTimer;
    }

    delete ui;
}

// 生成怪物函数
void Game::spawnMonster()
{
    // 检查是否已经生成了足够的怪物
    if (monstersSpawned >= monstersInWave) {
        // 当前波次完成，等待所有怪物被消灭
        if (monsters.empty()) {
            // 进入下一波
            currentWave++;
            monstersInWave = 5 + currentWave * 2;  // 每波增加2个怪物
            monstersSpawned = 0;
            qDebug() << "进入第" << currentWave << "波，将生成" << monstersInWave << "个怪物";
        }
        return;
    }

    // 生成怪物
    Monster1* newMonster = new Monster1(map[0]);  // 使用地图指针创建怪物
    scene->addItem(newMonster);
    monsters.push_back(newMonster);

    // 连接怪物信号
    connect(newMonster, &Monster::reachedDestination, this, [this]() {
        // 怪物到达终点，减少玩家生命值
        // 这里需要调用PlayerUI减少生命值
        qDebug() << "怪物到达终点！";
        // 这里可以添加生命值减少的逻辑
    });

    connect(newMonster, &Monster::died, this, [this](int gold) {
        // 怪物死亡，增加金币
        // 这里需要调用PlayerUI增加金币
        qDebug() << "怪物死亡，获得金币：" << gold;
        // 这里可以添加金币增加的逻辑
    });

    // 开始怪物移动
    newMonster->startMove();

    monstersSpawned++;
    qDebug() << "生成第" << monstersSpawned << "/" << monstersInWave << "个怪物";
}

// 更新游戏状态
void Game::updateGame()
{
    // 移除已死亡的怪物
    auto it = monsters.begin();
    while (it != monsters.end()) {
        Monster* monster = *it;
        if (!monster || monster->isDead()) {
            // 从场景中移除（如果还没有被移除）
            if (monster && scene->items().contains(monster)) {
                scene->removeItem(monster);
            }
            // 从容器中移除
            it = monsters.erase(it);
            //delete monster;（这个好像有bug带上后怪物到达终点游戏就退出了）
        } else {
            ++it;
        }
    }

    // 检查玩家与怪物的碰撞
    if (me) {
        for (Monster* monster : monsters) {
            if (monster && me->collidesWithItem(monster)) {
                // 玩家与怪物碰撞，减少玩家生命值
                qDebug() << "玩家与怪物碰撞！";
                // 这里可以添加碰撞处理逻辑
                break;
            }
        }
    }
}

void Game::keyPressEvent(QKeyEvent *ev)
{
    if (!me) return;
    if(me->isRolling()) return;

    if (ev->key() == Qt::Key_W){
        me->setMoveUp(true);
    }
    if (ev->key() == Qt::Key_S){
        me->setMoveDown(true);
    }
    if (ev->key() == Qt::Key_A){
        me->setMoveLeft(true);
    }
    if (ev->key() == Qt::Key_D){
        me->setMoveRight(true);
    }

    if(ev->key() == Qt::Key_Shift) emit me->rolling();

    QWidget::keyPressEvent(ev);
}

void Game::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Tab)
    {
        uiVisible = !uiVisible;
        if(uiVisible)
        {
            myUI->show();
            myUI->raise();
        }
        else myUI->hide();
        return;  // 处理完就退出，避免影响后面逻辑
    }

    if(ev->key() == Qt::Key_Q)
    {
        buttonVisible = !buttonVisible;
        if(buttonVisible)
        {
            for(int i = 0; i < towerNum; i++)
            {
                tower[i]->show();
                tower[i]->raise();
            }
        }
        else
        {
            for(int i = 0; i < towerNum; i++)
            {
                tower[i]->hide();
            }
        }
    }

    if (!me) return;
    if(me->isRolling()) return;

    if (ev->key() == Qt::Key_W){
        me->setMoveUp(false);
    }
    if (ev->key() == Qt::Key_S){
        me->setMoveDown(false);
    }
    if (ev->key() == Qt::Key_A){
        me->setMoveLeft(false);
    }
    if (ev->key() == Qt::Key_D){
        me->setMoveRight(false);
    }
    QWidget::keyReleaseEvent(ev);
}
