#include "monster.h"
#include <QString>
#include <QGraphicsScene>
#include <QPainter>
#include "global.h"
#include <vector>
#include "utils.h"
#include<cmath>
#include"monsterSpawner.h"

using namespace std;

// 怪物基类构造函数
Monster::Monster(MonsterType type,
                 QGraphicsItem*parent )
    :QGraphicsPixmapItem(parent),
    type(type),   // 设置怪物类型
    currentState(MonsterState::MOVING),
    currentTargetIndex(1),           // 从第0个路径点开始
    reachedEnd(false),               // 初始未到达终点
    isAttacking(false),
    currentFrameIndex(0),            // 动画帧索引从0开始
    isAttackAnimation(false)
{
    switch(type)
    {
    case MonsterType::MONSTER1:
        monsterHealth = MONSTER1_HEALTH;
        monsterSpeed = MONSTER1_SPEED;
        monsterGold = MONSTER1_GOLD;
        attackDamage = MONSTER1_DAMAGE;
        break;
    case MonsterType::MONSTER2:
        monsterHealth = MONSTER2_HEALTH;
        monsterSpeed = MONSTER2_SPEED;
        monsterGold = MONSTER2_GOLD;
        attackDamage = MONSTER2_DAMAGE;
        break;
    case MonsterType::MONSTER3:
        monsterHealth = MONSTER2_HEALTH;
        monsterSpeed = MONSTER2_SPEED;
        monsterGold = MONSTER3_GOLD;
        attackDamage = MONSTER2_DAMAGE;
        break;
    case MonsterType::MONSTER4:
        monsterHealth = MONSTER4_HEALTH;
        monsterSpeed = MONSTER4_SPEED;
        monsterGold = MONSTER4_GOLD;
        attackDamage = MONSTER4_DAMAGE;
        break;
    }
}

void Monster::initializeMonster()
{
    // 设置怪物的变换原点为中心点，便于旋转和翻转
    setTransformOriginPoint(16, 16);  // 32x32尺寸的中心点
    if(gMap){
    path.resize(gMap->monsterPath.size());
    for(int i = 0; i < (int)path.size(); i++)
    {
        QPointF point = gMap->gridToPixel(gMap->monsterPath[i].x(),  gMap->monsterPath[i].y());
        path[i].x = point.x();
        path[i].y = point.y();
    }
    monsterPosition.x=path[0].x;
    monsterPosition.y=path[0].y;
    setPos(monsterPosition.x - TILESIZE / 2.0, monsterPosition.y - TILESIZE / 2.0);
    }
    // 加载动画帧资源

    // 设置初始显示的图片
    if(!animationFrames.isEmpty()) {
        setPixmap(animationFrames.first());
    }else if(!attackFrames.isEmpty()){
        setPixmap(attackFrames.first());}
}

// 怪物析构函数
Monster::~Monster()
{
    stopAttack();
}

// 怪物受到伤害处理（之后来联系子弹）
void Monster::takeDamage(float damage)
{
    monsterHealth -= damage;  // 扣除伤害值

    // 检查生命值是否归零
    if (isDead()) {
        currentState = MonsterState::DEAD;
        emit died(monsterGold);  // 发射死亡信号，传递金币奖励

        // 从场景中移除并安全删除
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();  // 安全延迟删除
        return;
    }
}

// 更新动画帧
void Monster::updateAnimationFrame()
{

    if (isDead()) return;

    // 根据当前状态选择动画帧集合
    QVector<QPixmap>* currentFrames = nullptr;
    int* currentIndex = &currentFrameIndex;
    int frameCount = 0;

    if (isAttackAnimation && !attackFrames.isEmpty()) {
        // 攻击动画
        currentFrames = &attackFrames;
        frameCount = attackFrames.size();
    } else if (!animationFrames.isEmpty()) {
        // 移动动画
        currentFrames = &animationFrames;
        frameCount = animationFrames.size();
    }

    // 如果有可用的动画帧，更新它们
    if (currentFrames && !currentFrames->isEmpty() && frameCount > 0) {
        *currentIndex = (*currentIndex + 1) % frameCount;
        setPixmap(currentFrames->at(*currentIndex));
    }
}


