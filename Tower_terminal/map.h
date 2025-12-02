#ifndef MAP_H
#define MAP_H

#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QTime>
#include <vector>
#include <QPainter>
#include <QDebug>
#include "structVector.h"

// 格子状态枚举
enum TileState {
    TILE_WALKABLE = 0,   // 可通行（怪物路径）
    TILE_BUILDABLE = 1,  // 可建造（放置塔） - 平地
    TILE_BLOCKED = 2,    // 障碍物
    TILE_START = 3,      // 怪物起点
    TILE_END = 4,        // 怪物终点
    TILE_WATER = 5,      // 水池（最低层）
    TILE_MOUNTAIN = 6,   // 山地（最高层）
    TILE_LADDER = 7      // 梯子（连接不同层的特殊格子）
};

// 图层枚举
enum TileLayer {
    LAYER_WATER = 0,     // 水池层
    LAYER_GROUND = 1,    // 平地层
    LAYER_MOUNTAIN = 2   // 山地层
};

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
    bool isWalkable(int gridX, int gridY) const;
    bool isBuildable(int gridX, int gridY) const;
    bool isBlocked(int gridX, int gridY) const;
    bool isWater(int gridX, int gridY) const;
    bool isMountain(int gridX, int gridY) const;
    bool isLadder(int gridX, int gridY) const;
    TileState getTileState(int gridX, int gridY) const;
    TileLayer getTileLayer(int gridX, int gridY) const;
    void setTileState(int gridX, int gridY, TileState state);

    // 边界检查
    bool isInsideMap(const QPointF& pixelPos) const;
    bool isGridInsideMap(int gridX, int gridY) const;

    // 路径相关
    void initializeTileData();
    std::vector<Vector2> findPath(const Vector2& start, const Vector2& end) const;
    std::vector<Vector2> getMonsterPath() const;
    void setStartPoint(int gridX, int gridY);
    void setEndPoint(int gridX, int gridY);
    void generateMonsterPath();

    // 高度检查（玩家移动用）
    bool canMoveTo(int fromX, int fromY, int toX, int toY) const;
    int getHeightDifference(int x1, int y1, int x2, int y2) const;

    // 塔相关
    bool canPlaceTower(const QPointF& pixelPos) const;
    bool placeTower(const QPointF& pixelPos);
    bool removeTower(const QPointF& pixelPos);

    // 绘制相关（可选，用于调试）
    void setShowGrid(bool show);

protected:
    void initpic();
    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    // A*算法辅助结构（查到的移动算法，慢慢研究）
    struct AStarNode {
        int x, y;
        float gCost;  // 从起点到当前节点的代价
        float hCost;  // 到终点的预估代价（启发式）
        float fCost() const { return gCost + hCost; }
        AStarNode* parent;

        bool operator>(const AStarNode& other) const {
            return fCost() > other.fCost();
        }

        bool operator==(const AStarNode& other) const {
            return x == other.x && y == other.y;
        }
    };

    // A*算法辅助函数
    float calculateHeuristic(int x1, int y1, int x2, int y2) const;
    std::vector<QPoint> getNeighbors(int x, int y) const;
    std::vector<Vector2> reconstructPath(AStarNode* endNode) const;

    // 地形高度计算
    int getTerrainHeight(int x, int y) const;
    bool isHeightTransitionValid(int fromHeight, int toHeight) const;

    // 成员变量
    int tileSize;   // 正方形地块边长
    QPixmap background;
    int width;      // 地图宽度（像素）
    int height;     // 地图高度（像素）
    int widthNum;   // 地图宽上的地块数
    int heightNum;  // 地图高上的地块数
    std::vector<std::vector<int>> tileData;  // 存储地图信息

    // 路径相关
    QPoint startPoint;  // 怪物起点（格子坐标）
    QPoint endPoint;    // 怪物终点（格子坐标）
    std::vector<Vector2> monsterPath;  // 生成的怪物路径

    // 绘制选项
    bool showGrid;
};

#endif // MAP_H
