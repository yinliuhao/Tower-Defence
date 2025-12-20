#ifndef FLYMONSTERS_H
#define FLYMONSTERS_H

#include "_monster/monster.h"

/***********************************
 *          Bat Base
 ***********************************/
// 飞行单位基类（无视地面路径）
class BatBase : public Monster {
    Q_OBJECT
public:
    explicit BatBase(MonsterType type,
                     QGraphicsItem* parent = nullptr);

    /************* override *************/
    void initializeMonster() override;
    void moveToNextPosition() override;

    /************* combat *************/
    void startAttack() override;
    void performAttack() override;

    void continueChaseAfterAttack();

protected:
    /************* resources *************/
    virtual void loadAnimationFrames() override = 0;
    virtual void loadAttackFrames()    override = 0;

    /************* bat logic *************/
    void updateTargetPosition();

protected:
    float attackRange;
    bool  hasAttacked;
    bool  isChasing;
    bool  moveToPlayer;

private:
    /************* optimization *************/
    int updateCounter;
    int updateFrequency;

    int targetUpdateCounter;
    int targetUpdateFrequency;

    Vector2 targetPosition;

private:
    void checkAttackCondition();
    void smoothMove();
};


/***********************************
 *        Bat Variants
 ***********************************/
class BatToPlayer : public BatBase {
    Q_OBJECT
public:
    explicit BatToPlayer(QGraphicsItem* parent = nullptr);

    void resetAttackState() { hasAttacked = false; }

protected:
    void loadAnimationFrames() override;
    void loadAttackFrames()    override;
    void loadDeathFrames()     override;
};

/**********************************/

class BatToCamp : public BatBase {
    Q_OBJECT
public:
    explicit BatToCamp(QGraphicsItem* parent = nullptr);

protected:
    void loadAnimationFrames() override;
    void loadAttackFrames()    override;
    void loadDeathFrames()     override;
};



#endif // FLYMONSTERS_H
