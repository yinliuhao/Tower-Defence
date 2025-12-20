#ifndef EXPLORATION_MARKER_H
#define EXPLORATION_MARKER_H

#include <QVector>
#include <QPoint>
#include <QGraphicsPixmapItem>
#include "QObject"

class ExplorationMarker : public QObject
{
    Q_OBJECT
public:
    ExplorationMarker(QGraphicsScene* scene);
    ~ExplorationMarker();

    void setMarked(bool marked);
    void refreshMarkers();

private:
    void detectPoints();
    void clearMarkers();
    void createMarkers();

private:
    bool isMarked_ = false;

    QPixmap picture;
    QVector<QPoint> gridPoints;

    QVector<QGraphicsPixmapItem*> markerItems;
    QGraphicsScene* scene_;
};

#endif
