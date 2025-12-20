#include "_tower/explorationMarker.h"
#include "_others/global.h"
#include "_others/utils.h"
#include <QtGlobal>
#include <cmath>
#include "qgraphicsscene.h"

ExplorationMarker::ExplorationMarker(QGraphicsScene* scene)
    : scene_(scene)
{
    picture.load(":/picture/tower/explorationMarker.png");
}

ExplorationMarker::~ExplorationMarker()
{
    clearMarkers();
}

void ExplorationMarker::setMarked(bool marked)
{
    if (isMarked_ == marked)
        return;

    isMarked_ = marked;

    if (!isMarked_) {
        clearMarkers();
        return;
    }

    detectPoints();
    createMarkers();
}

void ExplorationMarker::createMarkers()
{
    clearMarkers();

    for (const QPoint& point : gridPoints)
    {
        auto* item = new QGraphicsPixmapItem(picture);
        item->setPos(point.y() * TILESIZE,
                     point.x() * TILESIZE);
        item->setZValue(MARKERZVALUE);

        scene_->addItem(item);
        markerItems.push_back(item);
    }
}

void ExplorationMarker::clearMarkers()
{
    for (auto* item : markerItems) {
        scene_->removeItem(item);
        delete item;
    }
    markerItems.clear();
}

void ExplorationMarker::detectPoints()
{
    QPointF playerCenter = me->getCenterPos();
    qreal range = me->getRange();

    int minY = qMax(0, int((playerCenter.x() - range) / TILESIZE));
    int maxY = qMin(90, int((playerCenter.x() + range) / TILESIZE) + 1);
    int minX = qMax(0, int((playerCenter.y() - range) / TILESIZE));
    int maxX = qMin(60, int((playerCenter.y() + range) / TILESIZE) + 1);

    gridPoints.clear();

    for (int gridY = minY; gridY < maxY; gridY++)
    {
        for (int gridX = minX; gridX < maxX; gridX++)
        {
            if (!gMap->isBuildable(gridX, gridY))
                continue;

            QPointF gridCenter = gMap->gridToPixel(gridX, gridY);
            qreal dx = gridCenter.x() - playerCenter.x();
            qreal dy = gridCenter.y() - playerCenter.y();

            if (std::sqrt(dx * dx + dy * dy) <= range) {
                gridPoints.push_back(QPoint(gridX, gridY));
            }
        }
    }
}

void ExplorationMarker::refreshMarkers()
{
    if(!isMarked_) return;
    clearMarkers();
    detectPoints();
    createMarkers();
}
