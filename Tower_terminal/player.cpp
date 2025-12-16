#include "player.h"
#include <QString>
#include <QPainter>
#include <QTransform>
#include "utils.h"
#include "global.h"
#include "resource.h"
#include "resourcemanager.h"

Player::Player()
{
    initpic();
    setZValue(PLAYERZVALUE);

    setPos(WIDTH / 2.0, HEIGHT / 2.0);

    //处理人物行走图片帧变化
    picTimer.setInterval(PICTURE_DELTA);
    connect(&picTimer, &QTimer::timeout, this, [&](){
        updatePicture();
    });
    picTimer.start();

    //处理玩家移动速度
    moveTimer.setInterval(POSITION_DELTA);
    connect(&moveTimer, &QTimer::timeout, this, [&](){
        updatePosition();
    });
    moveTimer.start();

    findTimer.setInterval(FIND_TARGET_INTERVAL);
    connect(&findTimer, &QTimer::timeout, this, [&](){
        findNearbyResource();
    });
    findTimer.start();
}

//更新图像函数
void Player::updatePicture()
{
    if(state == PlayerState::WALKING)
    {
        if(walk_left)  isWalkingLeft = true;
        if(walk_right)  isWalkingLeft = false;
        if(!walk_up && !walk_down && !walk_left && !walk_right)
        {
            walkIndex = 0;
            update();
            return;
        }
        walkFrameCounter += PICTURE_DELTA;
        if(walkFrameCounter >= WALK_FRAME_INTERVAL)
        {
            walkFrameCounter = 0;
            walkIndex = (walkIndex + 1) % 4;
            update();
        }
    }
    else if(state == PlayerState::ROLLING)
    {
        if(roll_left)  isRollingLeft = true;
        if(roll_right)  isRollingLeft = false;
        if(!roll_up && !roll_down && !roll_left && !roll_right)
        {
            rollIndex = 0;
            state = PlayerState::WALKING;
            update();
            return;
        }
        rollFrameCounter += PICTURE_DELTA;
        if(rollFrameCounter >= ROLL_FRAME_INTERVAL)
        {
            rollFrameCounter = 0;
            ++rollIndex;
            update();
            if(rollIndex >= 4 * ROLLTIMES)
            {
                rollIndex = -1;
                state = PlayerState::WALKING;
            }
        }
    }
    else if(target != nullptr && state == PlayerState::CUTTING)
    {
        if(cut_left)  isCuttingLeft = true;
        if(cut_right)  isCuttingLeft = false;
        if(!cut_left && !cut_right)
        {
            cutIndex = 0;
            state = PlayerState::WALKING;
            update();
            return;
        }
        cutFrameCounter += PICTURE_DELTA;
        if(cutFrameCounter >= CUT_FRAME_INTERVAL)
        {
            cutFrameCounter = 0;
            cutIndex = (cutIndex + 1) % 4;
            update();
            if(cutIndex >= 3)
            {
                cutIndex = -1;
                target->takeDamage();
                state = PlayerState::WALKING;
            }
        }
    }
    else if(target != nullptr && state == PlayerState::DIGGING)
    {
        if(dig_left)  isDiggingLeft = true;
        if(dig_right)  isDiggingLeft = false;
        if(!dig_left && !dig_right)
        {
            digIndex = 0;
            state = PlayerState::WALKING;
            update();
            return;
        }
        digFrameCounter += PICTURE_DELTA;
        if(digFrameCounter >= DIG_FRAME_INTERVAL)
        {
            digFrameCounter = 0;
            digIndex = (digIndex + 1) % 4;
            update();
            if(digIndex >= 3)
            {
                digIndex = -1;
                target->takeDamage();
                state = PlayerState::WALKING;
            }
        }
    }
}


