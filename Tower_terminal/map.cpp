#include "map.h"
#include <QString>
#include <QPainter>
#include <QPen>
#include <QColor>
#include <algorithm>
#include <QDebug>
#include <cmath>
#include "utils.h"

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

// 坐标转换：像素坐标 -> 格子坐标
QPoint Map::pixelToGrid(const QPointF& pixelPos) const {
    int gridX = static_cast<int>(pixelPos.x()) / tileSize;
    int gridY = static_cast<int>(pixelPos.y()) / tileSize;

    // 确保在有效范围内
    gridX = qMax(0, qMin(gridX, widthNum - 1));
    gridY = qMax(0, qMin(gridY, heightNum - 1));

    return QPoint(gridX, gridY);
}

// 坐标转换：格子坐标 -> 像素坐标（格子中心）
QPointF Map::gridToPixel(const QPoint& gridPos) const {
    return gridToPixel(gridPos.x(), gridPos.y());
}

QPointF Map::gridToPixel(int gridX, int gridY) const {
    float pixelX = gridX * tileSize + tileSize / 2.0f;
    float pixelY = gridY * tileSize + tileSize / 2.0f;
    return QPointF(pixelX, pixelY);
}

// 检查格子是否在有效范围内
bool Map::isGridInsideMap(int gridX, int gridY) const {
    return gridX >= 0 && gridX < widthNum && gridY >= 0 && gridY < heightNum;
}

// 获取格子状态
TileState Map::getTileState(int gridX, int gridY) const {
    if (!isGridInsideMap(gridX, gridY)) {
        return TILE_BLOCKED;  // 越界视为障碍物
    }
    return static_cast<TileState>(tileData[gridY][gridX]);
}

// 获取格子所在层
TileLayer Map::getTileLayer(int gridX, int gridY) const {
    TileState state = getTileState(gridX, gridY);
    switch (state) {
    case TILE_WATER:
        return LAYER_WATER;
    case TILE_MOUNTAIN:
        return LAYER_MOUNTAIN;
    case TILE_LADDER:
    case TILE_BUILDABLE:
    case TILE_WALKABLE:
    case TILE_START:
    case TILE_END:
    default:
        return LAYER_GROUND;
    }
}

// 设置格子状态
void Map::setTileState(int gridX, int gridY, TileState state) {
    if (isGridInsideMap(gridX, gridY)) {
        tileData[gridY][gridX] = static_cast<int>(state);
        update();  // 请求重绘
    }
}

// 获取地形高度（用于高度差计算）
int Map::getTerrainHeight(int x, int y) const {
    TileLayer layer = getTileLayer(x, y);
    switch (layer) {
    case LAYER_WATER: return 0;    // 水池：最低层
    case LAYER_GROUND: return 1;   // 平地：中间层
    case LAYER_MOUNTAIN: return 2; // 山地：最高层
    default: return 1;
    }
}

// 检查高度过渡是否有效
bool Map::isHeightTransitionValid(int fromHeight, int toHeight) const {
    int diff = toHeight - fromHeight;
    // 可以向下走（从高到低）
    if (diff <= 0) return true;
    // 向上走需要梯子，但这里我们只检查高度差，梯子检查在canMoveTo中
    return diff <= 1;  // 允许向上走一层（如果有梯子）
}

// 各种状态判断的便捷函数
bool Map::isWalkable(int gridX, int gridY) const {
    TileState state = getTileState(gridX, gridY);
    return state == TILE_WALKABLE || state == TILE_START || state == TILE_END ||
           state == TILE_LADDER;  // 梯子也是可通行的
}

bool Map::isBuildable(int gridX, int gridY) const {
    TileState state = getTileState(gridX, gridY);
    // 平地可以建造，其他都不行
    return state == TILE_BUILDABLE;
}

bool Map::isBlocked(int gridX, int gridY) const {
    return getTileState(gridX, gridY) == TILE_BLOCKED;
}

bool Map::isWater(int gridX, int gridY) const {
    return getTileState(gridX, gridY) == TILE_WATER;
}

