#ifndef PLAYER_H
#define PLAYER_H

#include<QGraphicsPixmapItem>
#include<qpixmap.h>
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

protected:
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) override;
    void initpic();

private slots:
    void updatePosition();

private:
    int width;
    int height;
    QPixmap walking;
    bool m_up = false, m_down = false, m_left = false, m_right = false;
    qreal m_speed = PLAYERSPEED;   //单位：像素/秒
    QTimer m_timer;
};

#endif // PLAYER_H
