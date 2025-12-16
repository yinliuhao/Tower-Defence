#include "monstermanager.h"
#include <QDebug>
#include <fstream>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDir>


// 构造函数
MonsterManager::MonsterManager(QGraphicsScene* scene, QObject* parent)
    : QObject(parent)
    , m_scene(scene)
    , m_spawnTimer(nullptr)
    , m_cleanupTimer(nullptr)
    , m_isSpawning(false)
{
    // 设置默认路径
    m_monsterPath = createDefaultPath();

    // 初始化计时器
    m_spawnTimer = new QTimer(this);
    connect(m_spawnTimer, &QTimer::timeout, this, &MonsterManager::spawnMonster);

    // 清理计时器：每3秒清理一次死亡怪物
    m_cleanupTimer = new QTimer(this);
    connect(m_cleanupTimer, &QTimer::timeout, this, &MonsterManager::cleanupDeadMonsters);
    m_cleanupTimer->start(3000);

}

 // 析构函数
MonsterManager::~MonsterManager()
{
    stopSpawning();
    clearAllMonsters();

    qDebug() << "怪物管理器已销毁";
}


 // 创建默认路径(测试用，后根据map类修改)
std::vector<Vector2> MonsterManager::createDefaultPath()
{
    std::vector<Vector2> path;

    path.push_back(Vector2(0, 200));    // 起点：屏幕左侧外
    path.push_back(Vector2(100, 200));    // 进入屏幕
    path.push_back(Vector2(300, 200));    // 向右移动
    path.push_back(Vector2(500, 200));    // 继续向右
    path.push_back(Vector2(700, 200));    // 接近右侧
    path.push_back(Vector2(1000, 200));    // 终点：屏幕右侧外

    return path;
}


// 设置怪物移动路径
void MonsterManager::setMonsterPath(const std::vector<Vector2>& path)
{
    if (path.size() >= 2) {
        m_monsterPath = path;
        qDebug() << "怪物路径已更新，路径点数量:" << m_monsterPath.size();
    } else {
        qWarning() << "路径设置失败：至少需要2个路径点";
    }
}

//
void MonsterManager::startSpawning(int delayMs, int intervalMs)
{
    if (m_isSpawning) {
        qDebug() << "怪物生成已在运行中";
        return;
    }

    if (m_monsterPath.size() < 2) {
        qWarning() << "无法开始生成：路径点不足";
        return;
    }

    m_spawnTimer->setInterval(intervalMs);//开始生成计时器
    m_isSpawning = true;

    qDebug() << "开始生成怪物，延迟:" << delayMs << "ms, 间隔:" << intervalMs << "ms";

    // 延迟后开始生成
    QTimer::singleShot(delayMs, this, [this]() {
        if (m_isSpawning) {
            m_spawnTimer->start();
        }
    });
}

//停止生成怪物
void MonsterManager::stopSpawning()
{
    if (m_spawnTimer->isActive()) {
        m_spawnTimer->stop();
    }
    m_isSpawning = false;

    qDebug() << "怪物生成已停止";
}


// 生成一个怪物
void MonsterManager::spawnMonster()
{
    if (!m_scene) {
        qWarning() << "无法生成怪物：场景为空";
        return;
    }

    // 创建怪物实例
    Monster* monster = new Monster1(m_monsterPath);

    // 添加到场景
    m_scene->addItem(monster);
    m_activeMonsters.append(monster);

    // 连接信号
    connect(monster, &Monster::died, this, &MonsterManager::handleMonsterDeath);
    connect(monster, &Monster::reachedDestination, this, &MonsterManager::handleMonsterReachedEnd);

    // 启动怪物移动
    monster->startMove();

    qDebug() << "怪物已生成，当前数量:" << m_activeMonsters.size();
}


 // 处理怪物死亡

