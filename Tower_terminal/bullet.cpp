#include "bullet.h"
#include <QPainter>
#include <QGraphicsScene>
#include <cmath>
#include <fstream>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include "utils.h"
#include "structVector.h"

Bullet::Bullet(const Vector2& startPos,
               Monster* target,
               TowerType towerType,
               int towerLevel,
               float speed,
               float damage,
               QGraphicsItem* parent)
    : QObject(nullptr),
    QGraphicsItem(parent),
    position_(startPos),
    target(target),
    speed_(speed),
    damage_(damage),
    tolerance_(BULLET_TOLERANCE),
    towerType_(towerType),
    towerLevel_(towerLevel)
{
    qDebug() << "[Bullet::Ctor]" << static_cast<void*>(this)
             << "target:" << static_cast<void*>(target);

    setPos(QPointF(startPos.x, startPos.y));
    setZValue(BULLETZVALUE);

    loadAnimatedPixmap();
    loadDefaultPixmap();

    updateTimer_ = new QTimer(this);
    connect(updateTimer_, &QTimer::timeout, this, &Bullet::onUpdateTimer);
    updateTimer_->start(UPDATE_INTERVAL_MS);

    frameTimer_ = new QTimer(this);
    connect(frameTimer_, &QTimer::timeout, this, &Bullet::onFrameUpdate);
    frameTimer_->setInterval(BULLET_FRAME_DELAY);

    if (target) {
        connect(target, &Monster::died, this, [this](int){
            qDebug() << "[Bullet] target died"
                     << "bullet:" << static_cast<void*>(this)
                     << "old target:" << static_cast<void*>(this->target);
            this->target = nullptr;
        });
    }
}

Bullet::~Bullet()
{
    // QTimer 作为子对象会自动销毁
}

QRectF Bullet::boundingRect() const
{
    qreal r = std::sqrt(pixmapSize_.width() * pixmapSize_.width() +
                        pixmapSize_.height() * pixmapSize_.height());
    return QRectF(-r/2, -r/2, r, r);
}

void Bullet::paint(QPainter* painter,
                   const QStyleOptionGraphicsItem*,
                   QWidget*)
{
    if (dying_) return;

    painter->save();
    painter->rotate(rotation_);

    QPointF drawPos(-pixmapSize_.width() / 2,
                    -pixmapSize_.height() / 2);

    if (!bulletFrames_.isEmpty() && currentFrame_ != 0)
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

void Bullet::onUpdateTimer()
{
    updatePosition();
}

void Bullet::updatePosition()
{
    qDebug() << "[Bullet::updatePosition]"
             << static_cast<void*>(this)
             << "dying:" << dying_
             << "hasHit:" << hasHit_
             << "target:" << static_cast<void*>(target);

    // #region agent log
    /*{
        try {
            QDir().mkpath(".cursor");
            QFile file(".cursor/debug.log");
            if (file.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream ts(&file);
                ts << "{\"sessionId\":\"debug-session\","
                      "\"runId\":\"pre-fix\","
                      "\"hypothesisId\":\"H2\","
                      "\"location\":\"bullet.cpp:96\","
                      "\"message\":\"Bullet::updatePosition\","
                      "\"data\":{\"bullet\":\"" << this
                   << "\",\"target\":\"" << target
                   << "\",\"dying\":" << (dying_ ? "true" : "false")
                   << ",\"hasHit\":" << (hasHit_ ? "true" : "false")
                   << "},"
                      "\"timestamp\":" << static_cast<long long>(QDateTime::currentMSecsSinceEpoch())
                   << "}\n";
            }
        } catch (...) {
        }
    }*/
    // #endregion

    if (dying_ || !target) {
        //qDebug() << " -> destroySelf";
        destroySelf();
        return;
    }

    Vector2 targetPos = target->getPosition();
    Vector2 dir = targetPos - position_;
    float dist = dir.distanceTo(Vector2(0, 0));

    if (dist > 0.f) {
        dir = dir.normalized();
        rotation_ = std::atan2(dir.y, dir.x) * 180 / M_PI;
        position_ = position_ + dir * speed_;
        setPos(QPointF(position_.x, position_.y));
    }

    if (isAtTarget()) {
        hitTarget();
    }
}

bool Bullet::isAtTarget() const
{
    if (!target) return false;
    return position_.distanceTo(target->getPosition()) <= tolerance_;
}

void Bullet::hitTarget()
{
    //qDebug() << "[Bullet::hitTarget]"
    //         << static_cast<void*>(this)
    //         << "target:" << static_cast<void*>(target)
    //         << "hasHit:" << hasHit_
    //         << "dying:" << dying_;

    if (hasHit_ || dying_ || !target) return;

    hasHit_ = true;

    // #region agent log
    /*{
        try {
            QFile file(".cursor/debug.log");
            if (file.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream ts(&file);
                ts << "{\"sessionId\":\"debug-session\","
                      "\"runId\":\"pre-fix\","
                      "\"hypothesisId\":\"H2\","
                      "\"location\":\"bullet.cpp:133\","
                      "\"message\":\"Bullet::hitTarget\","
                      "\"data\":{\"bullet\":\"" << this
                   << "\",\"target\":\"" << target
                   << "\",\"damage\":" << damage_
                   << "},"
                      "\"timestamp\":" << static_cast<long long>(QDateTime::currentMSecsSinceEpoch())
                   << "}\n";
            }
        } catch (...) {
        }
    }*/
    // #endregion

    updateTimer_->stop();
    if (totalFrames_ > 1) {
        currentFrame_ = 0;
        frameTimer_->start();
    } else {
        emit hit(damage_);
        qDebug() << " -> takeDamage";
        takeDamage();
    }
}

void Bullet::onFrameUpdate()
{
    if (++currentFrame_ > totalFrames_) {
        //qDebug() << " -> takeDamage";
        dying_ = true;
        takeDamage();
        return;
    }
    update();
}

void Bullet::destroySelf()
{
    qDebug() << "[Bullet::destroySelf]" << static_cast<void*>(this);

    if (dying_) {
        if (scene()) scene()->removeItem(this);
        deleteLater();
    }
}

void Bullet::loadAnimatedPixmap()
{
    bulletFrames_.clear();
    totalFrames_ = 1;

    if (towerType_ == TowerType::TOWER3) {
        for (int i = 1; i <= 4; ++i) {
            bulletFrames_.push_back(
                QPixmap(QString(":/picture/bullet/mortar_level%1_frame%2.png")
                            .arg(towerLevel_).arg(i)));
        }
        totalFrames_ = bulletFrames_.size();
    }

    if (!bulletFrames_.isEmpty()) {
        pixmapSize_ = bulletFrames_.first().size();
    }
}

void Bullet::loadDefaultPixmap()
{
    switch (towerType_) {
    case TowerType::TOWER1:
        bulletPixmap_ = QPixmap(":/picture/bullet/archer.png");
        break;
    case TowerType::TOWER2:
        bulletPixmap_ =
            QPixmap(QString(":/picture/bullet/cannon_level%1.png").arg(towerLevel_));
        break;
    case TowerType::TOWER3:
        bulletPixmap_ =
            QPixmap(QString(":/picture/bullet/mortar_level%1.png").arg(towerLevel_));
        break;
    default:
        break;
    }
    if (!bulletPixmap_.isNull())
        pixmapSize_ = bulletPixmap_.size();
}

void Bullet::takeDamage()
{
    target->takeDamage(damage_);
    destroySelf();
}
