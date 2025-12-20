#pragma once

#include <vector>
#include <QVector>
#include <QTimer>
#include <QGraphicsPixmapItem>
#include <QObject>
#include "_others/structVector.h"
#include "_others/utils.h"

class Map;

/***********************************
 *          Monster Base
 ***********************************/
// 怪物基类（抽象类，但允许实例化子类）
class Monster : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    /************* ctor / dtor *************/
    explicit Monster(MonsterType type,
                     QGraphicsItem* parent = nullptr);
    virtual ~Monster();

    /************* basic *************/
    virtual void initializeMonster();   // 初始化怪物
    void startMove();                   // 开始移动 & 动画

    /************* takeDamage *************/
    void takeDamage(float damage);
    void showDamageEffect(float damage);

    /************* inflictDamage *************/
    virtual void startAttack();
    virtual void performAttack();

    /************* getters *************/
    Vector2      getPosition() const { return monsterPosition; }
    int          getGold()     const { return monsterGold; }
    float        getHealth()   const { return monsterHealth; }
    float        getAttackDamage() const { return attackDamage; }
    MonsterState getMonsterState() const { return currentState; }

    bool isDead() const { return currentState == MonsterState::DEAD; }
    bool isDying() const { return currentState == MonsterState::DYING; }
    bool isAttacking() const { return currentState == MonsterState::ATTACKING; }

    static QSize getMonsterSize() { return QSize(32, 32); }

signals:
    /************* signals *************/
    void reachedDestination();
    void startedAttackingCamp();
    void attackedCamp(int damage);   //已处理
    void attackedPlayer(int damage);   //已处理
    void died(MonsterType type);   //已处理

public slots:
    /************* slots *************/
    void updateAnimationFrame();
    virtual void moveToNextPosition();

private slots:
    /************* private slots *************/
    void onAttackTimerTimeout();  // 攻击定时器回调

protected:
    /************* render *************/
    void paint(QPainter* painter,
               const QStyleOptionGraphicsItem* option,
               QWidget* widget = nullptr) override;

    /************* death *************/
    void markDead();
    void onDeath();
    void playDeathAnimation();

    /************* resource loading *************/
    virtual void loadAnimationFrames() = 0;
    virtual void loadAttackFrames()    = 0;
    virtual void loadDeathFrames()     = 0;

    void initializeTimers();

protected:
    /************* base attributes *************/
    MonsterType  type;
    MonsterState currentState;

    float monsterHealth;
    float monsterSpeed;
    int   monsterGold;
    float attackDamage;

    /************* movement *************/
    Vector2 monsterPosition;
    std::vector<Vector2> path;
    int  currentTargetIndex;
    bool reachedEnd;

    /************* animation *************/
    QVector<QPixmap> animationFrames;
    QVector<QPixmap> attackFrames;
    QVector<QPixmap> deathFrames;
    int currentFrameIndex;

};
