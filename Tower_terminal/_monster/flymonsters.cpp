#include "_monster/flymonsters.h"
#include "_monster/monster.h"
#include "_others/global.h"
#include "_others/utils.h"
#include "QPointer"

// 蝙蝠基类构造函数
BatBase::BatBase(MonsterType type, QGraphicsItem* parent)
    : Monster(type, parent),
    attackRange(16.0f),       // 50像素攻击范围
    hasAttacked(false),
    isChasing(true),
    moveToPlayer(type==MonsterType::BAT_TO_PLAYER),
    updateCounter(0),
    updateFrequency(2),
    targetUpdateCounter(0),
    targetUpdateFrequency(10)

{
    // 设置怪物状态
    currentState = MonsterState::MOVING;

    // 设置中心点
    setTransformOriginPoint(24, 24);

}



// 初始化蝙蝠
void BatBase::initializeMonster()
{
    // 设置初始位置（从起点开始）
    if(gMap && !gMap->monsterPath.empty()) {
        QPointF startPoint = gMap->gridToPixel(gMap->monsterPath[0].x(), gMap->monsterPath[0].y());
        monsterPosition.x = startPoint.x();
        monsterPosition.y = startPoint.y();
        setPos(monsterPosition.x - 24, monsterPosition.y - 24);
    }

    // 设置初始目标
    updateTargetPosition();

    // 设置初始显示的图片
    if(!animationFrames.isEmpty()) {
        setPixmap(animationFrames.first());
    }
}



// 蝙蝠移动逻辑（直接飞向目标）
void BatBase::moveToNextPosition()
{
    if (isDead() || currentState == MonsterState::ATTACKING) {
        return;
    }

    updateCounter++;
    if(updateCounter%updateFrequency!=0){
        return;
    }

    // 更新目标位置（动态追踪）
    targetUpdateCounter++;
    if(targetUpdateCounter%targetUpdateFrequency!=0){
        updateTargetPosition();
        targetUpdateCounter=0;
    }


    smoothMove();
}
// 平滑移动（优化性能）
void BatBase::smoothMove()
{
    if(isDead()||isDying()){
        return;
    }
    if (!isChasing) return;

    // 计算到目标的距离
    float distance = monsterPosition.distanceTo(targetPosition);

    // 如果已经在攻击范围内，开始攻击
    if (distance <= attackRange && !hasAttacked) {
        startAttack();
        return;
    }

    // 计算移动方向
    Vector2 direction = (targetPosition - monsterPosition).normalized();

    if(direction.x>0){
        setTransform(QTransform().scale(-1, 1));
    }


    // 计算移动距离
    float moveDistance = monsterSpeed * (MONSTERMOVE / 1000.0f);

    // 正常移动
    monsterPosition = monsterPosition + direction * moveDistance;

    // 更新位置
    setPos(monsterPosition.x - 24, monsterPosition.y - 24);
}

// 开始攻击
void BatBase::startAttack()
{
    if (isDead()) return;
    currentState = MonsterState::ATTACKING;
    isChasing = false;  // 停止追击

    if (!attackFrames.isEmpty()) {
        currentFrameIndex = 0;
        setPixmap(attackFrames.first());
    }

    // 执行攻击
    performAttack();
}

// 执行攻击（重写父类方法）
void BatBase::performAttack()
{
    if (isDead()) {
        return;
    }

    // 根据攻击目标发射不同的信号
    if (moveToPlayer) {
        // 攻击玩家
        emit attackedPlayer(attackDamage);
        qDebug() << "蝙蝠攻击玩家，造成伤害：" << attackDamage;
    } else {
        // 攻击营地
        emit attackedCamp(attackDamage);
        qDebug() << "蝙蝠攻击营地，造成伤害：" << attackDamage;
    }

    // 攻击特效（使用 QPointer 保护，避免 this 被删除后悬空调用）
    setScale(1.2);
    {
        QPointer<BatBase> safeThis(this);
        QTimer::singleShot(ATTACKDELTA, [safeThis]() {
            if (!safeThis) return;
            safeThis->setScale(1.0);
        });
    }

    hasAttacked = true;

    // 攻击后等待一段时间继续追击（使用 QPointer 保护）
    {
        QPointer<BatBase> safeThis(this);
        QTimer::singleShot(ATTACKDELAY, [safeThis]() {
            if (!safeThis) return;
            safeThis->continueChaseAfterAttack();
        });
    }
}

// 攻击后继续追击
void BatBase::continueChaseAfterAttack()
{
    if (isDead()) return;

    // 重置攻击状态，允许再次攻击
    hasAttacked = false;
    isChasing = true;
    currentState = MonsterState::MOVING;

    // 更新目标位置
    updateTargetPosition();

    // 设置移动动画
    if (!animationFrames.isEmpty()) {
        setPixmap(animationFrames.first());
        currentFrameIndex = 0;
    }
}

