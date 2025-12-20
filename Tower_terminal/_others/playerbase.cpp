#include "_others/playerbase.h"
#include <QPainter>
#include "_others/utils.h"

PlayerBase::PlayerBase(int maxHp, QGraphicsItem* parent)
    : QObject(),
    QGraphicsItem(parent),
    maxHp_(maxHp),
    currentHp_(maxHp),
    size_(TILESIZE * 2, TILESIZE * 2)
{
    pixFull_.load(":/picture/playerBase/base_frame1.png");
    pixLight_.load(":/picture/playerBase/base_frame2.png");
    pixHeavy_.load(":/picture/playerBase/base_frame3.png");
    pixCritical_.load(":/picture/playerBase/base_frame3.png");

    updatePixmap();
    setZValue(HOMEZVALUE);
}

QRectF PlayerBase::boundingRect() const
{
    return QRectF(-size_.width() / 2,
                  -size_.height() / 2,
                  size_.width(),
                  size_.height());
}

void PlayerBase::paint(QPainter* painter,
                       const QStyleOptionGraphicsItem*,
                       QWidget*)
{
    painter->drawPixmap(boundingRect().toRect(),
                        currentPixmap_);
}

void PlayerBase::takeDamage(int dmg)
{
    currentHp_ -= dmg;
    if (currentHp_ < 0)
        currentHp_ = 0;

    updatePixmap();
    update();

    if (currentHp_ == 0)
        emit baseDestroyed();
}

void PlayerBase::updatePixmap()
{
    float ratio = float(currentHp_) / maxHp_;

    if (ratio >= 0.75f)
        currentPixmap_ = pixFull_;
    else if (ratio >= 0.5f)
        currentPixmap_ = pixLight_;
    else if (ratio >= 0.25f)
        currentPixmap_ = pixHeavy_;
    else
        currentPixmap_ = pixCritical_;
}