void MonsterManager::handleMonsterDeath()
{
    Monster* monster = qobject_cast<Monster*>(sender());
    if (monster) {
        qDebug() << "怪物死亡，开始清理";

        // #region agent log
        {
            try {
                QDir().mkpath(".cursor");
                QFile file(".cursor/debug.log");
                if (file.open(QIODevice::Append | QIODevice::Text)) {
                    QTextStream ts(&file);
                    ts << "{\"sessionId\":\"debug-session\","
                          "\"runId\":\"pre-fix\","
                          "\"hypothesisId\":\"H1\","
                          "\"location\":\"monstermanager.cpp:130\","
                          "\"message\":\"MonsterManager::handleMonsterDeath\","
                          "\"data\":{\"ptr\":\"" << monster
                       << "\"},"
                          "\"timestamp\":" << static_cast<long long>(QDateTime::currentMSecsSinceEpoch())
                       << "}\n";
                }
            } catch (...) {
            }
        }
        // #endregion

        m_deadMonsters.append(monster);
        removeMonster(monster);
    }
}

 //处理怪物到达终点
void MonsterManager::handleMonsterReachedEnd()
{
    Monster* monster = qobject_cast<Monster*>(sender());
    if (monster) {
        qDebug() << "怪物到达终点，开始清理";
        m_deadMonsters.append(monster);
        removeMonster(monster);
    }
}


 //清理死亡怪物
void MonsterManager::cleanupDeadMonsters()
{
    if (m_deadMonsters.isEmpty()) {
        return;
    }

    for (Monster* monster : m_deadMonsters) {
        if (monster) {
            // #region agent log
            {
                try {
                    QDir().mkpath(".cursor");
                    QFile file(".cursor/debug.log");
                    if (file.open(QIODevice::Append | QIODevice::Text)) {
                        QTextStream ts(&file);
                        ts << "{\"sessionId\":\"debug-session\","
                              "\"runId\":\"pre-fix\","
                              "\"hypothesisId\":\"H1\","
                              "\"location\":\"monstermanager.cpp:159\","
                              "\"message\":\"MonsterManager::cleanupDeadMonsters deleting monster\","
                              "\"data\":{\"ptr\":\"" << monster
                           << "\"},"
                              "\"timestamp\":" << static_cast<long long>(QDateTime::currentMSecsSinceEpoch())
                           << "}\n";
                    }
                } catch (...) {
                }
            }
            // #endregion

            if (m_scene) {
                m_scene->removeItem(monster);
            }
            delete monster;
        }
    }

    int cleanedCount = m_deadMonsters.size();
    m_deadMonsters.clear();

    qDebug() << "已清理" << cleanedCount << "个死亡怪物";
}


// 从活跃列表移除怪物

void MonsterManager::removeMonster(Monster* monster)
{
    if (!monster) return;

    // #region agent log
    {
        try {
            QDir().mkpath(".cursor");
            QFile file(".cursor/debug.log");
            if (file.open(QIODevice::Append | QIODevice::Text)) {
                QTextStream ts(&file);
                ts << "{\"sessionId\":\"debug-session\","
                      "\"runId\":\"pre-fix\","
                      "\"hypothesisId\":\"H1\","
                      "\"location\":\"monstermanager.cpp:177\","
                      "\"message\":\"MonsterManager::removeMonster\","
                      "\"data\":{\"ptr\":\"" << monster
                   << "\"},"
                      "\"timestamp\":" << static_cast<long long>(QDateTime::currentMSecsSinceEpoch())
                   << "}\n";
            }
        } catch (...) {
        }
    }
    // #endregion

    int index = m_activeMonsters.indexOf(monster);
    if (index != -1) {
        m_activeMonsters.removeAt(index);
        qDebug() << "从活跃列表移除怪物，剩余:" << m_activeMonsters.size();
    }
}


 //清理所有怪物

void MonsterManager::clearAllMonsters()
{
    qDebug() << "开始清理所有怪物，当前数量:" << m_activeMonsters.size();

    stopSpawning();

    // 停止所有怪物移动
    for (Monster* monster : m_activeMonsters) {
        if (monster) {
            monster->stopMove();
        }
    }

    // 清理活跃怪物
    for (Monster* monster : m_activeMonsters) {
        if (monster && m_scene) {
            m_scene->removeItem(monster);
        }
        delete monster;
    }
    m_activeMonsters.clear();

    // 清理死亡怪物
    cleanupDeadMonsters();

    qDebug() << "所有怪物清理完成";
}