bool Map::isMountain(int gridX, int gridY) const {
    return getTileState(gridX, gridY) == TILE_MOUNTAIN;
}

bool Map::isLadder(int gridX, int gridY) const {
    return getTileState(gridX, gridY) == TILE_LADDER;
}

// 检查像素坐标是否在地图内
bool Map::isInsideMap(const QPointF& pixelPos) const {
    return pixelPos.x() >= 0 && pixelPos.x() < width &&
           pixelPos.y() >= 0 && pixelPos.y() < height;
}

// 检查玩家是否可以移动到目标格子
bool Map::canMoveTo(int fromX, int fromY, int toX, int toY) const {
    if (!isGridInsideMap(toX, toY) || !isGridInsideMap(fromX, fromY)) {
        return false;
    }

    TileState toState = getTileState(toX, toY);
    TileState fromState = getTileState(fromX, fromY);

    // 检查目标格子是否可通行（排除障碍物、水池等）
    if (toState == TILE_BLOCKED ) {
        return false;
    }

    // 检查高度差
    int fromHeight = getTerrainHeight(fromX, fromY);
    int toHeight = getTerrainHeight(toX, toY);

    // 如果从低到高（向上走），需要梯子
    if (toHeight > fromHeight) {
        // 检查当前位置或目标位置是否有梯子
        bool hasLadder = (fromState == TILE_LADDER) || (toState == TILE_LADDER);
        if (!hasLadder) {
            return false;
        }
    }

    return true;
}

// 获取高度差
int Map::getHeightDifference(int x1, int y1, int x2, int y2) const {
    if (!isGridInsideMap(x1, y1) || !isGridInsideMap(x2, y2)) {
        return 0;
    }
    return getTerrainHeight(x2, y2) - getTerrainHeight(x1, y1);
}

