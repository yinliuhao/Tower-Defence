#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include "utils.h"
#include "resource.h"

enum class PlayerState
{
    WALKING,
    ROLLING,
    CUTTING,
    DIGGING
};

class Player : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Player();

    void setWalkUp(bool on) { walk_up = on; }
    void setWalkDown(bool on) { walk_down = on; }
    void setWalkLeft(bool on) { walk_left = on; }
    void setWalkRight(bool on) { walk_right = on; }

    bool getWalkUp() { return walk_up; }
    bool getWalkDown() { return walk_down; }
    bool getWalkLeft() { return walk_left; }
    bool getWalkRight() { return walk_right; }

    PlayerState getState() { return state; }

    void setRollUp(bool on) { roll_up = on; }
    void setRollDown(bool on) { roll_down = on; }
    void setRollLeft(bool on) { roll_left = on; }
    void setRollRight(bool on) {roll_right = on; }

    void setCutLeft(bool on) { cut_left = on; }
    void setCutRight(bool on) { cut_right = on; }

    void setDigLeft(bool on) { dig_left = on; }
    void setDigRight(bool on) { dig_right = on; }

    void setState(PlayerState sta) { state = sta; }

    void updatePosition();
    void updatePicture();

    bool isWalking() { return state == PlayerState::WALKING; }
    bool isRolling() { return state == PlayerState::ROLLING; }
    bool isCutting() { return state == PlayerState::CUTTING; }
    bool isDigging() { return state == PlayerState::DIGGING; }

    Resource* findNearbyResource() const;

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) override;
    void initpic();

private:
    int width;
    int height;

    QPixmap leftWalking[4];
    QPixmap rightWalking[4];

    QPixmap leftRolling[4];
    QPixmap rightRolling[4];

    QPixmap leftCutting[4];
    QPixmap rightCutting[4];

    QPixmap leftDigging[4];
    QPixmap rightDigging[4];

    bool isWalkingLeft = false;
    bool isRollingLeft = false;
    bool isCuttingLeft = false;
    bool isDiggingLeft = false;

    QTimer moveTimer;
    QTimer picTimer;   //控制图片切换

    bool walk_up = false;
    bool walk_down = false;
    bool walk_left = false;
    bool walk_right = false;

    bool roll_up = false;
    bool roll_down = false;
    bool roll_left = false;
    bool roll_right = false;

    bool cut_left = false;
    bool cut_right = false;

    bool dig_left = false;
    bool dig_right = false;


    int walkIndex = 0;
    int rollIndex = -1;
    int cutIndex = -1;
    int digIndex = -1;

    int walkFrameCounter = 0;
    int rollFrameCounter = 0;
    int cutFrameCounter = 0;
    int digFrameCounter = 0;

    qreal walk_speed = WALKSPEED;   //单位：像素/秒
    qreal roll_speed = ROLLSPEED;

    PlayerState state = PlayerState::WALKING;

};

#endif // PLAYER_H
