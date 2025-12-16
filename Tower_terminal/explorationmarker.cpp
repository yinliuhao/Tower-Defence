#include "ExplorationMarker.h"
#include <QPainter>
#include <QPixmap>
#include "global.h"
#include "utils.h"
#include <QtGlobal>

ExplorationMarker::ExplorationMarker(QGraphicsPixmapItem* parent)
    : QGraphicsPixmapItem(parent)
{
    // 设置 item 的大小和位置
    setPos(0, 0);
    setZValue(MARKERZVALUE); // 假设 MARKERZVALUE 稍高于地图但低于炮塔/怪物
    setMarked(false);

    picture.load(QString(":/picture/tower/explorationMarker.png"));
}

void ExplorationMarker::setMarked(bool marked)
{
    if (isMarked_ != marked) {
        isMarked_ = marked;
        setVisible(marked); // 只在标记为 true 时才显示
        update();
    }
}

void ExplorationMarker::paint(QPainter *painter,
                              const QStyleOptionGraphicsItem *,
                              QWidget *)
{
    if (!isMarked_) return;
    detectPoints();

    for(QPoint point : gridPoints)
    {
        if(gMap->isBuildable(point.x(), point.y()))
        {
            painter->drawPixmap(point.y() * TILESIZE, point.x() * TILESIZE, picture);
        }
    }
}

void ExplorationMarker::detectPoints()
{
    QPointF playerCenter = me->getCenterPos();
    qreal range = me->getRange();

    int minY = qMax(0, (int)(playerCenter.x() - range) / TILESIZE);
    int maxY = qMin(90, (int)(playerCenter.x() + range) / TILESIZE + 1);
    int minX = qMax(0, (int)(playerCenter.y() - range) / TILESIZE);
    int maxX = qMin(60, (int)(playerCenter.y() + range) / TILESIZE + 1);

    gridPoints.clear();
    for(int gridY = minY; gridY < maxY; gridY++)
    {
        for(int gridX = minX; gridX < maxX; gridX++)
        {
            if(gMap->isBuildable(gridX, gridY))
            {
                QPointF gridCenter = gMap->gridToPixel(gridX, gridY);
                int dx = gridCenter.x() - playerCenter.x();
                int dy = gridCenter.y() - playerCenter.y();
                double distance = sqrt(dx * dx + dy * dy);
                if(distance <= range)
                {
                    gridPoints.push_back(QPoint(gridX, gridY));
                }
            }
        }
    }
}

QRectF ExplorationMarker::boundingRect() const
{
    return QRectF(0, 0, MAPWIDTH, MAPHEIGHT);
}