// 移动到下一个路径点
void Monster::moveToNextPosition()
{
    if(isDead()){
        // 从场景中移除
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();  // 安全删除
    }
    if(currentState==MonsterState::ATTACKING){
        return;
    }
    // 终止条件检查：已到达终点、已死亡或超出路径范围
    if(reachedEnd || currentTargetIndex >= (int)path.size()) {
        reachedEnd = true;
        emit reachedDestination();  // 发射到达终点信号

        startAttack();
        return;
    }

    // 获取当前位置和目标位置
    Vector2 currentPos = monsterPosition;
    Vector2 targetPos = path[currentTargetIndex];

    // 计算移动方向向量并归一化
    Vector2 direction = (targetPos - currentPos).normalized();

    // 计算到目标点的距离
    double distance = currentPos.distanceTo(targetPos);

    // 计算本次移动的距离（基于速度和帧时间）
    double moveDistance = monsterSpeed * (MONSTERMOVE / 1000.0);  // 80ms = 0.08秒

    if(fabs(direction.x)>fabs(direction.y)){
        if(direction.x>0){
            setRotation(-90);
        }else{
            setRotation(90);
        }
    }else{
        if(direction.y>0){
            setRotation(0);
        }else{
            setRotation(180);
        }
    }


    // 如果移动距离大于等于到目标点的距离，说明可以到达目标点
    if(moveDistance >= distance) {
        // 直接移动到目标点
        monsterPosition = targetPos;
        currentTargetIndex++;  // 切换到下一个路径点

        // 检查是否到达路径终点
        if(currentTargetIndex >= (int)path.size()) {
            reachedEnd = true;
            emit reachedDestination();  // 发射到达终点信号

            startAttack();
            return;
        }

        // 处理多余的移动距离（移动到下一个路径点的部分距离）
        double extraDistance = moveDistance - distance;
        if(extraDistance > 0) {
            // 继续向新的目标点移动剩余距离
            Vector2 nextTargetPos = path[currentTargetIndex];
            Vector2 nextDirection = (nextTargetPos - monsterPosition).normalized();
            monsterPosition = monsterPosition + nextDirection * extraDistance;
        }
    } else {
        // 正常移动：按方向和距离移动
        monsterPosition = monsterPosition + direction * moveDistance;
    }

    // 更新图形项在场景中的位置
    setPos(monsterPosition.x - 16, monsterPosition.y - 16);

}

//开始攻击营地
void Monster::startAttack()
{
    if(isDead())    return;
    isAttacking = true;
    currentState = MonsterState::ATTACKING;
    isAttackAnimation = true;


    if(!attackFrames.isEmpty()){

        currentFrameIndex = 0;
        setPixmap(attackFrames.first());
        updateAnimationFrame();
    }


    emit startedAttackingCamp();
    QTimer::singleShot(ATTACKDELAY, this,&Monster::performAttack);
}

//停止攻击营地
void Monster::stopAttack()
{
    isAttacking = false;
    currentState = MonsterState::MOVING;
    isAttackAnimation = false;
    if(!animationFrames.isEmpty()){
        setPixmap(animationFrames.first());
        currentFrameIndex = 0;
    }
    emit stoppedAttackingCamp();
}

void Monster::performAttack(){
    if(isDead()||!isAttacking){
        stopAttack();
        return;
    }
    emit attackedCamp(attackDamage);
    setScale(1.2);
    QTimer::singleShot(ATTACKDELTA, this,[this](){
        setScale(1.0);
    });

    QTimer::singleShot(ATTACKDELAY, this, &Monster::performAttack);
}

