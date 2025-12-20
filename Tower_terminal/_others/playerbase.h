#ifndef PLAYERBASE_H
#define PLAYERBASE_H

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>
#include "_others/utils.h"

class PlayerBase : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit PlayerBase(int maxHp = INITCAMPHEALTH,
                        QGraphicsItem* parent = nullptr);

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    void takeDamage(int dmg);

private:
    void updatePixmap();

signals:
    void baseDestroyed();

private:
    int maxHp_;
    int currentHp_;

    QPixmap pixFull_;
    QPixmap pixLight_;
    QPixmap pixHeavy_;
    QPixmap pixCritical_;

    QPixmap currentPixmap_;
    QSize size_;
};

#endif // PLAYERBASE_H
