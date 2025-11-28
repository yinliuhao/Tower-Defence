#include <QGraphicsView>
#include <qstring.h>
#include <QKeyEvent>
#include "gameWidght.h"
#include "ui_gameWidght.h"
#include "towerButton.h"
#include "utils.h"

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
    scene->setSceneRect(0, 0, width, height);  // 根据你的地图大小

    // 2. 创建视图
    view = new QGraphicsView(scene, this);
    view->setRenderHint(QPainter::Antialiasing);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setFixedSize(width, height);  // 配合地图大小

    // 3. 取消滚动区域边距（重要）
    view->setFrameStyle(QFrame::NoFrame);

    //初始化地图
    Map *m = new Map();
    map.push_back(m);
    mapNum++;
    scene->addItem(map[0]);

    //初始化炮塔按钮
    towerNum = 0;
    tower.push_back(new TowerButton("archer", this));
    towerNum++;
    tower.push_back(new TowerButton("cannon", this));
    towerNum++;
    tower.push_back(new TowerButton("mortar", this));
    towerNum++;

    //显示炮塔按钮
    for(int i = 0; i < towerNum; i++)
    {
        tower[i]->move(this->map[0]->getWidth(), this->map[0]->getHeight() - (towerNum - i) * BTNHEIGHT);
        tower[i]->raise();
        tower[i]->show();
    }

    //初始化UI界面
    myUI = new PlayerUI(this);

    //显示UI界面
    myUI->move(this->map[0]->getWidth() - 5 * this->map[0]->gettileSize(), 0);
    myUI->raise();
    myUI->show();

    //初始化玩家
    me = new Player();

    //显示玩家
    scene->addItem(me);
}


Game::~Game()
{
    delete ui;
    for(int i = 0; i < mapNum; i++)
    {
        delete map[i];
        map[i] = nullptr;
    }
}

void Game::keyPressEvent(QKeyEvent *ev)
{
    if (!me) return;

    if (ev->key() == Qt::Key_W) me->setMoveUp(true);
    if (ev->key() == Qt::Key_S) me->setMoveDown(true);
    if (ev->key() == Qt::Key_A) me->setMoveLeft(true);
    if (ev->key() == Qt::Key_D) me->setMoveRight(true);

    QWidget::keyPressEvent(ev);
}

void Game::keyReleaseEvent(QKeyEvent *ev)
{
    if (!me) return;
    if (ev->isAutoRepeat())  return;

    if (ev->key() == Qt::Key_W) me->setMoveUp(false);
    if (ev->key() == Qt::Key_S) me->setMoveDown(false);
    if (ev->key() == Qt::Key_A) me->setMoveLeft(false);
    if (ev->key() == Qt::Key_D) me->setMoveRight(false);

    QWidget::keyReleaseEvent(ev);
}
