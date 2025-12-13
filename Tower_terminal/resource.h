#ifndef RESOURCE_H
#define RESOURCE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <vector>

enum class ResourceType
{
    GRASS_STONE,
    SNOW_STONE,
    SWAMP_STONE,
    GRASS_TREE,
    SWAMP_TREE
};

class Resource : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Resource(ResourceType type, QPoint grid, QGraphicsItem* parent = nullptr);

    void takeDamage();

    bool getSelecterd() { return isSelected; }
    void setSelected(bool on) { isSelected = on; }
    void setTimerStart() { respawnTimer.start(); }
    QPointF getPixal() { return pixal; }
    QPointF getCenterPixal() {return pixal + QPointF(normalPic[0].width(), normalPic[0].height());}
    QPoint getGrid() { return grid; }
    bool isDead() const { return hp <= 0; }
    ResourceType getType() { return type; }

signals:
    void resourceDepleted(Resource* r);

protected slots:
    virtual void respawn();

protected:
    ResourceType type;
    int hp;
    int maxHp;
    int respawnTimeMs;
    bool isSelected;

    QTimer respawnTimer;

    std::vector<QPixmap> normalPic;
    std::vector<QPixmap> highlightPic;

    QPoint grid;
    QPointF pixal;

    void paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*);
};

class Big_rock : public Resource
{
public:
    Big_rock(ResourceType type, QPoint grid, QGraphicsItem* parent = nullptr);

protected:
    void initPicture();
    QRectF boundingRect() const override;
};


class Tree : public Resource
{
public:
    Tree(ResourceType type, QPoint grid, QGraphicsItem* parent = nullptr);

protected:
    void initPicture();
    QRectF boundingRect() const override;
};

#endif // RESOURCE_H
