#include "player.h"
#include<qstring.h>
#include"QPainter"
#include"utils.h"

Player::Player()
{
    initpic();
    setZValue(PLAYERZVALUE);
    setPos(WIDTH / 2.0, HEIGHT / 2.0);

    //处理人物行走图片帧变化
    walkTimer.setInterval(WAKEDELTA);
    connect(&walkTimer, &QTimer::timeout, this, [this](){
        if(m_up || m_down || m_left || m_right)
        {
            if(m_left)
            {
                isGoingLeft = true;
            }
            if(m_right)
            {
                isGoingLeft = false;
            }
            nowIndex = (nowIndex + 1) % 4;
            update();
        }
        else
        {
            nowIndex = 0;
            update();
        }
    });
    walkTimer.start();

    //处理玩家移动速度
    moveTimer.setInterval(SPEEDDELTA);
    moveTimer.start();
    connect(&moveTimer, &QTimer::timeout, this, [&](){
            updatePosition(me_speed);
    });

    //处理玩家翻滚速度及图片帧变化
    rollTimer.setInterval(ROLLDELTA);
    connect(this, &Player::rolling, this, [&](){
        if(!m_up && !m_down && !m_left && !m_right) return;
        walkTimer.stop();
        moveTimer.stop();
        rollTimer.start();
    });

    connect(&rollTimer, &QTimer::timeout, this, [&](){
        ++rollingCount;
        update();
        updatePosition(ROLLSPEED);
        if(rollingCount >= 4 * ROLLTIMES)
        {
            rollingCount = 0;
            walkTimer.start();
            moveTimer.start();
            rollTimer.stop();
        }
    });
}

//改变人物实时前进方向
void Player::setMoveUp(bool on)    { m_up = on; }
void Player::setMoveDown(bool on)  { m_down = on; }
void Player::setMoveLeft(bool on)  { m_left = on; }
void Player::setMoveRight(bool on) { m_right = on; }

//更新位置函数
void Player::updatePosition(float speed)
{
    QPointF vel(0, 0);
    if (m_up)    vel.ry() -= 1;
    if (m_down)  vel.ry() += 1;
    if (m_left)  vel.rx() -= 1;
    if (m_right) vel.rx() += 1;

    // 归一化，防止对角线更快
    if (!qFuzzyIsNull(vel.x()) || !qFuzzyIsNull(vel.y()))
    {
        qreal len = qSqrt(vel.x()*vel.x() + vel.y()*vel.y());
        vel /= len;
    }

    // 移动（每帧速度 = speed * dt）
    qreal dt = SPEEDDELTA / 1000.0;
    setPos(pos() + vel * (speed * dt));
}

QRectF Player::boundingRect() const
{
    return QRectF(0, 0, PLAYERWIDTH, PLAYERHEIGHT);
}

void Player::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*)
{
    if(isGoingLeft)
    {
        if(rollingCount != 0)
        {
            p->drawPixmap(0, 0, leftRolling[rollingCount % 4]);
        }
        else
        {
            p->drawPixmap(0, 0, leftWalking[nowIndex]);
        }
    }
    else
    {
        if(rollingCount != 0)
        {
            p->drawPixmap(0, 0, rightRolling[(rollingCount - 1) % 4]);
        }
        else
        {
            p->drawPixmap(0, 0, rightWalking[nowIndex]);
        }
    }
}

void Player::initpic()
{
    for(int i = 0; i < 4; i++)
    {
        leftWalking[i].load(QString(":/picture/player/walking/%1.png").arg(i + 1));
        rightWalking[i] = leftWalking[i].transformed(QTransform().scale(-1, 1));
    }
    for(int i = 0; i < 4; i++)
    {
        leftRolling[i].load(QString(":/picture/player/rolling/%1.png").arg(i + 1));
        rightRolling[i] = leftRolling[i].transformed(QTransform().scale(-1, 1));
    }
}
