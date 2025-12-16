#ifndef EXPLORATIONMARKER_H
#define EXPLORATIONMARKER_H

#include <QGraphicsPixmapItem>
#include <vector>

class ExplorationMarker : public QGraphicsPixmapItem
{
public:
    ExplorationMarker(QGraphicsPixmapItem* parent = nullptr);

    // 更新标记是否可见
    void setMarked(bool marked);

    void detectPoints();

private:
    bool isMarked_ = false;
    std::vector<QPoint> gridPoints;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr) override;

    QRectF boundingRect() const override;

    QPixmap picture;
};

#endif // EXPLORATIONMARKER_H
