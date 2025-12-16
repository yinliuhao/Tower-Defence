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
    monsterPath = {QPointF(4, 6),
                   QPointF(14, 6),
                   QPointF(14, 21),
                   QPointF(9, 21),
                   QPointF(9, 32), // 5
                   QPointF(20, 32),
                   QPointF(20, 22),
                   QPointF(27, 22),
                   QPointF(27, 8),
                   QPointF(23, 8), //10
                   QPointF(23, 31),
                   QPointF(31, 31),
                   QPointF(31, 40),
                   QPointF(23, 40),
                   QPointF(23, 46), //15
                   QPointF(20, 46),
                   QPointF(17, 46),
                   QPointF(17, 47),
                   QPointF(7, 47)};

    monsterHome.resize(4);
    monsterHome = {QPoint(3, 5),
                   QPoint(5, 54),
                   QPoint(11, 76),
                   QPoint(54, 84)};

    playerHome.resize(1);
    playerHome = {QPoint(6, 47)};

    grassStone.resize(6);
    grassStone = {QPoint(7, 25),
                  QPoint(15, 21),
                  QPoint(19, 5),
                  QPoint(18, 39),
                  QPoint(18, 52),
                  QPoint(34, 29)};

    swampStone.resize(5);
    swampStone = {QPoint(38, 12),
                  QPoint(46, 8),
                  QPoint(38, 31),
                  QPoint(44, 23),
                  QPoint(55, 50)};

    snowStone.resize(4);
    snowStone = {QPoint(5, 60),
                 QPoint(6, 86),
                 QPoint(13, 70),
                 QPoint(21, 73)};

    grassTree.resize(26);
    grassTree = {QPoint(7, 3),
                 QPoint(9, 5),
                 QPoint(12, 3),
                 QPoint(19, 3),
                 QPoint(30, 3), // 5
                 QPoint(26, 5),
                 QPoint(30, 8),
                 QPoint(15, 10),
                 QPoint(32, 15),
                 QPoint(31, 17), // 10
                 QPoint(17, 19),
                 QPoint(18, 20),
                 QPoint(17, 22),
                 QPoint(3, 18),
                 QPoint(3, 28), // 15
                 QPoint(2, 32),
                 QPoint(21, 27),
                 QPoint(20, 29),
                 QPoint(23, 30),
                 QPoint(32, 29), // 20
                 QPoint(16, 33),
                 QPoint(7, 40),
                 QPoint(16, 38),
                 QPoint(20, 40),
                 QPoint(26, 39), // 25
                 QPoint(32, 38)};

    swampTree.resize(19);
    swampTree = {QPoint(37, 7),
                 QPoint(45, 1),
                 QPoint(50, 3),
                 QPoint(57, 2),
                 QPoint(41, 10), // 5
                 QPoint(54, 9),
                 QPoint(40, 16),
                 QPoint(47, 14),
                 QPoint(54, 16),
                 QPoint(57, 16), // 10
                 QPoint(46, 23),
                 QPoint(40, 30),
                 QPoint(54, 33),
                 QPoint(48, 38),
                 QPoint(37, 43), // 15
                 QPoint(42, 45),
                 QPoint(55, 46),
                 QPoint(44, 50),
                 QPoint(56, 55)};
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
    background.load(QString(":/picture/map/map0.png"));
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
bool Map::canPlaceTower(const QPoint& gridPos) const {
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
