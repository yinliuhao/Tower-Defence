#include "player.h"
#include<qstring.h>
#include"QPainter"
#include"utils.h"

Player::Player()
{
    initpic();
    setZValue(PLAYERZVALUE);
    setPos(MAPWIDTH / 2.0, MAPHEIGHT / 2.0);
    m_timer.setInterval(DELTATIME);
    connect(&m_timer, &QTimer::timeout, this, &Player::updatePosition);
    m_timer.start();
}

void Player::setMoveUp(bool on)    { m_up = on; }
void Player::setMoveDown(bool on)  { m_down = on; }
void Player::setMoveLeft(bool on)  { m_left = on; }
void Player::setMoveRight(bool on) { m_right = on; }

void Player::updatePosition()
{
    QPointF vel(0, 0);
    if (m_up)    vel.ry() -= 1;
    if (m_down)  vel.ry() += 1;
    if (m_left)  vel.rx() -= 1;
    if (m_right) vel.rx() += 1;

    // 归一化，防止对角线更快
    if (!qFuzzyIsNull(vel.x()) || !qFuzzyIsNull(vel.y())) {
        qreal len = qSqrt(vel.x()*vel.x() + vel.y()*vel.y());
        vel /= len;
    }

    // 移动（每帧速度 = speed * dt）
    qreal dt = DELTATIME / 1000.0;
    setPos(pos() + vel * (m_speed * dt));
}


QRectF Player::boundingRect() const
{
    return QRectF(0, 0, PLAYERWIDTH, PLAYERHEIGHT);
}

void Player::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*)
{
    p->drawPixmap(0, 0, walking);
}

void Player::initpic()
{
    walking.load(QString(":/picture/player/walking/1.png"));
}
