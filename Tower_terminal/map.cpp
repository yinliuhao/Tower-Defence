#include "map.h"
#include <QString>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <QDebug>
#include <cmath>
#include "utils.h"
#include<fstream>


Map::Map(): tileSize(TILESIZE), width(MAPWIDTH), height(MAPHEIGHT), showGrid(false)
{
    initpic();
    setPos(0, 0);

    // 计算格子数量
    heightNum = height / tileSize;  // 1920 / 32 = 60
    widthNum = width / tileSize;    // 2880 / 32 = 90

    setZValue(MAPZVALUE);
    initializeTileData();
}

void Map::initializeTileData()
{
    std::ifstream ifs1("tileState.txt");
    if(!ifs1.is_open()) {
        qDebug() << "加载失败！";
        return;
    }

    tileState.resize(heightNum);
    for(int i = 0; i < heightNum; i++){
        tileState[i].resize(widthNum);
    }
    for(int i = 0; i < heightNum; i++){
        for(int j = 0; j < widthNum; j++){
            int tmp;
            ifs1 >> tmp;
            tileState[i][j] = (TileState)tmp;
            qDebug() << tileState[i][j];
        }
    }

    ifs1.close();

    std::ifstream ifs2("tileLayer.txt");
    if(!ifs2.is_open()) {
        qDebug() << "加载失败！";
        return;
    }

    tileLayer.resize(heightNum);
    for(int i = 0; i < heightNum; i++){
        tileLayer[i].resize(widthNum);
    }
    for(int i = 0; i < heightNum; i++){
        for(int j = 0; j < widthNum; j++){
            int tmp;
            ifs2 >> tmp;
            tileLayer[i][j] = (TileLayer)tmp;
        }
    }

    ifs2.close();

    monsterPath.resize(19);
    monsterPath = {QPointF(6, 4), QPointF(6, 14), QPointF(21, 14), QPointF(21, 9), QPointF(32, 9), QPointF(32, 20),
                   QPointF(22, 20), QPointF(22, 27), QPointF(8, 27), QPointF(8, 23), QPointF(31, 23), QPointF(31, 31),
                   QPointF(40, 31), QPointF(40, 23), QPointF(46, 23), QPointF(46, 20), QPointF(46, 17), QPointF(47, 17), QPointF(47, 7)};
}

//注意！！！像素的原点为左上点，x轴为从左到右，y轴为从上到下
        //格子的原点为左上点，x轴为从上到下，y轴为从左到右
// 坐标转换：像素坐标 -> 格子坐标
QPoint Map::pixelToGrid(const QPointF& pixelPos) const {
    int gridX = static_cast<int>(pixelPos.y()) / tileSize;
    int gridY = static_cast<int>(pixelPos.x()) / tileSize;

    // 确保在有效范围内
    gridX = qMax(0, qMin(gridX, heightNum - 1));
    gridY = qMax(0, qMin(gridY, widthNum - 1));

    return QPoint(gridX, gridY);
}

// 坐标转换：格子坐标 -> 像素坐标(格子中心)
QPointF Map::gridToPixel(const QPoint& gridPos) const {
    return gridToPixel(gridPos.x(), gridPos.y());
}

QPointF Map::gridToPixel(int gridX, int gridY) const {
    float pixelX = gridY * tileSize + tileSize / 2.0f;
    float pixelY = gridX * tileSize + tileSize / 2.0f;
    return QPointF(pixelX, pixelY);
}

void Map::setTileState(int gridX, int gridY, TileState state)
{
    tileState[gridX][gridY] = state;
}


// 检查格子是否在有效范围内
bool Map::isGridInsideMap(int gridX, int gridY) const {
    return gridX >= 0 && gridX < heightNum && gridY >= 0 && gridY < widthNum;
}

// 检查像素坐标是否在地图内
bool Map::isInsideMap(const QPointF& pixelPos) const {
    return pixelPos.x() >= 0 && pixelPos.x() < width &&
           pixelPos.y() >= 0 && pixelPos.y() < height;
}


// 获取格子状态
TileState Map::getTileState(int gridX, int gridY) const {
    if (!isGridInsideMap(gridX, gridY)) {
        return TILE_BLOCKED;  // 越界视为障碍物
    }
    return static_cast<TileState>(tileState[gridX][gridY]);
}

TileLayer Map::getTileLayer(int gridX, int gridY) const {
    if (!isGridInsideMap(gridX, gridY)) {
        return TILE_FIRST;  // 越界视为最高层
    }
    return static_cast<TileLayer>(tileLayer[gridX][gridY]);
}

// 各种状态判断的便捷函数
bool Map::isBuildable(int gridX, int gridY) const {
    TileState state = getTileState(gridX, gridY);
    return state == TILE_BUILDABLE;
}

bool Map::isBlocked(int gridX, int gridY) const {
    return getTileState(gridX, gridY) == TILE_BLOCKED;
}

bool Map::isLadder(int gridX, int gridY) const {
    return getTileState(gridX, gridY) == TILE_LADDER;
}

