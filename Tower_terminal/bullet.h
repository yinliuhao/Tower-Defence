#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QGraphicsItem>
#include <QTimer>
#include <QPixmap>
#include "structVector.h"
#include "monster.h"

#define BULLET_SPEED 8.0f
#define BULLET_TOLERANCE 5.0f
#define UPDATE_INTERVAL_MS 16
#define BULLET_FRAME_DELAY 100

class Bullet : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Bullet(const Vector2& startPos, const Vector2& targetPos,
           float speed = BULLET_SPEED, QGraphicsItem* parent = nullptr);
    ~Bullet();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    QPainterPath shape() const override;

    void updatePosition();
    bool isActive() const { return active_; }
    bool hasHit() const { return hasHit_; }
    const Vector2& getPosition() const { return position_; }
    const Vector2& getTargetPosition() const { return targetPos_; }
    float getRotation() const { return rotation_; }

    void startEffect();
    void setBulletPixmap(const QPixmap& pixmap);
    void setBulletPixmap(const QString& imagePath);

public slots:
    void onUpdateTimer();
    void onFrameUpdate();

private:
    Vector2 position_;
    Vector2 targetPos_;
    float speed_;
    bool active_;
    bool hasHit_;
    float tolerance_;
    float rotation_;

    QTimer* updateTimer_;
    QTimer* frameTimer_;
    QPixmap bulletPixmap_;
    int currentFrame_;
    int totalFrames_;
    QSize pixmapSize_;

    void loadDefaultPixmap();
    bool isAtTarget() const;
    void hitTarget();

    Monster * target;

signals:
    void hit(const Vector2& position);
};

#endif // BULLET_H
