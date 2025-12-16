#include <QGraphicsView>
#include <QString>
#include <QKeyEvent>
#include "gameWidght.h"
#include "ui_gameWidght.h"
#include "towerButton.h"
#include "global.h"
#include "utils.h"
#include <QDebug>
#include "tower.h"
#include "bulletmanager.h"

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

    //开启鼠标追踪
    setMouseTracking(true);
    view->setMouseTracking(true);
    view->viewport()->setMouseTracking(true);

    //安装事件过滤器
    view->installEventFilter(this);
    view->viewport()->installEventFilter(this);

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
        connect(monster, &Monster::died, this, [this, monster](int goldReward) {
            qDebug() << "怪物死亡，奖励金币：" << goldReward;
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

    for(int i = 0; i < resourceManager->getResourcesNum(); i++)
    {
        Resource * res = resourceManager->getResources(i);
        connect(res, &Resource::resourceDepleted, myUI, [this, res](){
            myUI->collectResource(res->getType());
        });
    }


    buildManager = new BuildManager;
    for(int i = 0; i < towerNum; i++)
    {
        connect(tower[i], &QPushButton::clicked, this, [i](){
            switch(i)
            {
            case 0:
                buildManager->startBuild(TowerType::TOWER1);
                break;
            case 1:
                buildManager->startBuild(TowerType::TOWER2);
                break;
            case 2:
                buildManager->startBuild(TowerType::TOWER3);
                break;
            }
        });
    }

    setMouseTracking(true);
    view->setMouseTracking(true);

    connect(buildManager, &BuildManager::previewCreated,
            this, [this](PreviewTower* preview){
                scene->addItem(preview);

                QPoint viewPos = view->mapFromGlobal(QCursor::pos());
                QPointF worldPos = view->mapToScene(viewPos);
                QPoint grid = gMap->pixelToGrid(worldPos);

                bool canPlace = gMap->canPlaceTower(grid);
                preview->setGridPos(grid);
                preview->setValid(canPlace);
            });

    connect(buildManager, &BuildManager::previewRemoved,
            this, [this](PreviewTower* preview){
                scene->removeItem(preview);
            });
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
        Resource * r = me->getTarget();
        if(r != nullptr)
        {
            if(r->getType() == ResourceType::GRASS_TREE || r->getType() == ResourceType::SWAMP_TREE)
            {
                me->setState(PlayerState::CUTTING);
                if(r->getCenterPixal().x() < me->pos().x())
                {
                    me->setCutLeft(true);
                    me->setCutRight(false);
                }
                else
                {
                    me->setCutLeft(false);
                    me->setCutRight(true);
                }
                return;
            }

            else
            {
                me->setState(PlayerState::DIGGING);
                if(r->getCenterPixal().x() < me->pos().x())
                {
                    me->setDigLeft(true);
                    me->setDigRight(false);
                }
                else
                {
                    me->setDigLeft(false);
                    me->setDigRight(true);
                }
                return;
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

bool Game::eventFilter(QObject *watched, QEvent *event)
{
    // ---------------------------------------------------------
    // 1. 处理鼠标移动 (预览塔跟随)
    // ---------------------------------------------------------
    if (event->type() == QEvent::MouseMove)
    {
        // 如果不在建造模式，不拦截，交给 View 默认处理
        if (!buildManager->isBuilding())
            return false;

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        // 【关键】因为监听的是 View，所以 pos 直接就是 View 坐标
        QPoint viewPos = mouseEvent->pos();
        QPointF worldPos = view->mapToScene(viewPos);
        QPoint grid = gMap->pixelToGrid(worldPos);

        bool canPlace = gMap->canPlaceTower(grid);
        PreviewTower* preview = buildManager->getPreviewTower();
        if (preview)
        {
            preview->setGridPos(grid);
            preview->setValid(canPlace);
        }
        // 这里返回 false，让 view 也能收到移动信号（不影响功能，比较保险）
        return false;
    }

    // ---------------------------------------------------------
    // 2. 处理鼠标按下 (放置塔 / 取消)
    // ---------------------------------------------------------
    if (event->type() == QEvent::MouseButtonPress)
    {
        // 如果不在建造模式，就不管，让 View 自己处理（比如点击选中其他塔）
        if (!buildManager->isBuilding())
            return false;

        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        // 同样：坐标直接就是 View 坐标
        QPoint viewPos = mouseEvent->pos();
        QPointF worldPos = view->mapToScene(viewPos);
        QPoint grid = gMap->pixelToGrid(worldPos);

        // --- 左键：放置 ---
        if (mouseEvent->button() == Qt::LeftButton)
        {
            if (gMap->placeTower(worldPos))
            {
                // 创建真塔
                Tower* tower = new Tower(buildManager->getPreviewTower()->getTowerType());

                // 这里需要你的 Tower 类有对应的添加到场景的逻辑
                tower->put(grid.x(), grid.y());
                scene->addItem(tower);
                // 放置成功后，取消建造模式
                connect(tower->getManager(), &BulletManager::bulletCreated, this, [this](Bullet* bullet){
                    scene->addItem(bullet);
                });
                buildManager->cancelBuild();
            }
            // 返回 true，表示“这事我处理完了”，View 就不要再瞎掺和了
            return true;
        }

        // --- 右键：取消 ---
        else if (mouseEvent->button() == Qt::RightButton)
        {
            buildManager->cancelBuild();
            return true; // 拦截事件，防止弹出默认菜单等
        }
    }

    // 其他事件交给父类默认处理
    return QWidget::eventFilter(watched, event);
}
