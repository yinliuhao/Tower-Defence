#pragma once
#include <vector>
#include <QVector>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QObject>
#include "structVector.h"

// 前向声明
class Map;

enum class MonsterType {
    MONSTER1,  ///< 怪物类型1
    MONSTER2,  ///< 怪物类型2
    MONSTER3,  ///< 怪物类型3
    MONSTER4   ///< 怪物类型4
};

enum class MonsterState{
    MOVING,
    ATTACKING,
    DEAD
};

//怪物基类（纯虚函数，要有实例）
class Monster : public QObject, public QGraphicsPixmapItem {//public QObject, public QGraphicsPixmapItem用于显示
    Q_OBJECT

public:
    // 修改构造函数，不需要传入路径，但需要传入Map指针
    Monster(MonsterType type,       // 怪物类型
            float monsterHealth,    // 怪物初始生命值
            float monsterSpeed,     // 怪物移动速度
            int monsterGold,        // 怪物被击败时奖励的金币
            float attackDamage,     // 攻击伤害
            QGraphicsItem* parent = nullptr);

    virtual ~Monster();

    void initializeMonster();       // 初始化函数

    // 基础功能接口
    void startMove();               // 开始怪物移动和动画
    void stopMove();                // 停止怪物移动和动画

    bool isDead() const { return monsterHealth <= 0; }  // 判断怪物是否死亡
    void takeDamage(float damage);  // 怪物受到伤害

    //攻击相关
    void startAttack();
    void stopAttack();
    void performAttack(); // 执行攻击

    //获取器
    Vector2 getPosition() const { return monsterPosition; }  // 获取当前位置
    int getGold() const { return monsterGold; }  // 获取金币奖励
    float getHealth() const { return monsterHealth; }  // 获取当前生命值
    float getAttackDamage()const {return attackDamage;} //获取怪物伤害
    MonsterState getMonsterState()const {return currentState;} //获取当前状态
    bool isAttackingCamp()const {return isAttacking;} //是否攻击

    // 获取怪物标准尺寸
    static QSize getMonsterSize() { return QSize(32, 32); }//一个格子的大小

signals:  //Qt的内容
    // 信号：怪物到达终点
    void reachedDestination();
    // 信号：怪物开始攻击营地
    void startedAttackingCamp();
    // 信号：怪物停止攻击营地
    void stoppedAttackingCamp();
    // 信号：攻击营地，传递伤害值
    void attackedCamp(int damage);
    // 信号：怪物死亡
    void died(int goldReward);

public slots:  //Qt的内容
    void updateAnimationFrame();    // 更新动画帧
    void moveToNextPosition();      // 移动到下一个路径点

protected:
    // 纯虚函数：加载动画帧
    virtual void loadAnimationFrames()=0;
    virtual void loadAttackFrames()=0;

    void initializeTimers();        // 初始化动画和移动计时器

    // 怪物基础属性
    MonsterType type;               // 怪物类型标识
    MonsterState currentState;      // 怪物当前状态
    float monsterHealth;            // 当前生命值
    float monsterSpeed;             // 移动速度（像素/秒）
    int monsterGold;                // 击败奖励金币

    // 移动路径相关
    Vector2 monsterPosition;        // 当前精确位置（浮点数坐标）
    std::vector<Vector2> path;      // 移动路径点序列（从Map获取）
    int currentTargetIndex;         // 当前目标路径点索引
    bool reachedEnd;                // 是否已到达路径终点

    //攻击相关
    float attackDamage;
    bool isAttacking;


    // 动画系统相关
    QVector<QPixmap> animationFrames;  // 移动动画帧图片集合
    QVector<QPixmap>attackFrames;      // 攻击动画帧集合
    int currentFrameIndex;             // 当前动画帧索引
    bool isAttackAnimation;            // 是否播放攻击动画


private:
    QTimer* animationTimer;    // 动画更新计时器
    QTimer* moveTimer;         // 位置移动计时器
    QTimer* attackTimer;       // 攻击计时器
};

class Monster1:public Monster{
    Q_OBJECT
public:
    Monster1(QGraphicsItem* parent = nullptr);
protected:
    virtual void loadAnimationFrames()override;
    virtual void loadAttackFrames()override;
    };

class Monster2:public Monster{
    Q_OBJECT
public:
    Monster2(QGraphicsItem* parent = nullptr);
protected:
    virtual void loadAnimationFrames()override;
     virtual void loadAttackFrames()override;
};

class Monster3:public Monster{
    Q_OBJECT
public:
    Monster3(QGraphicsItem* parent = nullptr);
protected:
    virtual void loadAnimationFrames()override;
     virtual void loadAttackFrames()override;
};