// 检查玩家是否可以移动到目标格子
bool Map::canMoveTo(int fromX, int fromY, int toX, int toY) const {
    if (!isGridInsideMap(toX, toY)) {
        return false;
    }

    TileLayer toLayer = getTileLayer(toX, toY);
    TileLayer fromLayer = getTileLayer(fromX, fromY);

    TileState toState = getTileState(toX, toY);
    TileState fromState = getTileState(fromX, fromY);

    // 检查目标格子是否可通行（排除障碍物、水池等）
    if (toState == TILE_BLOCKED ) {
        return false;
    }

    // 如果不同层，需要梯子
    if (toLayer != fromLayer) {
        // 检查当前位置或目标位置是否有梯子
        bool hasLadder = (fromState == TILE_LADDER) || (toState == TILE_LADDER);
        if (!hasLadder) {
            return false;
        }
    }
    return true;
}

// 设置起点
void Map::setStartPoint(int gridX, int gridY) {
    if (isGridInsideMap(gridX, gridY)) {
        startPoint = QPoint(gridX, gridY);
        setTileState(gridX, gridY, TILE_START);
    }
}

// 设置终点
void Map::setEndPoint(int gridX, int gridY) {
    if (isGridInsideMap(gridX, gridY)) {
        endPoint = QPoint(gridX, gridY);
        setTileState(gridX, gridY, TILE_END);
    }
}

void Map::initpic()
{
    background.load(QString(":/picture/map/map.png"));
}

QRectF Map::boundingRect() const
{
    return QRectF(0, 0, width, height);
}

void Map::paint(QPainter *p, const QStyleOptionGraphicsItem* , QWidget* )
{
    // 绘制背景
    p->drawPixmap(0, 0, background);

    // 如果开启网格显示，绘制格子边界和类型（这个超级棒在gameWidght.cpp里 47行 map[0]->setShowGrid(true);)
    if (showGrid) {
        p->setPen(QPen(Qt::gray, 1));

        // 绘制格子边界
        for (int y = 0; y <= heightNum; ++y) {
            p->drawLine(0, y * tileSize, width, y * tileSize);
        }
        for (int x = 0; x <= widthNum; ++x) {
            p->drawLine(x * tileSize, 0, x * tileSize, height);
        }

        // 绘制格子类型（颜色标识）
        for (int y = 0; y < heightNum; ++y) {
            for (int x = 0; x < widthNum; ++x) {
                TileState state = getTileState(y, x);
                QColor color;

                switch (state) {
                case TILE_BLOCKED:  // 1
                    color = QColor(255, 0, 0, 100);      // 红色（障碍）
                    break;

                case TILE_BUILDABLE: // 3
                    color = QColor(34, 139, 34, 50);     // 绿色（可建造）
                    break;

                case TILE_LADDER: // 4
                    color = QColor(255, 255, 0, 150);    // 黄色（梯子/台阶）
                    break;

                case TILE_START: // 5
                    color = QColor(0, 255, 0, 150);      // 亮绿色（起点）
                    break;

                case TILE_END: // 6
                    color = QColor(255, 0, 0, 150);      // 亮红色（终点）
                    break;

                default: // 普通格子 = 0
                    color = QColor(200, 200, 200, 50);   // 灰色
                    break;
                }

                p->fillRect(x * tileSize, y * tileSize, tileSize, tileSize, color);

                // 为特殊格子添加文字标识
                if (state == TILE_START) {
                    p->setPen(QPen(Qt::white, 2));
                    p->drawText(x * tileSize, y * tileSize, tileSize, tileSize,
                                Qt::AlignCenter, "S");
                } else if (state == TILE_END) {
                    p->setPen(QPen(Qt::white, 2));
                    p->drawText(x * tileSize, y * tileSize, tileSize, tileSize,
                                Qt::AlignCenter, "E");
                } else if (state == TILE_LADDER) {
                    p->setPen(QPen(Qt::black, 2));
                    p->drawText(x * tileSize, y * tileSize, tileSize, tileSize,
                                Qt::AlignCenter, "L");
                }
            }
        }
    }
}

// 设置是否显示网格
void Map::setShowGrid(bool show) {
    showGrid = show;
    update();  // 请求重绘
}

// 检查是否可以放置塔
bool Map::canPlaceTower(const QPointF& pixelPos) const {
    QPoint gridPos = pixelToGrid(pixelPos);
    // 塔只能放在平地上
    return isBuildable(gridPos.x(), gridPos.y());
}

// 放置塔
bool Map::placeTower(const QPointF& pixelPos) {
    QPoint gridPos = pixelToGrid(pixelPos);
    if (isBuildable(gridPos.x(), gridPos.y())) {
        setTileState(gridPos.x(), gridPos.y(), TILE_BLOCKED);
        return true;
    }
    return false;
}

// 移除塔
bool Map::removeTower(const QPointF& pixelPos) {
    QPoint gridPos = pixelToGrid(pixelPos);
    if (getTileState(gridPos.x(), gridPos.y()) == TILE_BLOCKED) {
        setTileState(gridPos.x(), gridPos.y(), TILE_BUILDABLE);
        return true;
    }
    return false;
}


Map::~Map() {
    // 清理tileData
    for (auto& row : tileState) {
        row.clear();
    }
    tileState.clear();

    for (auto& row : tileLayer) {
        row.clear();
    }
    tileLayer.clear();
}
