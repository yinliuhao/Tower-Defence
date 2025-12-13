#include <QGraphicsView>
#include <QString>
#include <QKeyEvent>
#include "gameWidght.h"
#include "ui_gameWidght.h"
#include "towerButton.h"
#include "global.h"
#include "utils.h"
#include <QDebug>


Game::Game(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::Game),
    width(WIDTH),
    height(HEIGHT)
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
     gMap = new Map;
    scene->addItem(gMap);
    scene->setSceneRect(0, 0, gMap->getWidth(), gMap->getHeight());

    // 可以开启网格显示（用于调试）
    // 注释掉就不显示
    gMap->setShowGrid(true);
    //这里有bug，但是找不到
    // 初始化怪物生成塔
    monsterSpawner = new MonsterSpawnerTower();
    scene->addItem(monsterSpawner);

    // 连接怪物生成信号
    connect(monsterSpawner, &MonsterSpawnerTower::monsterSpawned, this, [this](Monster* monster) {
        scene->addItem(monster);
        // 连接怪物攻击信号
       // connect(monster, &Monster::attackedCamp, this, &Game::handleCampAttacked);

        // 连接怪物开始攻击信号
        connect(monster, &Monster::startedAttackingCamp, this, [monster]() {
            qDebug() << "怪物开始攻击营地！";
        });

        // 连接怪物停止攻击信号
        connect(monster, &Monster::stoppedAttackingCamp, this, [monster]() {
            qDebug() << "怪物停止攻击营地！";
        });

        // 连接怪物死亡信号
        connect(monster, &Monster::died, this, [this, monster](int gold) {
            qDebug() << "怪物死亡，奖励金币：" << gold;
            // 怪物会自动从场景中移除（在Monster::takeDamage中处理）
        });

        // 连接怪物到达终点信号
        connect(monster, &Monster::reachedDestination, this, []() {
            qDebug() << "怪物到达终点！";
        });
    });

    // 连接波数更新信号
    connect(monsterSpawner, &MonsterSpawnerTower::waveChanged, this, [](int wave) {
        qDebug() << "当前波数：" << wave;
    });

    // 延迟1秒后开始生成怪物
    QTimer::singleShot(1000, this, [this]() {
        monsterSpawner->startSpawning();
    });

    resourceManager = new ResourceManager();
    connect(resourceManager, &ResourceManager::resourceAdded, this, [this](Resource* r){
        scene->addItem(r);
    });
    resourceManager->initPicture();

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
    me = new Player();  // 将地图指针传递给Player

    // 显示玩家
    scene->addItem(me);

    // 玩家设置为视角中心
    viewTimer = new QTimer(this);
    connect(viewTimer, &QTimer::timeout, this, [&](){
        view->centerOn(me);
    });
    viewTimer->start(10);
}




Game::~Game()
{
    monsterSpawner->stopSpawning();
    delete monsterSpawner;
    delete gMap;
    if(gMap != NULL)  gMap = NULL;
    delete ui;
}

void Game::keyPressEvent(QKeyEvent *ev)
{
    if (!me) return;

    if (ev->key() == Qt::Key_W){
        me->setWalkUp(true);
    }
    if (ev->key() == Qt::Key_S){
        me->setWalkDown(true);
    }
    if (ev->key() == Qt::Key_A){
        me->setWalkLeft(true);
    }
    if (ev->key() == Qt::Key_D){
        me->setWalkRight(true);
    }

    if(ev->key() == Qt::Key_Shift){
        me->setState(PlayerState::ROLLING);
        me->setRollUp(me->getWalkUp());
        me->setRollDown(me->getWalkDown());
        me->setRollLeft(me->getWalkLeft());
        me->setRollRight(me->getWalkRight());
    }

    if(me->getState() == PlayerState::WALKING && ev->key() == Qt::Key_Space)
    {
        Resource * r = me->findNearbyResource();
        if(r != nullptr)
        {
            if(r->getType() == ResourceType::GRASS_TREE || r->getType() == ResourceType::SWAMP_TREE)
            {
                me->setState(PlayerState::CUTTING);
                if(r->getCenterPixal().y() < me->pos().y())
                {
                    me->setCutLeft(true);
                    me->setCutRight(false);
                }
                else
                {
                    me->setCutLeft(false);
                    me->setCutRight(true);
                }
            }

            else
            {
                me->setState(PlayerState::DIGGING);
                if(r->getCenterPixal().y() < me->pos().y())
                {
                    me->setDigLeft(true);
                    me->setDigRight(false);
                }
                else
                {
                    me->setDigLeft(false);
                    me->setDigRight(true);
                }
            }
        }
    }

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
        return;
    }

    if (!me) return;

    if (ev->key() == Qt::Key_W){
        me->setWalkUp(false);
    }
    if (ev->key() == Qt::Key_S){
        me->setWalkDown(false);
    }
    if (ev->key() == Qt::Key_A){
        me->setWalkLeft(false);
    }
    if (ev->key() == Qt::Key_D){
        me->setWalkRight(false);
    }
    QWidget::keyReleaseEvent(ev);
}
