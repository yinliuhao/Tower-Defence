#include "map.h"
#include<qstring.h>
#include<QPainter>
#include"utils.h"

Map::Map(): tileSize(TILESIZE), width(WIDTH - BTNWIDTH), height(HEIGHT)
{
    initpic();
    setPos(0, 0);
    heightNum = height / tileSize;
    widthNum = width / tileSize;
    setZValue(MAPZVALUE);
    initializeTileData();
}

void Map::initpic()
{
    background.load(QString(":/picture/map/map.png"));
}

QRectF Map::boundingRect() const
{
    return QRectF(0, 0, width, height);
}

void Map::paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*)
{
    p->drawPixmap(0, 0, background);
}

void Map::initializeTileData()
{
    // 调整外层 vector 的大小（行数 = heightNum）
    tileData.resize(heightNum);

    // 调整内层 vector 的大小（列数 = widthNum）并初始化值
    for (int y = 0; y < heightNum; ++y) {
        tileData[y].resize(widthNum);
    }
}
