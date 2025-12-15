#ifndef GAMEWIDGHT_H
#define GAMEWIDGHT_H

#include <QWidget>
#include "towerButton.h"
#include "playerUI.h"
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
    void keyPressEvent(QKeyEvent *ev) override;
    void keyReleaseEvent(QKeyEvent *ev) override;

private slots:
   // void handleCampAttacked(float damage);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Ui::Game *ui;
    QGraphicsView* view;
    QGraphicsScene* scene;

    int width;
    int height;

    int towerNum;  // 炮塔种类数
    std::vector<TowerButton*> tower;   // 炮塔按钮
    PlayerUI *myUI;   // UI界面

    QTimer * viewTimer;
    bool uiVisible;
    bool buttonVisible;
};

#endif // GAMEWIDGHT_H
