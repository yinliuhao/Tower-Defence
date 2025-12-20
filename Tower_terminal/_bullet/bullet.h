#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QGraphicsItem>
#include <QTimer>
#include <QPixmap>
#include <QPointer>
#include "_others/structVector.h"
#include "_monster/monster.h"
#include "_tower/tower.h"
#include "_others/utils.h"

class Bullet : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)

public:
    Bullet(const Vector2& startPos,
           Monster* target,
           TowerType towerType,
           int towerLevel = 1,
           float speed = BULLET_SPEED_DEFAULT,
           float damage = BULLET_DAMAGE_DEFAULT,
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
