#pragma once
#include <QObject>
#include <QTimer>
#include <QVector>
#include <QGraphicsScene>
#include "monster.h"

// 怪物管理器
// 专门负责怪物的生成和生命周期管理
class MonsterManager : public QObject
{
    Q_OBJECT

public:
    /**
     * 构造函数
     *  scene 游戏场景，用于添加怪物显示
     *  parent 父对象
     */
    explicit MonsterManager(QGraphicsScene* scene, QObject* parent = nullptr);
    // 析构函数
    ~MonsterManager();

    /**
     * 开始生成怪物
     *  delayMs 首次生成延迟（毫秒）
     *  intervalMs 生成间隔（毫秒）
     */
    void startSpawning(int delayMs = 3000, int intervalMs = 5000);

    // 停止生成怪物
    void stopSpawning();

    // 清理所有怪物
    void clearAllMonsters();

    /**
     *  设置怪物移动路径
     *  path 路径点序列
     */
    void setMonsterPath(const std::vector<Vector2>& path);

    // 获取当前活跃怪物数量
    int getActiveMonsterCount() const { return m_activeMonsters.size(); }

private slots:
    // 生成一个怪物
    void spawnMonster();

    // 处理怪物死亡
    void handleMonsterDeath();

    //处理怪物到达终点
    void handleMonsterReachedEnd();

    // 清理死亡怪物
    void cleanupDeadMonsters();

private:
    //创建默认路径,返回预设的路径点

    std::vector<Vector2> createDefaultPath();

    /**
     *  从管理列表中移除怪物
     *  monster 要移除的怪物
     */
    void removeMonster(Monster* monster);
    QGraphicsScene* m_scene;                   // 游戏场景指针
    QTimer* m_spawnTimer;                     // 生成计时器
    QTimer* m_cleanupTimer;                   // 清理计时器

    QVector<Monster*> m_activeMonsters;       // 活跃怪物列表
    QVector<Monster*> m_deadMonsters;         // 待清理的死亡怪物

    std::vector<Vector2> m_monsterPath;       // 怪物移动路径
    bool m_isSpawning;                        // 是否正在生成怪物
};
