#include "bullet.h"
#include <QPainter>
#include <QTimer>
#include <cmath>
#include <QGraphicsScene>
#include <QDebug>
#include "utils.h"


Bullet::Bullet(const Vector2& startPos, const Monster* target,
               float speed, float damage, QGraphicsItem* parent)
    : QGraphicsItem(parent),
    position_(startPos),
    target(target),
    speed_(speed),
    damage_(damage),
    active_(true),
    hasHit_(false),
    tolerance_(BULLET_TOLERANCE),
    rotation_(90),
    currentFrame_(0),
    totalFrames_(1),
    pixmapSize_(20, 10)  // 默认尺寸
{
    // 加载默认子弹图片
    loadDefaultPixmap();

    setPos(QPointF(startPos.x, startPos.y));
    setZValue(BULLETZVALUE);

    updateTimer_ = new QTimer(this);
    connect(updateTimer_, &QTimer::timeout, this, &Bullet::onUpdateTimer);
    updateTimer_->start(UPDATE_INTERVAL_MS);

    frameTimer_ = new QTimer(this);
    connect(frameTimer_, &QTimer::timeout, this, &Bullet::onFrameUpdate);
    if (totalFrames_ > 1) {
        frameTimer_->start(BULLET_FRAME_DELAY);
    }
}

void Bullet::loadDefaultPixmap()
{
    // 从资源文件加载图片
    if (bulletPixmap_.load(":/images/bullet.png")) {
        pixmapSize_ = bulletPixmap_.size();
        qDebug() << "成功从资源文件加载子弹图片，尺寸：" << pixmapSize_;
    } else {
        qDebug() << "错误：无法从资源文件加载子弹图片！";
        // 如果加载失败，创建一个空的透明图片
        bulletPixmap_ = QPixmap(pixmapSize_);
        bulletPixmap_.fill(Qt::transparent);
    }
}

Bullet::~Bullet()
{
    if (updateTimer_ && updateTimer_->isActive()) {
        updateTimer_->stop();
    }
    if (frameTimer_ && frameTimer_->isActive()) {
        frameTimer_->stop();
    }
}

QRectF Bullet::boundingRect() const
{
    return QRectF(0, 0, pixmapSize_.width(), pixmapSize_.height());
}

void Bullet::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();

    // 计算中心点
    QPointF center(pixmapSize_.width() / 2.0, pixmapSize_.height() / 2.0);

    // 应用旋转
    painter->translate(center);
    painter->rotate(rotation_);
    painter->translate(-center);

    // 绘制图片
    painter->drawPixmap(0, 0, bulletPixmap_);

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
    if (totalFrames_ > 1) {
        currentFrame_ = (currentFrame_ + 1) % totalFrames_;
        update();
    }
}

void Bullet::updatePosition()
{
    if (!active_ || hasHit_) return;

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
    Vector2 targetPos_ = target->getPosition();    float distance = position_.distanceTo(targetPos_);
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
    emit hit(damage_);

    if (scene()) {
        scene()->removeItem(this);
    }
}

void Bullet::startEffect()
{
    // 命中特效
}
