#ifndef MAP_H
#define MAP_H

#include<qpixmap.h>
#include<QGraphicsPixmapItem>

class Map : public QGraphicsPixmapItem
{
public:
    Map();
    int getWidth()   {return width;}
    int getHeight()  {return height;}
    int gettileSize()  {return tileSize;}
    void initializeTileData();

protected:
    void initpic();
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem*, QWidget*) override;

private:
    int tileSize;   //正方形地块边长
    QPixmap background;
    int width;    //地图宽度
    int height;   //地图高度
    int widthNum;  //地图宽上的地块数
    int heightNum;  //地图高上的地块数
    std::vector<std::vector<int>> tileData;  //存储真正的地图
};

#endif // MAP_H
