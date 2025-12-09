#include "monster.h"
#include <QString>
#include <QGraphicsScene>
#include <QPainter>
#include "global.h"
#include <vector>
#include "utils.h"
#include<cmath>

using namespace std;

// 怪物基类构造函数
Monster::Monster(MonsterType type,
                 float monsterHealth,
                 float monsterSpeed,
                 int monsterGold,
                 float attackDamage,
                 QGraphicsItem*parent )
    :QGraphicsPixmapItem(parent),
    type(type),   // 设置怪物类型
    currentState(MonsterState::MOVING),
    monsterHealth(monsterHealth),    // 设置初始生命值
    monsterSpeed(monsterSpeed),      // 设置移动速度
    monsterGold(monsterGold),        // 设置金币奖励
    currentTargetIndex(1),           // 从第0个路径点开始
    reachedEnd(false),               // 初始未到达终点
    attackDamage(attackDamage),      // 初始化攻击伤害
    isAttacking(false),
    currentFrameIndex(0),            // 动画帧索引从0开始
    isAttackAnimation(false),
    animationTimer(nullptr),         // 计时器指针初始化为空
    moveTimer(nullptr),              // 移动计时器指针初始化为空
    attackTimer(nullptr)

{

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

    // 初始化计时器系统
    initializeTimers();
}

// 怪物析构函数
Monster::~Monster()
{
    stopMove();      // 确保停止所有移动和动画
    stopAttack();
    delete animationTimer;  // 释放动画计时器内存
    delete moveTimer;       // 释放移动计时器内存
    delete attackTimer;
}

// 初始化计时器系统
void Monster::initializeTimers()
{
    // 创建动画计时器，每150毫秒触发一次动画更新
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Monster::updateAnimationFrame);

    // 创建移动计时器，每80毫秒触发一次位置更新
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Monster::moveToNextPosition);

    attackTimer = new QTimer(this);
    connect(attackTimer, &QTimer::timeout, this, &Monster::performAttack);
}

// 开始怪物移动
void Monster::startMove()
{
    // 安全检查：如果已到达终点、已死亡或路径点不足，则不启动移动
    if(reachedEnd || isDead() || path.size() < 2) {
        return;
    }

    // 启动动画计时器（150ms间隔）
    animationTimer->start(150);
    // 启动移动计时器（80ms间隔）
    moveTimer->start(80);
}

// 停止所有计时器，冻结怪物状态
void Monster::stopMove()
{
    // 安全停止动画计时器
    //if (animationTimer && animationTimer->isActive()) {
    //    animationTimer->stop();
    //}
    // 安全停止移动计时器
    if (moveTimer && moveTimer->isActive()) {
        moveTimer->stop();
    }
}

// 怪物受到伤害处理（之后来联系子弹）
void Monster::takeDamage(float damage)
{
    monsterHealth -= damage;  // 扣除伤害值

    // 检查生命值是否归零
    if (isDead()) {
        stopMove();  // 停止移动
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

    if(isAttackAnimation&&!attackFrames.isEmpty()){
        currentFrameIndex=(currentFrameIndex+1)%attackFrames.size();
        setPixmap(attackFrames[currentFrameIndex]);
    }
    else if(!animationFrames.isEmpty()) {

        // 更新当前帧索引（循环）
        currentFrameIndex = (currentFrameIndex + 1) % animationFrames.size();

        // 设置当前显示的图片
        setPixmap(animationFrames[currentFrameIndex]);
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
        stopMove();
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
    double moveDistance = monsterSpeed * (80.0 / 1000.0);  // 80ms = 0.08秒

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
            stopMove();
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
    setPos(monsterPosition.x-16, monsterPosition.y-16);

}

//开始攻击营地
void Monster::startAttack()
{
    if(isDead()){
        return;
    }
    isAttacking=true;
    currentState=MonsterState::ATTACKING;

    stopMove();
    if(animationTimer&&animationTimer->isActive()){
        animationTimer->start(150);
    }
    isAttackAnimation=true;
    if(!attackFrames.isEmpty()){
        setPixmap(attackFrames.first());
        currentFrameIndex=0;
        updateAnimationFrame();
    }

    attackTimer->start(MONSTER_ATTACK_INTERWAL);
    emit startedAttackingCamp();
    QTimer::singleShot(500,this,&Monster::performAttack);
}
//停止攻击营地
void Monster::stopAttack()
{
    if(attackTimer&&attackTimer->isActive()){
        attackTimer->stop();
    }
    isAttacking=false;
    currentState=MonsterState::MOVING;
    isAttackAnimation=false;
    if(!animationFrames.isEmpty()){
        setPixmap(animationFrames.first());
        currentFrameIndex=0;
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
    QTimer::singleShot(100,this,[this](){
        setScale(1.0);
    });
}
//----monster1----
Monster1::Monster1(QGraphicsItem* parent):Monster(MonsterType::MONSTER1,MONSTER1_HEALTH,MONSTER1_SPEED,MONSTER1_GOLD,MONSTER1_DAMAGE,parent){
    Monster1::loadAnimationFrames();
    Monster1::loadAttackFrames();
    initializeMonster();
    startMove();
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
    frameFiles << ":/picture/monster1/(5).png"
               << ":/picture/monster1/(6).png"
               << ":/picture/monster1/(7).png"
               << ":/picture/monster1/(8).png";


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
Monster2::Monster2(QGraphicsItem* parent):Monster(MonsterType::MONSTER2,MONSTER2_HEALTH,MONSTER2_SPEED,MONSTER2_GOLD,MONSTER2_DAMAGE,parent){
    Monster2::loadAnimationFrames();
    Monster2::loadAttackFrames();
    initializeMonster();
    startMove();
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
Monster3::Monster3(QGraphicsItem* parent):Monster(MonsterType::MONSTER3,MONSTER3_HEALTH,MONSTER3_SPEED,MONSTER3_GOLD,MONSTER3_DAMAGE,parent){
    Monster3::loadAnimationFrames();
    Monster3::loadAttackFrames();
    initializeMonster();
    startMove();
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
