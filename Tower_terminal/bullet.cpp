#include "bullet.h"
#include <QPainter>
#include <QTimer>
#include <cmath>
#include <QGraphicsScene>
#include <QDebug>
#include "utils.h"
#include "tower.h"

// Bullet.cpp 中的构造函数修改
Bullet::Bullet(const Vector2& startPos, Monster* target, TowerType towerType,
               int towerLevel, float speed, float damage, QGraphicsItem* parent)
    : QObject(nullptr),  // 先初始化 QObject
    QGraphicsItem(parent),  // 再初始化 QGraphicsItem
    position_(startPos),
    target(target),
    speed_(speed),
    damage_(damage),
    active_(true),
    hasHit_(false),
    tolerance_(BULLET_TOLERANCE),
    rotation_(0),
    towerType_(towerType),
    towerLevel_(towerLevel),
    currentFrame_(0)
{
    // 初始化指针为 nullptr
    updateTimer_ = nullptr;
    frameTimer_ = nullptr;

    loadDefaultPixmap();
    setPos(QPointF(startPos.x, startPos.y));
    setZValue(BULLETZVALUE);

    updateTimer_ = new QTimer(this);
    connect(updateTimer_, &QTimer::timeout, this, &Bullet::onUpdateTimer);
    updateTimer_->start(UPDATE_INTERVAL_MS);

    frameTimer_ = new QTimer(this);
    connect(frameTimer_, &QTimer::timeout, this, &Bullet::onFrameUpdate);
    frameTimer_->setInterval(BULLET_FRAME_DELAY);

    loadAnimatedPixmap();
    loadDefaultPixmap();
}

void Bullet::loadAnimatedPixmap()
{
    bulletFrames_.clear();  // 现在使用正确的成员变量
    if(towerType_ == TowerType::TOWER2)
    {
        bulletFrames_.clear();
        // 加载并处理每一帧图片
        for(int i = 1; i <= 4; i++)
        {
            bulletFrames_.push_back(QPixmap(QString(":/picture/bullet/mortar_level%1_").arg(towerLevel_) +
                                            QString("/frame%1.png").arg(i)));
            totalFrames_ = 4;
        }
    }
    else  totalFrames_ = 1;
}
void Bullet::loadDefaultPixmap()
{
    switch(towerType_) {
    case TowerType::TOWER1:  // 弓箭塔 - 只有一级，单帧
        bulletPixmap_ = QPixmap(QString(":/picture/bullet/archer.png"));
        break;

    case TowerType::TOWER2:  // 加农炮 - 三级，每级三帧动画
        bulletPixmap_ = QPixmap(QString(":/picture/bullet/cannon_level%1.png").arg(towerLevel_));
        break;

    case TowerType::TOWER3:  // 迫击炮 - 三级,，每级四帧动画
        bulletPixmap_ = QPixmap(QString(":/picture/bullet/mortar_level%1_frame1.png").arg(towerLevel_));
        break;

    default:
        break;  
    }
    pixmapSize_ = bulletPixmap_.size();
}
Bullet::~Bullet()
{
    if (updateTimer_ && updateTimer_->isActive()) {
        updateTimer_->stop();
        updateTimer_->deleteLater();
    }
    if (frameTimer_ && frameTimer_->isActive()) {
        frameTimer_->stop();
        frameTimer_->deleteLater();
    }
}

QRectF Bullet::boundingRect() const
{
    qreal w = pixmapSize_.width();
    qreal h = pixmapSize_.height();
    qreal r = std::sqrt(w*w + h*h); // 对角线长度

    return QRectF(-r/2, -r/2, r, r);
}

void Bullet::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();

    painter->translate(0, 0);
    painter->rotate(rotation_);

    QPointF drawPos(-pixmapSize_.width() / 2,
                    -pixmapSize_.height() / 2);

    if (currentFrame_ > 0 && currentFrame_ <= bulletFrames_.size())
        painter->drawPixmap(drawPos, bulletFrames_[currentFrame_ - 1]);
    else
        painter->drawPixmap(drawPos, bulletPixmap_);

    painter->restore();
}

QPainterPath Bullet::shape() const
{
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}

void Bullet::setBulletPixmap(const QPixmap& pixmap)
{
    bulletPixmap_ = pixmap;
    pixmapSize_ = pixmap.size();
    update();
}

void Bullet::setBulletPixmap(const QString& imagePath)
{
    if (bulletPixmap_.load(imagePath)) {
        pixmapSize_ = bulletPixmap_.size();
        update();
    }
}

void Bullet::onUpdateTimer()
{
    updatePosition();
}

void Bullet::onFrameUpdate()
{
    currentFrame_++;
    if(currentFrame_ > totalFrames_)
    {
        frameTimer_->stop();
        if (scene()) {
            scene()->removeItem(this);
        }
    }
    if (totalFrames_ >= 1) {
        update();
    }

}

void Bullet::updatePosition()
{
    if (!active_ || hasHit_) return;

    if (!target) {
        active_ = false;
        if (updateTimer_) updateTimer_->stop();
        if (scene()) scene()->removeItem(this);
        return;
    }

    Vector2 targetPos_ = target->getPosition();
    Vector2 direction = targetPos_ - position_;
    float distance = direction.distanceTo(Vector2(0, 0));

    if (distance > 0) {
        direction = direction.normalized();
        rotation_ = std::atan2(direction.y, direction.x) * 180 / M_PI;
        position_ = position_ + direction * speed_;
        setPos(QPointF(position_.x, position_.y));
    }

    if (isAtTarget()) {
        hitTarget();
    }

    if (position_.x < 0 || position_.x > MAPWIDTH ||
        position_.y < 0 || position_.y > MAPHEIGHT) {
        active_ = false;
        if (updateTimer_ && updateTimer_->isActive()) {
            updateTimer_->stop();
        }
        if (scene()) {
            scene()->removeItem(this);
        }
    }
}

bool Bullet::isAtTarget() const
{
    if (!target) return false;

    Vector2 targetPos_ = target->getPosition();
    float distance = position_.distanceTo(targetPos_);
    return distance <= tolerance_;
}


void Bullet::hitTarget()
{
    hasHit_ = true;
    active_ = false;

    if (updateTimer_ && updateTimer_->isActive()) {
        updateTimer_->stop();
    }

    startEffect();
    target->takeDamage(damage_);
}

void Bullet::startEffect()
{
    if(totalFrames_ > 1)
        frameTimer_->start();
    else{
        if (scene()) {
            scene()->removeItem(this);
        }
    }
}
