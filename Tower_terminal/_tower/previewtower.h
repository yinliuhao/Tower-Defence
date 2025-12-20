#ifndef PREVIEWTOWER_H
#define PREVIEWTOWER_H

#include "_others/utils.h"
#include "QPixmap"
#include "QGraphicsItem"

class PreviewTower : public QGraphicsItem
{
public:
    PreviewTower(TowerType type);

    void setGridPos(const QPoint grid);
    void setValid(bool ok) { isValid = ok; }

    TowerType getTowerType() { return type; }
    QPoint getGridPos() const { return gridPos; }

    QRectF boundingRect() const override;
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget) override;

private:
    TowerType type;
    bool isValid = false;
    int attackRadius;
    QPixmap picture;
    QPoint gridPos;
};

#endif // PREVIEWTOWER_H