//更新位置函数
void Player::updatePosition()
{
    qreal delta = POSITION_DELTA;
    qreal speed = 0.0;
    bool m_up = false;
    bool m_down = false;
    bool m_left = false;
    bool m_right = false;
    switch(state)
    {
    case PlayerState::WALKING:
        speed = WALKSPEED;
        m_up = walk_up;
        m_down = walk_down;
        m_left = walk_left;
        m_right = walk_right;
        break;
    case PlayerState::ROLLING:
        speed = ROLLSPEED;
        m_up = roll_up;
        m_down = roll_down;
        m_left = roll_left;
        m_right = roll_right;
        break;
    case PlayerState::CUTTING:
        return;
    case PlayerState::DIGGING:
        return;
    }

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
    qreal dt = delta / 1000.0;
    QPointF center = QPointF(pos().x() + TILESIZE/ 2, pos().y() + TILESIZE / 2);
    QPointF newPos = pos() + vel * (speed * dt);
    QPointF newCenter = QPointF(newPos.x() + TILESIZE/ 2, newPos.y() + TILESIZE / 2);

    newCenter += vel * TILESIZE / 2;

    QPointF grid = gMap->pixelToGrid(center);
    QPointF newGrid = gMap->pixelToGrid(newCenter);
    if ( gMap->canMoveTo(grid.x(), grid.y(), newGrid.x(), newGrid.y()) )
    setPos(newPos);
}

void Player::findNearbyResource()
{
    if (state != PlayerState::WALKING) return;
    QPointF center = pos() + QPointF(PLAYERWIDTH / 2, PLAYERHEIGHT / 2);
    QPoint grid = gMap->pixelToGrid(center);

    static const QPoint dirs[] = {
        {0, 0}, {0, 1}, {0, -1}
    };

    Resource* r;
    for (const QPoint& d : dirs)
    {
        QPoint checkGrid = grid + d;
        r = resourceManager->checkResource(checkGrid);
        if (r && !r->isDead())
        {
            if(target != nullptr && target != r)
            {
                target->setSelected(false);
                target->update();
            }
            target = r;
            target->setSelected(true);
            target->update();
            return;
        }
    }
    if(target != nullptr && target != r)
    {
        target->setSelected(false);
        target->update();
    }
    target = nullptr;
}

QRectF Player::boundingRect() const
{
    return QRectF(0, 0, PLAYERWIDTH, PLAYERHEIGHT);
}

//绘制玩家
void Player::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*)
{
    if(isWalking())
    {
        if(isWalkingLeft)  p->drawPixmap(0, 0, leftWalking[walkIndex]);
        else  p->drawPixmap(0, 0, rightWalking[walkIndex]);
    }
    else if(isRolling())
    {
        if(isRollingLeft)  p->drawPixmap(0, 0, leftRolling[rollIndex % 4]);
        else  p->drawPixmap(0, 0, rightRolling[rollIndex % 4]);
    }
    else if(isCutting())
    {
        if(isCuttingLeft)  p->drawPixmap(0, 0, leftCutting[cutIndex]);
        else  p->drawPixmap(0, 0, rightCutting[cutIndex]);
    }
    else if(isDigging())
    {
        if(isDiggingLeft)  p->drawPixmap(0, 0, leftDigging[digIndex]);
        else  p->drawPixmap(0, 0, rightDigging[digIndex]);
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
    for(int i = 0; i < 4; i++)
    {
        leftCutting[i].load(QString(":/picture/player/cutting/%1.png").arg(i + 1));
        rightCutting[i] = leftCutting[i].transformed(QTransform().scale(-1, 1));
    }
    for(int i = 0; i < 4; i++)
    {
        leftDigging[i].load(QString(":/picture/player/digging/%1.png").arg(i + 1));
        rightDigging[i] = leftDigging[i].transformed(QTransform().scale(-1, 1));
    }
}

bool Player::isDetectable(int gridX, int gridY)
{
    QPointF gridCenter = gMap->gridToPixel(gridX, gridY);
    QPointF playerCenter = getCenterPos();
    int dx = gridCenter.x() - playerCenter.x();
    int dy = gridCenter.y() - playerCenter.y();
    double distance = sqrt(dx * dx + dy * dy);
    return distance <= detectRange;
}
