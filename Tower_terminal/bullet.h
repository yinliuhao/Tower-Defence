#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QGraphicsItem>
#include <QTimer>
#include <QPixmap>
#include <QPointer>
#include "structVector.h"
#include "monster.h"
#include "tower.h"

#define BULLET_SPEED 8.0f
#define BULLET_DAMAGE 5.0f
#define BULLET_TOLERANCE 5.0f
#define UPDATE_INTERVAL_MS 16
#define BULLET_FRAME_DELAY 25

class Bullet : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Bullet(const Vector2& startPos,
           Monster* target,
           TowerType towerType,
           int towerLevel = 1,
           float speed = BULLET_SPEED,
           float damage = BULLET_DAMAGE,
           QGraphicsItem* parent = nullptr);

    ~Bullet() override;

    QRectF boundingRect() const override;
    void paint(QPainter*, const QStyleOptionGraphicsItem*, QWidget*) override;
    QPainterPath shape() const override;

    Monster* getTarget() { return target; }
    float getDamage() { return damage_; }

protected slots:
    void onUpdateTimer();
    void onFrameUpdate();


protected:
    void updatePosition();
    void hitTarget();
    void destroySelf();

    bool isAtTarget() const;

    void loadDefaultPixmap();
    void loadAnimatedPixmap();
    virtual void takeDamage();

protected:
    Vector2 position_;
    QPointer<Monster> target;

    float speed_;
    float damage_;
    float tolerance_;
    float rotation_ = 0.f;

    bool active_ = true;
    bool hasHit_ = false;
    bool dying_ = false;

    TowerType towerType_;
    int towerLevel_;

    QTimer* updateTimer_ = nullptr;
    QTimer* frameTimer_  = nullptr;

    QPixmap bulletPixmap_;
    QVector<QPixmap> bulletFrames_;
    QSize pixmapSize_;

    int currentFrame_ = 0;
    int totalFrames_  = 1;

signals:
    void hit(int damage);
};

#endif
