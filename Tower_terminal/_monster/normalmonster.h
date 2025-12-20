#ifndef NORMALMONSTER_H
#define NORMALMONSTER_H

#include "_monster/monster.h"

// ==================== 普通怪物类 ====================

/**
 * @brief 怪物1类
 * 基础普通怪物，继承自Monster基类
 */
class Monster1 : public Monster
{
    Q_OBJECT

public:
    explicit Monster1(QGraphicsItem* parent = nullptr);

protected:
    // 重写动画帧加载函数
    void loadAnimationFrames() override;
    void loadAttackFrames()    override;
    void loadDeathFrames()     override;
};

/**
 * @brief 怪物2类
 * 进阶普通怪物，继承自Monster基类
 */
class Monster2 : public Monster
{
    Q_OBJECT

public:
    explicit Monster2(QGraphicsItem* parent = nullptr);

protected:
    // 重写动画帧加载函数
    void loadAnimationFrames() override;
    void loadAttackFrames()    override;
    void loadDeathFrames()     override;
};

/**
 * @brief 怪物3类
 * 高级普通怪物，继承自Monster基类
 */
class Monster3 : public Monster
{
    Q_OBJECT

public:
    explicit Monster3(QGraphicsItem* parent = nullptr);

protected:
    // 重写动画帧加载函数
    void loadAnimationFrames() override;
    void loadAttackFrames()    override;
    void loadDeathFrames()     override;
};

#endif // NORMALMONSTER_H
