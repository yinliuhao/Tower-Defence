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
#define BULLET_DAMAGE 10.0f
#define BULLET_TOLERANCE 5.0f
#define UPDATE_INTERVAL_MS 16
#define BULLET_FRAME_DELAY 100

class Bullet : public QObject, public QGraphicsItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsItem)
public:
    // 统一构造函数声明
    Bullet(const Vector2& startPos, Monster* target, TowerType towerType,
           int towerLevel = 1,
           float speed = BULLET_SPEED, float damage = BULLET_DAMAGE, QGraphicsItem* parent = nullptr);

    ~Bullet();

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;
    QPainterPath shape() const override;

    void updatePosition();

    bool isActive() const { return active_; }
    bool hasHit() const { return hasHit_; }
    const Vector2 getPosition() const { return position_; }
    Monster* getTarget() { return target; }
    float getDamage() { return damage_; }
    float getRotation() const { return rotation_; }
    TowerType getTowerType() const { return towerType_; }
    int getTowerLevel() const { return towerLevel_; }

    void startEffect();
    void setBulletPixmap(const QPixmap& pixmap);
    void setBulletPixmap(const QString& imagePath);

public slots:
    void onUpdateTimer();
    void onFrameUpdate();

protected:  // 改为protected，让派生类可以访问
    Vector2 position_;
    QPointer<Monster> target;
    float speed_;
    float damage_;
    bool active_;
    bool hasHit_;
    float tolerance_;
    float rotation_;  //角度制
    TowerType towerType_;  // 只保留一个声明
    int towerLevel_;

    QTimer* updateTimer_;
    QTimer* frameTimer_;
    QPixmap bulletPixmap_;
    QVector<QPixmap> bulletFrames_;  // 添加动画帧容器
    int currentFrame_;
    int totalFrames_;
    QSize pixmapSize_;

    void loadDefaultPixmap();
    void loadAnimatedPixmap();

    bool isAtTarget() const;
    virtual void hitTarget();  // 改为虚函数

signals:
    void hit(int damage);
};

#endif // BULLET_H
