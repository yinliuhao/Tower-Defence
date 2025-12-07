#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include "utils.h"


class Player : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    Player();

    void setMoveUp(bool on);
    void setMoveDown(bool on);
    void setMoveLeft(bool on);
    void setMoveRight(bool on);
    void updatePosition(float speed, float delta);
    bool isRolling() { return rollingCount; }

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
    bool isGoingLeft = false;

    QTimer moveTimer;
    QTimer rollTimer;
    bool m_up = false, m_down = false, m_left = false, m_right = false;
    QTimer walkTimer;   //控制走路图片切换
    int nowIndex = 0;
    qreal me_speed = PLAYERSPEED;   //单位：像素/秒
    int rollingCount = 0;

signals:
    void rolling();
};

#endif // PLAYER_H