// 更新目标位置（蝙蝠特有逻辑）
void BatBase::updateTargetPosition()
{


    if (moveToPlayer && me) {
        // 向玩家移动：获取玩家当前位置
        QPointF playerPos = me->pos();
        targetPosition.x = playerPos.x() + TILESIZE / 2.0f;
        targetPosition.y = playerPos.y() + TILESIZE / 2.0f;
    } else if (gMap && !gMap->monsterPath.empty()) {
        // 向营地移动：获取路径终点
        int lastIndex = gMap->monsterPath.size() - 1;
        QPointF campPoint = gMap->gridToPixel(gMap->monsterPath[lastIndex].x(),
                                              gMap->monsterPath[lastIndex].y());
        targetPosition.x = campPoint.x();
        targetPosition.y = campPoint.y();
    }
}

// 检查攻击条件
void BatBase::checkAttackCondition()
{
    if (isDead()) return;

    // 计算到目标的距离
    float distance = monsterPosition.distanceTo(targetPosition);

    // 如果在攻击范围内，开始攻击
    if (distance <= attackRange) {
        performAttack();
    }
    BatBase::moveToNextPosition();
}

// 攻击玩家的蝙蝠类实现
BatToPlayer::BatToPlayer(QGraphicsItem* parent)
    : BatBase(MonsterType::BAT_TO_PLAYER, parent)
{


    BatToPlayer::loadAnimationFrames();
    BatToPlayer::loadAttackFrames();
    BatToPlayer::loadDeathFrames();
    initializeMonster();
}

void BatToPlayer::loadAnimationFrames(){
    animationFrames.clear();
    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/picture/bat/(1).png"
               << ":/picture/bat/(2).png"
               << ":/picture/bat/(3).png"
               << ":/picture/bat/(4).png";

    // 加载并处理每一帧图片
    for(const QString& filePath : frameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            // 将图片缩放为32x32尺寸，保持宽高比，使用平滑变换
            frame = frame.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            animationFrames.append(frame);
        }
    }

    // 设置初始显示的图片
    if (!animationFrames.isEmpty()) {
        setPixmap(animationFrames.first());
    }
}

void BatToPlayer:: loadAttackFrames(){
    attackFrames.clear();
    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/picture/bat/(5).png"
               << ":/picture/bat/(6).png"
               << ":/picture/bat/(7).png"
               << ":/picture/bat/(8).png";


    // 加载并处理每一帧图片
    for(const QString& filePath : frameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            // 将图片缩放为32x32尺寸，保持宽高比，使用平滑变换
            frame = frame.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            attackFrames.append(frame);
        }
    }

}

void BatToPlayer::loadDeathFrames()
{
    deathFrames.clear();

    QStringList deathFrameFiles;
    deathFrameFiles << ":/picture/bat/(9).png"
                    << ":/picture/bat/(10).png"
                    << ":/picture/bat/(11).png"
                    << ":/picture/bat/(12).png";

    for(const QString& filePath : deathFrameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            frame = frame.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            deathFrames.append(frame);
        }
    }
}


BatToCamp::BatToCamp(QGraphicsItem* parent)
    : BatBase(MonsterType::BAT_TO_CAMP, parent)
{
    BatToCamp::loadAnimationFrames();
    BatToCamp::loadAttackFrames();
    BatToCamp::loadDeathFrames();
    initializeMonster();
}

void BatToCamp::loadAnimationFrames(){
    animationFrames.clear();
    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/picture/bat/(13).png"
               << ":/picture/bat/(14).png"
               << ":/picture/bat/(15).png"
               << ":/picture/bat/(16).png";

    // 加载并处理每一帧图片
    for(const QString& filePath : frameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            // 将图片缩放为32x32尺寸，保持宽高比，使用平滑变换
            frame = frame.scaled(48, 48 , Qt::KeepAspectRatio, Qt::SmoothTransformation);
            animationFrames.append(frame);
        }
    }

    // 设置初始显示的图片
    if (!animationFrames.isEmpty()) {
        setPixmap(animationFrames.first());
    }
}

void BatToCamp:: loadAttackFrames(){
    attackFrames.clear();
    // 定义动画帧文件路径
    QStringList frameFiles;
    frameFiles << ":/picture/bat/(17).png"
               << ":/picture/bat/(18).png"
               << ":/picture/bat/(19).png"
               << ":/picture/bat/(20).png";


    // 加载并处理每一帧图片
    for(const QString& filePath : frameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            // 将图片缩放为32x32尺寸，保持宽高比，使用平滑变换
            frame = frame.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            attackFrames.append(frame);
        }
    }

}

void BatToCamp::loadDeathFrames()
{
    deathFrames.clear();

    QStringList deathFrameFiles;
    deathFrameFiles << ":/picture/bat/(21).png"
                    << ":/picture/bat/(22).png"
                    << ":/picture/bat/(23).png"
                    << ":/picture/bat/(24).png";

    for(const QString& filePath : deathFrameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            frame = frame.scaled(48, 48, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            deathFrames.append(frame);
        }
    }
}



