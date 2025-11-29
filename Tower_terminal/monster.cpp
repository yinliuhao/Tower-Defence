#include <iostream>
#include <algorithm>
#include "monster.h"
#include "bullet.h"
#include "map.h"
#include "utils.h"
#include <QString>
#include <QGraphicsScene>
#include <QPainter>

using namespace std;


//怪物基类构造函数,初始化怪物所有属性并设置初始状态
Monster::Monster(vector<Vector2> path,
                 MonsterType type,
                 float monsterHealth,
                 float monsterSpeed,
                 int monsterGold,
                 QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent),     // 初始化QGraphicsPixmapItem
    type(type),                      // 设置怪物类型
    monsterHealth(monsterHealth),    // 设置初始生命值
    monsterSpeed(monsterSpeed),      // 设置移动速度
    monsterGold(monsterGold),        // 设置金币奖励
    path(path),                      // 设置移动路径
    currentTargetIndex(1),           // 从第1个路径点开始移动（第0个是起始点）
    reachedEnd(false),               // 初始未到达终点
    currentFrameIndex(0),            // 动画帧索引从0开始
    animationTimer(nullptr),         // 计时器指针初始化为空
    moveTimer(nullptr)               // 移动计时器指针初始化为空
{
    // 设置怪物的初始位置为路径的第一个点
    if(!path.empty()) {
        monsterPosition = path[0];
        setPos(monsterPosition.x, monsterPosition.y);  // 设置图形项位置
    }

    // 设置怪物的变换原点为中心点，便于旋转和翻转
    setTransformOriginPoint(16, 16);  // 32x32尺寸的中心点
}
void Monster::initializeMonster()
{
    // 加载动画帧资源
    loadAnimationFrames();

    // 设置初始显示的图片
    if(!animationFrames.isEmpty()) {
        setPixmap(animationFrames.first());
    }

    // 初始化计时器系统
    initializeTimers();
}

//怪物析构函数 ,负责清理资源，停止所有计时器
Monster::~Monster()
{
    stopMove();      // 确保停止所有移动和动画
    delete animationTimer;  // 释放动画计时器内存
    delete moveTimer;       // 释放移动计时器内存
}

// 初始化计时器系统,创建动画计时器和移动计时器，并连接对应的槽函数
void Monster::initializeTimers()
{
    // 创建动画计时器，每150毫秒触发一次动画更新
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Monster::updateAnimationFrame);

    // 创建移动计时器，每80毫秒触发一次位置更新
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Monster::moveToNextPosition);
}

// 开始怪物移动,启动动画计时器和移动计时器，使怪物开始沿路径移动
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


//停止所有计时器，冻结怪物状态
void Monster::stopMove()
{
    // 安全停止动画计时器
    if (animationTimer && animationTimer->isActive()) {
        animationTimer->stop();
    }
    // 安全停止移动计时器
    if (moveTimer && moveTimer->isActive()) {
        moveTimer->stop();
    }
}


//怪物受到伤害处理
//damage 受到的伤害值 减少生命值，检查是否死亡，触发相应信号
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
    }
}

//更新动画帧
// 循环播放动画帧，实现行走动画效果
void Monster::updateAnimationFrame()
{
    // 安全检查：确保有动画帧可用
    if(animationFrames.isEmpty()) return;

    // 更新当前帧索引（循环）
    currentFrameIndex = (currentFrameIndex + 1) % animationFrames.size();

    // 设置当前显示的图片
    setPixmap(animationFrames[currentFrameIndex]);
}

// 移动到下一个路径点
//计算移动方向和距离，更新怪物位置，处理路径点切换
void Monster::moveToNextPosition()
{
    // 终止条件检查：已到达终点、已死亡或超出路径范围
    if(reachedEnd || isDead() || currentTargetIndex >= (int)path.size()) {
        reachedEnd = true;
        stopMove();
        emit reachedDestination();  // 发射到达终点信号

        // 从场景中移除
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();  // 安全删除
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
    double moveDistance = monsterSpeed * SPEEDDELTA;

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

            // 从场景中移除并删除
            if (scene()) {
                scene()->removeItem(this);
            }
            deleteLater();
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
    setPos(monsterPosition.x, monsterPosition.y);

    // 根据水平移动方向调整怪物朝向（左右翻转）
    if (direction.x > 0.1) {
        // 向右移动：正常显示
        setTransform(QTransform().scale(1, 1));
    } else if (direction.x < -0.1) {
        // 向左移动：水平翻转
        setTransform(QTransform().scale(-1, 1));
    }
}

//-------Monster1类实现-------
//怪物1构造函数
//设置怪物1的特定属性：生命值100，速度60，金币奖励10

Monster1::Monster1(vector<Vector2> path, QGraphicsItem* parent)
    : Monster(path, MonsterType::MONSTER1, 100.0f, 60.0f, 10, parent) {
    initializeMonster();
}

// 加载怪物1的动画帧
//从资源文件加载4张行走动画图片，并缩放为32x32尺寸
void Monster1::loadAnimationFrames() {
    animationFrames.clear();

    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/monster/monster1/walk1.png"
               << ":/monster/monster1/walk2.png"
               << ":/monster/monster1/walk3.png"
               << ":/monster/monster1/walk4.png";

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