void Map::initializeTileData()
{
    // 调整外层 vector 的大小（行数 = heightNum）
    tileData.resize(heightNum);

    // 调整内层 vector 的大小（列数 = widthNum）并初始化值为可建造区域（平地）
    for (int y = 0; y < heightNum; ++y) {
        tileData[y].resize(widthNum);
        for (int x = 0; x < widthNum; ++x) {
            // 默认都是平地（可建造）
            tileData[y][x] = TILE_BUILDABLE;
        }
    }

    // 目前测试部分区域，之后考虑用图片生成一个地图数据文件，用输入流实现
    // 根据提供的数据设置特殊区域

    // 1. 设置水池区域：(5,21) 到 (24,31) 的矩形区域
    for (int x = 5; x <= 24; ++x) {
        for (int y = 21; y <= 31; ++y) {
            if (isGridInsideMap(x, y)) {
                tileData[y][x] = TILE_WATER;
            }
        }
    }

    // 2. 设置梯子：(15,21)
    if (isGridInsideMap(15, 21)) {
        tileData[21][15] = TILE_LADDER;
    }

    // 3. 设置障碍物区域：(34,21) 到 (36,28) 的矩形区域
    for (int x = 34; x <= 36; ++x) {
        for (int y = 21; y <= 28; ++y) {
            if (isGridInsideMap(x, y)) {
                tileData[y][x] = TILE_BLOCKED;
            }
        }
    }

    // 4. 设置一些山地（最高层）作为示例
    // 假设 (19,5) 到 (37,16) 是山地

    for (int x = 19; x <= 37; ++x) {
        for (int y = 5; y <= 16; ++y) {
            if (isGridInsideMap(x, y)) {
                tileData[y][x] = TILE_MOUNTAIN;
            }
        }
    }

    // 5. 创建怪物路径（避开障碍物和水池）
    // 从左边到右边的简单路径

    int pathX = 46;  // 路径所在的Y坐标

    // 水平路径
    for (int y = 7; y <= 44; ++y) {
        if (isGridInsideMap(pathX, y)) {
            // 确保路径不经过障碍物和水池
            if (getTileState(pathX, y) == TILE_BUILDABLE) {
                tileData[y][pathX] = TILE_WALKABLE;
            }
        }
    }

    // 6. 设置起点和终点
    // 起点在左边，终点在右边
    setStartPoint(pathX, 44);
    setEndPoint(pathX, 7);

    // 7. 生成路径
    generateMonsterPath();
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

// 生成怪物路径
void Map::generateMonsterPath() {
    // 将 QPointF 转换为 Vector2
    QPointF startPixel = gridToPixel(startPoint.x(), startPoint.y());
    QPointF endPixel = gridToPixel(endPoint.x(), endPoint.y());

    Vector2 start(startPixel.x(), startPixel.y());
    Vector2 end(endPixel.x(), endPixel.y());

    monsterPath = findPath(start, end);

    // 如果路径查找失败，创建一条简单路径
    if (monsterPath.empty()) {
        qDebug() << "路径查找失败，使用简单路径";
        monsterPath.push_back(start);

        // 创建一条简单的直线路径
        int steps = 50;
        for (int i = 1; i <= steps; ++i) {
            float t = static_cast<float>(i) / steps;
            float x = start.x + (end.x - start.x) * t;
            float y = start.y + (end.y - start.y) * t;
            monsterPath.push_back(Vector2(x, y));
        }
        monsterPath.push_back(end);
    }

    // 输出路径信息（用于调试）
    qDebug() << "生成的怪物路径有" << monsterPath.size() << "个点";
    if (!monsterPath.empty()) {
        qDebug() << "起点: (" << monsterPath.front().x << "," << monsterPath.front().y << ")";
        qDebug() << "终点: (" << monsterPath.back().x << "," << monsterPath.back().y << ")";
    }
}

// 获取怪物路径
std::vector<Vector2> Map::getMonsterPath() const {
    return monsterPath;
}

void Map::initpic()
{
    background.load(QString(":/picture/map/map.png"));
}

QRectF Map::boundingRect() const
{
    return QRectF(0, 0, width, height);
}

void Map::paint(QPainter *p, const QStyleOptionGraphicsItem* option, QWidget* widget)
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
                TileState state = getTileState(x, y);
                QColor color;

                switch (state) {
                case TILE_WALKABLE:
                    color = QColor(139, 69, 19, 100);  // 棕色，半透明（路径）
                    break;
                case TILE_BUILDABLE:
                    color = QColor(34, 139, 34, 50);   // 绿色，半透明（平地）
                    break;
                case TILE_BLOCKED:
                    color = QColor(255, 0, 0, 100);    // 红色，半透明（障碍物）
                    break;
                case TILE_START:
                    color = QColor(0, 255, 0, 150);    // 亮绿色（起点）
                    break;
                case TILE_END:
                    color = QColor(255, 0, 0, 150);    // 亮红色（终点）
                    break;
                case TILE_WATER:
                    color = QColor(30, 144, 255, 100); // 蓝色，半透明（水池）
                    break;
                case TILE_MOUNTAIN:
                    color = QColor(139, 69, 19, 150);  // 深棕色，半透明（山地）
                    break;
                case TILE_LADDER:
                    color = QColor(255, 255, 0, 150);  // 黄色，半透明（梯子）
                    break;
                default:
                    color = QColor(200, 200, 200, 50); // 灰色
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

// A*算法启发式函数（使用曼哈顿距离）
float Map::calculateHeuristic(int x1, int y1, int x2, int y2) const {
    // 曼哈顿距离
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

// 获取相邻格子（怪物移动用）
std::vector<QPoint> Map::getNeighbors(int x, int y) const {
    std::vector<QPoint> neighbors;

    // 4方向移动：上、下、左、右
    int dx[] = {0, 1, 0, -1};
    int dy[] = {-1, 0, 1, 0};

    for (int i = 0; i < 4; ++i) {
        int nx = x + dx[i];
        int ny = y + dy[i];

        // 检查是否在网格内且可通行
        if (isGridInsideMap(nx, ny) && isWalkable(nx, ny)) {
            neighbors.push_back(QPoint(nx, ny));
        }
    }

    return neighbors;
}

// 重构路径
std::vector<Vector2> Map::reconstructPath(AStarNode* endNode) const {
    std::vector<Vector2> path;

    // 从终点回溯到起点
    AStarNode* current = endNode;
    while (current != nullptr) {
        // 将 QPointF 转换为 Vector2
        QPointF pixelPos = gridToPixel(current->x, current->y);
        path.push_back(Vector2(pixelPos.x(), pixelPos.y()));
        current = current->parent;
    }

    // 反转路径（起点到终点）
    std::reverse(path.begin(), path.end());

    return path;
}

// A*路径查找算法
std::vector<Vector2> Map::findPath(const Vector2& start, const Vector2& end) const {
    // 转换像素坐标为格子坐标
    QPoint startGrid = pixelToGrid(QPointF(start.x, start.y));
    QPoint endGrid = pixelToGrid(QPointF(end.x, end.y));

    // 检查起点和终点是否可通行
    if (!isWalkable(startGrid.x(), startGrid.y()) ||
        !isWalkable(endGrid.x(), endGrid.y())) {
        qDebug() << "起点或终点不可通行";
        return std::vector<Vector2>();
    }

    // 如果起点和终点相同，返回空路径
    if (startGrid == endGrid) {
        return std::vector<Vector2>();
    }

    // 开放列表（待检查节点）和关闭列表（已检查节点）
    std::vector<AStarNode*> openList;
    std::vector<AStarNode*> closedList;

    // 创建起点节点
    AStarNode* startNode = new AStarNode{startGrid.x(), startGrid.y(), 0,
                                         calculateHeuristic(startGrid.x(), startGrid.y(),
                                                            endGrid.x(), endGrid.y()),
                                         nullptr};
    openList.push_back(startNode);

    while (!openList.empty()) {
        // 查找开放列表中fCost最小的节点
        auto minIt = std::min_element(openList.begin(), openList.end(),
                                      [](AStarNode* a, AStarNode* b) { return a->fCost() < b->fCost(); });
        AStarNode* current = *minIt;

        // 如果当前节点是终点，重构路径并返回
        if (current->x == endGrid.x() && current->y == endGrid.y()) {
            std::vector<Vector2> path = reconstructPath(current);

            // 清理内存
            for (AStarNode* node : openList) delete node;
            for (AStarNode* node : closedList) delete node;

            return path;
        }

        // 将当前节点移到关闭列表
        openList.erase(minIt);
        closedList.push_back(current);

        // 检查相邻节点
        std::vector<QPoint> neighbors = getNeighbors(current->x, current->y);
        for (const QPoint& neighbor : neighbors) {
            // 检查是否在关闭列表中
            bool inClosedList = false;
            for (AStarNode* node : closedList) {
                if (node->x == neighbor.x() && node->y == neighbor.y()) {
                    inClosedList = true;
                    break;
                }
            }
            if (inClosedList) continue;

            // 计算相邻节点的gCost
            float newGCost = current->gCost + 1.0f;  // 每个格子的代价为1

            // 检查是否在开放列表中
            AStarNode* neighborNode = nullptr;
            for (AStarNode* node : openList) {
                if (node->x == neighbor.x() && node->y == neighbor.y()) {
                    neighborNode = node;
                    break;
                }
            }

            if (neighborNode == nullptr) {
                // 不在开放列表中，创建新节点
                neighborNode = new AStarNode{neighbor.x(), neighbor.y(), newGCost,
                                             calculateHeuristic(neighbor.x(), neighbor.y(),
                                                                endGrid.x(), endGrid.y()),
                                             current};
                openList.push_back(neighborNode);
            } else if (newGCost < neighborNode->gCost) {
                // 已在开放列表中，但新路径更好
                neighborNode->gCost = newGCost;
                neighborNode->parent = current;
            }
        }
    }

    // 没有找到路径
    qDebug() << "未找到路径";

    // 清理内存
    for (AStarNode* node : openList) delete node;
    for (AStarNode* node : closedList) delete node;

    return std::vector<Vector2>();
}

Map::~Map() {
    // 清理tileData
    for (auto& row : tileData) {
        row.clear();
    }
    tileData.clear();

    // 清理monsterPath
    monsterPath.clear();
}
