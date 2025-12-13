#include "bullet.h"
#include <QPainter>
#include <QTimer>
#include <cmath>
#include <QGraphicsScene>
#include <QDebug>
#include "utils.h"
#include "tower.h"

// Bullet.cpp 中的构造函数修改
Bullet::Bullet(const Vector2& startPos, const Monster* target, TowerType towerType,
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
    rotation_(90),
    towerType_(towerType),
    towerLevel_(towerLevel),
    currentFrame_(0),
    totalFrames_(1),
    pixmapSize_(20, 10)
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
    if (totalFrames_ > 1) {
        frameTimer_->start(BULLET_FRAME_DELAY);
    }
}
// 其他函数保持不变，但确保使用正确的成员变量名
void Bullet::loadAnimatedPixmap(const QString& basePath, int frameCount)
{
    bulletFrames_.clear();  // 现在使用正确的成员变量
    for (int i = 0; i < frameCount; ++i) {
        QString framePath = QString("%1_%2.png").arg(basePath).arg(i);
        QPixmap frame(framePath);
        if (!frame.isNull()) {
            bulletFrames_.append(frame);
            qDebug() << "成功加载子弹帧：" << framePath;
        } else {
            qDebug() << "错误：无法加载子弹帧：" << framePath;
            QPixmap defaultFrame(pixmapSize_);
            defaultFrame.fill(QColor(255, 200, 0));
            bulletFrames_.append(defaultFrame);
        }
    }
    if (!bulletFrames_.isEmpty()) {
        bulletPixmap_ = bulletFrames_.first();
        pixmapSize_ = bulletPixmap_.size();
    }
}
void Bullet::loadDefaultPixmap()
{
    QString imagePath;
    QString basePath;

    switch(towerType_) {
    case TowerType::TOWER1:  // 弓箭塔 - 只有一级，单帧
        // 注意路径前缀是 :/picture/
        imagePath = ":/picture/bullet/bullet_archer.png";
        totalFrames_ = 1;
        break;

    case TowerType::TOWER2:  // 加农炮 - 三级，每级三帧动画
        // 路径前缀是 :/picture/bullet/
        basePath = QString(":/picture/bullet/bullet_cannon_lv%1").arg(towerLevel_);
        totalFrames_ = 3;  // 每级都有3帧动画
        break;

    case TowerType::TOWER3:  // 迫击炮 - 三级，每级三帧动画
        // 路径前缀是 :/picture/bullet/
        basePath = QString(":/picture/bullet/bullet_mortar_lv%1").arg(towerLevel_);
        totalFrames_ = 3;  // 每级都有3帧动画
        break;

    default:
        imagePath = ":/picture/bullet/bullet_default.png";
        totalFrames_ = 1;
        break;
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

    // 绘制图片（支持多帧动画）
    if (!bulletFrames_.isEmpty() && totalFrames_ > 1) {
        // 多帧动画
        int frameIndex = currentFrame_ % bulletFrames_.size();
        painter->drawPixmap(0, 0, bulletFrames_.at(frameIndex));
    } else {
        // 单帧图片
        painter->drawPixmap(0, 0, bulletPixmap_);
    }

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
