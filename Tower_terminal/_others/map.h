#ifndef MAP_H
#define MAP_H

#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QTime>
#include <vector>
#include <QPainter>
#include <QDebug>
#include "_others/utils.h"

class Map : public QGraphicsPixmapItem
{
public:
    Map();
    ~Map();

    // 基础信息获取
    int getWidth()   {return width;}
    int getHeight()  {return height;}
    int gettileSize()  {return tileSize;}
    int getWidthNum() {return widthNum;}
    int getHeightNum() {return heightNum;}

    // 坐标转换（格子和像素间）
    QPoint pixelToGrid(const QPointF& pixelPos) const;
    QPointF gridToPixel(const QPoint& gridPos) const;
    QPointF gridToPixel(int gridX, int gridY) const;

    // 格子查询（对应上面的分类，可以继续添加）
    bool isBuildable(int gridX, int gridY) const;
    bool isBlocked(int gridX, int gridY) const;
    bool isLadder(int gridX, int gridY) const;
    TileState getTileState(int gridX, int gridY) const;
    TileLayer getTileLayer(int gridX, int gridY) const;
    void setTileState(int gridX, int gridY, TileState state);

    // 边界检查
    bool isInsideMap(const QPointF& pixelPos) const;
    bool isGridInsideMap(int gridX, int gridY) const;

    // 路径相关
    void initializeTileData();
    void setStartPoint(int gridX, int gridY);
    void setEndPoint(int gridX, int gridY);
    void generateMonsterPath();

    bool canMoveTo(int fromX, int fromY, int toX, int toY) const;
    // 塔相关
    bool canPlaceTower(const QPoint& gridPos) const;
    bool placeTower(const QPointF& pixelPos);
    bool removeTower(const QPointF& pixelPos);

    // 绘制相关（可选，用于调试）
    void setShowGrid(bool show);

    //公开数据
    std::vector<QPointF> monsterPath;  // 生成的怪物路径
    std::vector<QPointF> monsterHome;
    std::vector<QPoint> playerHome;
    std::vector<QPoint> grassStone;
    std::vector<QPoint> swampStone;
    std::vector<QPoint> snowStone;
    std::vector<QPoint> grassTree;
    std::vector<QPoint> swampTree;
    std::vector<std::vector<TileState>> tileState;
    std::vector<std::vector<TileLayer>> tileLayer;

protected:
    void initpic();
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    // 成员变量
    int tileSize;   // 正方形地块边长
    QPixmap background;
    int width;      // 地图宽度（像素）
    int height;     // 地图高度（像素）
    int widthNum;   // 地图宽上的地块数
    int heightNum;  // 地图高上的地块数

    // 路径相关
    QPoint startPoint;  // 怪物起点（格子坐标）
    QPoint endPoint;    // 怪物终点（格子坐标）

    // 绘制选项
    bool showGrid;
};

#endif // MAP_H
