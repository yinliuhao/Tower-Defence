#ifndef GAMEWIDGHT_H
#define GAMEWIDGHT_H

#include <QWidget>
#include "map.h"
#include "towerButton.h"
#include "player.h"
#include "playerUI.h"
#include "monster.h"
#include <vector>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QTimer>
#include <QTime>

namespace Ui {
class Game;
}

class Game : public QWidget
{
    Q_OBJECT

public:
    explicit Game(QWidget *parent = nullptr);
    ~Game();
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

private slots:
    void spawnMonster();  // 生成怪物
    void updateGame();    // 更新游戏状态

private:
    Ui::Game *ui;
    QGraphicsView* view;
    QGraphicsScene* scene;

    int width;
    int height;

    int mapNum;
    std::vector<Map*> map;  // 地图

    // 怪物相关
    std::vector<Monster*> monsters;  // 怪物容器
    QTimer* monsterSpawnTimer;       // 怪物生成计时器
    int currentWave;                 // 当前波次
    int monstersInWave;              // 当前波次的怪物数量
    int monstersSpawned;             // 已生成的怪物数量

    int towerNum;  // 炮塔种类数
    std::vector<TowerButton*> tower;   // 炮塔按钮
    PlayerUI *myUI;   // UI界面
    Player * me;
    QTimer * viewTimer;
    bool uiVisible;
    bool buttonVisible;

    QTimer* gameUpdateTimer;  // 游戏状态更新计时器
};

#endif // GAMEWIDGHT_H
