#include <QGraphicsView>
#include <QString>
#include <QKeyEvent>
#include "_UI/gameWidght.h"
#include "ui_gameWidght.h"
#include "_tower/towerButton.h"
#include "_others/global.h"
#include "_others/utils.h"
#include <QDebug>
#include "_tower/tower.h"
#include "_bullet/bulletmanager.h"
#include "utility"
#include "QRandomGenerator"

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
    tower.push_back(new TowerButton("archer", TowerType::TOWER1, this));
    towerNum++;
    tower.push_back(new TowerButton("cannon", TowerType::TOWER2, this));
    towerNum++;
    tower.push_back(new TowerButton("mortar", TowerType::TOWER3, this));
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
        connect(monster, &Monster::attackedCamp, this, [this](int damage) {
            playerBase->takeDamage(damage);
            myUI->sub(damage, CAMPHEALTH);
        });

        connect(monster, &Monster::attackedPlayer, this, [this](int damage) {
            myUI->sub(damage, PLAYERHEALTH);
        });

        // 连接怪物死亡信号
        connect(monster, &Monster::died, this, [this](MonsterType type) {
            QRandomGenerator* random = QRandomGenerator::global();
            int rewardType = random->bounded(100);
            switch(type)
            {
            case MonsterType::MONSTER1:
                if (rewardType >= 0 && rewardType < 2) {
                    myUI->add(1, BRONZE);
                }
                else if (rewardType >= 2 && rewardType < 4) {
                    myUI->add(1, SILVER);
                }
                else if (rewardType >= 4 && rewardType < 6) {
                    myUI->add(1, GOLD);
                }
                break;
            case MonsterType::MONSTER2:
                if (rewardType >= 0 && rewardType < 3) {
                    myUI->add(1, BRONZE);
                }
                else if (rewardType >= 3 && rewardType < 6) {
                    myUI->add(1, SILVER);
                }
                else if (rewardType >= 6 && rewardType < 9) {
                    myUI->add(1, GOLD);
                }
                break;
            case MonsterType::MONSTER3:
                if (rewardType >= 0 && rewardType < 5) {
                    myUI->add(1, BRONZE);
                }
                else if (rewardType >= 5 && rewardType < 10) {
                    myUI->add(1, SILVER);
                }
                else if (rewardType >= 10 && rewardType < 15) {
                    myUI->add(1, GOLD);
                }
                break;
            case MonsterType::MONSTER4:
                if (rewardType >= 0 && rewardType < 8) {
                    myUI->add(1, BRONZE);
                }
                else if (rewardType >= 8 && rewardType < 16) {
                    myUI->add(1, SILVER);
                }
                else if (rewardType >= 16 && rewardType < 24) {
                    myUI->add(1, GOLD);
                }
                break;
            default:
                if (rewardType >= 0 && rewardType < 10) {
                    myUI->add(1, BRONZE);
                }
                else if (rewardType >= 10 && rewardType < 20) {
                    myUI->add(1, SILVER);
                }
                else if (rewardType >= 20 && rewardType < 30) {
                    myUI->add(1, GOLD);
                }
                break;
            }
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
            updateBtn();
        });
    }

    updateBtn();


    buildManager = new BuildManager;
    for(int i = 0; i < towerNum; i++)
    {
        connect(tower[i], &QPushButton::clicked, this, [i, this](){
            switch(i)
            {
            case 0:
                if(tower[0]->enoughOrNot())
                    buildManager->startBuild(TowerType::TOWER1);
                break;
            case 1:
                if(tower[1]->enoughOrNot())
                    buildManager->startBuild(TowerType::TOWER2);
                break;
            case 2:
                if(tower[2]->enoughOrNot())
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

    explorationmarker = new ExplorationMarker(scene);
    connect(me, &Player::moved, explorationmarker, &ExplorationMarker::refreshMarkers);

    playerBase = new PlayerBase();
    scene->addItem(playerBase);
    QPoint gridPos = gMap->playerHome[0];
    playerBase->setPos(gridPos.y() * TILESIZE, gridPos.x() * TILESIZE);
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
                if(r->getCenterPixal().x() < me->getCenterPos().x())
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
                if(r->getCenterPixal().x() < me->getCenterPos().x())
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
            explorationmarker->setMarked(true);
        }
        else
        {
            for(int i = 0; i < towerNum; i++)
            {
                tower[i]->hide();
            }
            explorationmarker->setMarked(false);
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

// ---------------------------------------------------------
// 处理鼠标移动事件（预览塔跟随鼠标）
// ---------------------------------------------------------
bool Game::handleMouseMove(QMouseEvent* event)
{
    if(!buildManager->isBuilding())
        return false; // 非建造模式交给 View 默认处理

    QPoint viewPos = event->pos();
    QPointF worldPos = view->mapToScene(viewPos);
    QPoint grid = gMap->pixelToGrid(worldPos);

    bool canPlace = gMap->canPlaceTower(grid);
    bool canDetect = me->isDetectable(grid.x(), grid.y());

    PreviewTower* preview = buildManager->getPreviewTower();
    if(preview)
    {
        preview->setGridPos(grid);
        preview->setValid(canPlace && canDetect);
    }

    return false; // 允许 View 继续处理
}

// ---------------------------------------------------------
// 处理单击事件（选中塔 / 放置塔 / 取消）
// ---------------------------------------------------------
bool Game::handleMousePress(QMouseEvent* event)
{
    QPoint viewPos = event->pos();
    QPointF worldPos = view->mapToScene(viewPos);
    QPoint grid = gMap->pixelToGrid(worldPos);

    if(!buildManager->isBuilding())
    {
        // 非建造模式：处理塔选择和升级
        Tower* tower = buildManager->getTower(grid.x(), grid.y());

        Tower* selectedTower = buildManager->getSelectedTower();

        if(event->button() == Qt::LeftButton)
        {
            if(!tower) return true;
            // 第一次点击——选择塔
            if(!selectedTower)
            {
                selectTower(tower);
                return true;
            }

            // 第二次点击——尝试升级
            if(selectedTower != tower)
            {
                selectTower(tower);
                return true;
            }

            handleTowerUpgrade(tower);
            clearTowerSelection();
            return true;
        }
        else if(event->button() == Qt::RightButton)
        {
            // 右键取消选择
            clearTowerSelection();
            return true;
        }
    }
    else
    {
        // 建造模式：处理塔放置
        if(event->button() == Qt::LeftButton)
        {
            if(gMap->canPlaceTower(grid) && me->isDetectable(grid.x(), grid.y()))
            {
                // 扣除资源
                auto sourceNeeded = Tower::getBuildCost(buildManager->getBuildingType());
                for(auto& s : sourceNeeded)
                    myUI->sub(s.second, s.first);

                // 创建塔
                Tower* tower = new Tower(buildManager->getPreviewTower()->getTowerType());
                buildManager->finalBuild(grid.x(), grid.y(), tower);
                tower->put(grid.x(), grid.y());
                scene->addItem(tower);

                connect(tower->getManager(), &BulletManager::bulletCreated, this, [this](Bullet* bullet){
                    scene->addItem(bullet);
                });

                gMap->placeTower(QPointF(grid.x(), grid.y()));
                buildManager->cancelBuild();
            }
            return true;
        }
        else if(event->button() == Qt::RightButton)
        {
            // 右键取消建造
            buildManager->cancelBuild();
            return true;
        }
    }

    return false;
}

// ---------------------------------------------------------
// 处理双击事件（直接升级塔）
// ---------------------------------------------------------
bool Game::handleMouseDoubleClick(QMouseEvent* event)
{
    if(event->button() != Qt::LeftButton)
        return false;

    if(buildManager->isBuilding()) return true; // 建造模式不处理

    QPoint viewPos = event->pos();
    QPointF worldPos = view->mapToScene(viewPos);
    QPoint grid = gMap->pixelToGrid(worldPos);

    Tower* tower = buildManager->getTower(grid.x(), grid.y());
    if(!tower) return true;

    handleTowerUpgrade(tower);
    clearTowerSelection();

    //qDebug() << "双击升级成功";
    return true;
}

// ---------------------------------------------------------
// 升级塔的统一逻辑
// ---------------------------------------------------------
void Game::handleTowerUpgrade(Tower* tower)
{
    const auto& sourceNeeded = tower->getUpgradeCost();

    for(auto& s : sourceNeeded)
    {
        if(myUI->getNum(s.first) < s.second)
            return; // 资源不足，无法升级
    }

    for(auto& s : sourceNeeded)
        myUI->sub(s.second, s.first);

    tower->upgrade();
}

bool Game::eventFilter(QObject *watched, QEvent *event)
{
    switch(event->type())
    {
    case QEvent::MouseMove:
        return handleMouseMove(static_cast<QMouseEvent*>(event));

    case QEvent::MouseButtonPress:
        return handleMousePress(static_cast<QMouseEvent*>(event));

    case QEvent::MouseButtonDblClick:
        return handleMouseDoubleClick(static_cast<QMouseEvent*>(event));

    default:
        return QWidget::eventFilter(watched, event);
    }
}

void Game::updateBtn()
{
    for(int i = 0; i < towerNum; i++)
    {
        std::vector<std::pair<UiResource, int>> sourceNeeded = Tower::getBuildCost(TowerType(i));
        bool isEnough = true;
        for(int i = 0; i < (int)sourceNeeded.size(); i++)
        {
            if(myUI->getNum(sourceNeeded[i].first) < sourceNeeded[i].second)
            {
                isEnough = false;
                break;
            }
        }
        tower[i]->setEnough(isEnough);
    }
}

void Game::selectTower(Tower* tower)
{
    if (!tower) return;

    // 清理之前的显示
    clearTowerSelection();

    buildManager->setSelectedTower(tower->getGridX(), tower->getGridY());

    // 绘制攻击范围
    if(tower->getTowerType() != TowerType::TOWER3)
    {
        QGraphicsEllipseItem* rangeCircle = new QGraphicsEllipseItem();
        rangeCircle->setRect(
            tower->x() + TILESIZE / 2.0 - tower->getAttackRange(),
            tower->y() + TILESIZE / 2.0 - tower->getAttackRange(),
            tower->getAttackRange() * 2,
            tower->getAttackRange() * 2
            );
        rangeCircle->setBrush(QBrush(QColor(0, 255, 0, 50))); // 半透明绿色
        rangeCircle->setPen(Qt::NoPen);
        scene->addItem(rangeCircle);
        buildManager->setRangeCircle(rangeCircle);  // Tower 保存这个指针方便取消
    }
    else
    {
        qreal outerR = tower->getAttackRange();
        qreal innerR = outerR / OUTER_DIVIDE_INNER;   // 外内环比 = 3

        QPainterPath path;
        path.setFillRule(Qt::OddEvenFill);

        // 外圆
        path.addEllipse(
            QPointF(
                tower->x() + TILESIZE / 2.0,
                tower->y() + TILESIZE / 2.0
                ),
            outerR,
            outerR
            );

        // 内圆（挖空）
        path.addEllipse(
            QPointF(
                tower->x() + TILESIZE / 2.0,
                tower->y() + TILESIZE / 2.0
                ),
            innerR,
            innerR
            );

        QGraphicsPathItem* rangeRing = new QGraphicsPathItem();
        rangeRing->setPath(path);
        rangeRing->setBrush(QColor(0, 255, 0, 50)); // 半透明绿色
        rangeRing->setPen(Qt::NoPen);
        rangeRing->setZValue(-1); // 在炮塔下面

        scene->addItem(rangeRing);
        buildManager->setRangeCircle(rangeRing);

    }

    // 添加升级提示文字
    QGraphicsTextItem* upgradeText = new QGraphicsTextItem("Upgrade?");
    upgradeText->setDefaultTextColor(Qt::yellow);
    upgradeText->setFont(QFont("Arial", 20, QFont::Bold));
    upgradeText->setPos(tower->x() - 20, tower->y() - 40); // 炮塔上方
    scene->addItem(upgradeText);
    buildManager->setUpgradeText(upgradeText);
}

void Game::clearTowerSelection()
{
    Tower* tower = buildManager->getSelectedTower();
    if (!tower) return;

    // 移除显示的攻击范围和文字
    if (buildManager->getRangeCircle()) {
        scene->removeItem(buildManager->getRangeCircle());
        delete buildManager->getRangeCircle();
        buildManager->setRangeCircle(nullptr);
    }

    if (buildManager->getUpgradeText()) {
        scene->removeItem(buildManager->getUpgradeText());
        delete buildManager->getUpgradeText();
        buildManager->setUpgradeText(nullptr);
    }

    buildManager->clearSelectedTower();
}
