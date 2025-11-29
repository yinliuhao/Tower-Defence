#pragma once
#include <vector>
#include <QVector>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QObject>
#include "structVector.h"

enum class MonsterType {
    MONSTER1,  ///< 怪物类型1
    MONSTER2,  ///< 怪物类型2
    MONSTER3,  ///< 怪物类型3
    MONSTER4   ///< 怪物类型4
};

// 前向声明
class Map;
class Bullet;

//怪物基类
class Monster : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:

    Monster(std::vector<Vector2> path,//怪物移动路径点集合
            MonsterType type,//怪物类型
            float monsterHealth,//怪物初始生命值
            float monsterSpeed,//怪物移动速度
            int monsterGold,//怪物被击败时奖励的金币
            QGraphicsItem* parent = nullptr);//父对象指针

    virtual ~Monster();

    void initializeMonster();//初始化函数

    // 基础功能接口
    void startMove();       // 开始怪物移动和动画
    void stopMove();        ///停止怪物移动和动画
    bool isDead() const { return monsterHealth <= 0; }  // 判断怪物是否死亡
    void takeDamage(float damage);  //怪物受到伤害
    Vector2 getPosition() const { return monsterPosition; }  // 获取当前位置
    int getGold() const { return monsterGold; }  //获取金币奖励
    float getHealth() const { return monsterHealth; }  // 获取当前生命值

    //获取怪物标准尺寸,返回32x32的固定尺寸
    static QSize getMonsterSize() { return QSize(32, 32); }

signals:
    //信号：怪物到达终点,当怪物成功沿着路径移动到终点时发射
    void reachedDestination();

    //信号：怪物死亡   goldReward 死亡时奖励的金币数,当怪物生命值归零时发射
    void died(int goldReward);

public slots:
    void updateAnimationFrame();    // 更新动画帧
    void moveToNextPosition();      // 移动到下一个路径点

protected:
    //纯虚函数：加载动画帧, 由具体怪物子类实现，加载对应的动画图片资源
    virtual void loadAnimationFrames() = 0;

    void initializeTimers();        // 初始化动画和移动计时器

    // 怪物基础属性
    MonsterType type;               // 怪物类型标识
    float monsterHealth;            // 当前生命值
    float monsterSpeed;             // 移动速度（像素/秒）
    int monsterGold;                // 击败奖励金币

    // 移动路径相关
    Vector2 monsterPosition;        // 当前精确位置（浮点数坐标）
    std::vector<Vector2> path;      // 移动路径点序列
    int currentTargetIndex;         // 当前目标路径点索引
    bool reachedEnd;                // 是否已到达路径终点

    // 动画系统相关
    QVector<QPixmap> animationFrames;  // 动画帧图片集合
    int currentFrameIndex;             // 当前动画帧索引

private:
    QTimer* animationTimer;    // 动画更新计时器
    QTimer* moveTimer;         // 位置移动计时器
};

// 第一种怪物类型的具体实现
class Monster1 : public Monster {
public:
    //构造函数
    // path 移动路径
    // parent 父对象指针
    Monster1(std::vector<Vector2> path, QGraphicsItem* parent = nullptr);

protected:
    // 实现基类的纯虚函数，加载怪物1的动画资源
    void loadAnimationFrames() override;
};