//----monster1----
Monster1::Monster1(QGraphicsItem* parent):Monster(MonsterType::MONSTER1,parent){
    Monster1::loadAnimationFrames();
    Monster1::loadAttackFrames();
    initializeMonster();
}

void Monster1::loadAnimationFrames(){
    animationFrames.clear();
    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/picture/monster1/(1).png"
               << ":/picture/monster1/(2).png"
               << ":/picture/monster1/(3).png"
               << ":/picture/monster1/(4).png";

    // 加载并处理每一帧图片
    for(const QString& filePath : frameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            // 将图片缩放为32x32尺寸，保持宽高比，使用平滑变换
            frame = frame.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            animationFrames.append(frame);
        }
    }

    // 设置初始显示的图片
    if (!animationFrames.isEmpty()) {
        setPixmap(animationFrames.first());
    }
}

void Monster1::loadAttackFrames(){
    attackFrames.clear();
    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/picture/monster1/(9).png"
               << ":/picture/monster1/(10).png"
               << ":/picture/monster1/(11).png"
               << ":/picture/monster1/(12).png";


    // 加载并处理每一帧图片
    for(const QString& filePath : frameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            // 将图片缩放为32x32尺寸，保持宽高比，使用平滑变换
            frame = frame.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            attackFrames.append(frame);
        }
    }

}
//----monster2----
Monster2::Monster2(QGraphicsItem* parent):Monster(MonsterType::MONSTER2,parent){
    Monster2::loadAnimationFrames();
    Monster2::loadAttackFrames();
    initializeMonster();
}

void Monster2::loadAnimationFrames(){
    animationFrames.clear();
    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/picture/monster2/(1).png"
               << ":/picture/monster2/(2).png"
               << ":/picture/monster2/(3).png"
               << ":/picture/monster2/(4).png";

    // 加载并处理每一帧图片
    for(const QString& filePath : frameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            // 将图片缩放为32x32尺寸，保持宽高比，使用平滑变换
            frame = frame.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            animationFrames.append(frame);
        }
    }

    // 设置初始显示的图片
    if (!animationFrames.isEmpty()) {
        setPixmap(animationFrames.first());
    }
}
void Monster2::loadAttackFrames(){
    attackFrames.clear();
    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/picture/monster2/(5).png"
               << ":/picture/monster2/(6.png"
               << ":/picture/monster2/(7).png"
               << ":/picture/monster2/(8).png";


    // 加载并处理每一帧图片
    for(const QString& filePath : frameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            // 将图片缩放为32x32尺寸，保持宽高比，使用平滑变换
            frame = frame.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            attackFrames.append(frame);
        }
    }

}
//----monster3----
Monster3::Monster3(QGraphicsItem* parent):Monster(MonsterType::MONSTER3,parent){
    Monster3::loadAnimationFrames();
    Monster3::loadAttackFrames();
    initializeMonster();
}

void Monster3::loadAnimationFrames(){
    animationFrames.clear();
    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/picture/monster3/(1).png"
               << ":/picture/monster3/(2).png"
               << ":/picture/monster3/(3).png"
               << ":/picture/monster3/(4).png";

    // 加载并处理每一帧图片
    for(const QString& filePath : frameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            // 将图片缩放为32x32尺寸，保持宽高比，使用平滑变换
            frame = frame.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            animationFrames.append(frame);
        }
    }

    // 设置初始显示的图片
    if (!animationFrames.isEmpty()) {
        setPixmap(animationFrames.first());
    }
}

void Monster3::loadAttackFrames(){
    attackFrames.clear();
    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/picture/monster3/(5).png"
               << ":/picture/monster3/(6).png"
               << ":/picture/monster3/(7).png"
               << ":/picture/monster3/(8).png";


    // 加载并处理每一帧图片
    for(const QString& filePath : frameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            // 将图片缩放为32x32尺寸，保持宽高比，使用平滑变换
            frame = frame.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            attackFrames.append(frame);
        }
    }

}
