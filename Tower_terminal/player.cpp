#include "player.h"
#include <QString>
#include <QPainter>
#include <QTransform>
#include "utils.h"
#include "map.h"  // 关键：包含Map类的完整定义

Player::Player(Map* map) : currentMap(map)
{
    initpic();
    setZValue(PLAYERZVALUE);

    // 设置初始位置
    if (currentMap) {
        // 如果有地图，放在地图中心（之后改为帐篷的位置）
        setPos(currentMap->getWidth() / 2.0, currentMap->getHeight() / 2.0);
    } else {
        // 否则放在窗口中心
        setPos(WIDTH / 2.0, HEIGHT / 2.0);
    }

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

    // 归一化
    if (!qFuzzyIsNull(vel.x()) || !qFuzzyIsNull(vel.y()))
    {
        qreal len = qSqrt(vel.x()*vel.x() + vel.y()*vel.y());
        vel /= len;
    }

    // 移动
    qreal dt = SPEEDDELTA / 1000.0;
    QPointF newPos = pos() + vel * (speed * dt);

    // 如果有地图，检查边界和地形
    if (currentMap) {
        // 获取当前格子和目标格子
        QPoint currentGrid = currentMap->pixelToGrid(pos());
        QPoint targetGrid = currentMap->pixelToGrid(newPos);

        // 如果目标格子与当前格子不同，检查是否可以移动
        if (currentGrid != targetGrid) {
            if (!currentMap->canMoveTo(currentGrid.x(), currentGrid.y(),
                                       targetGrid.x(), targetGrid.y())) {
                // 不能移动到目标格子，保持原位
                return;
            }
        }

        // 获取玩家边界
        QRectF playerRect = boundingRect();

        // 计算新位置下的玩家边界
        QRectF newPlayerRect = QRectF(newPos, playerRect.size());

        // 检查是否在地图边界内
        if (newPlayerRect.left() < 0) newPos.setX(0);
        if (newPlayerRect.top() < 0) newPos.setY(0);
        if (newPlayerRect.right() > currentMap->getWidth())
            newPos.setX(currentMap->getWidth() - playerRect.width());
        if (newPlayerRect.bottom() > currentMap->getHeight())
            newPos.setY(currentMap->getHeight() - playerRect.height());
    }

    setPos(newPos);
}

void Player::setMap(Map* map) {
    currentMap = map;
    if (map) {
        // 更新位置到地图中心
        setPos(map->getWidth() / 2.0, map->getHeight() / 2.0);
    }
}

QRectF Player::boundingRect() const
{
    return QRectF(0, 0, PLAYERWIDTH, PLAYERHEIGHT);
}

//绘制玩家
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
