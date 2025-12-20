#include "_monster/monster.h"

// Qt
#include <QString>
#include <QPointer>
#include <QGraphicsScene>
#include <QGraphicsColorizeEffect>
#include <QPropertyAnimation>
#include <QPainter>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QTextStream>

// STL
#include <vector>
#include <cmath>

// Project
#include "_others/global.h"
#include "_others/utils.h"
#include "_monster/monsterSpawner.h"

using namespace std;

/* =====================================================
 * 构造 / 析构
 * ===================================================== */

// 怪物基类构造函数
Monster::Monster(MonsterType type,
                 QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent),
    type(type),                     // 设置怪物类型
    currentState(MonsterState::MOVING),
    currentTargetIndex(1),           // 从第0个路径点开始
    reachedEnd(false),               // 初始未到达终点
    currentFrameIndex(0)            // 动画帧索引从0开始
{
    // qDebug() << "[Monster::Ctor]" << static_cast<void*>(this);

    switch (type)
    {
    case MonsterType::MONSTER1:
        monsterHealth = MONSTER1_HEALTH;
        monsterSpeed  = MONSTER1_SPEED;
        monsterGold   = MONSTER1_GOLD;
        attackDamage  = MONSTER1_DAMAGE;
        break;

    case MonsterType::MONSTER2:
        monsterHealth = MONSTER2_HEALTH;
        monsterSpeed  = MONSTER2_SPEED;
        monsterGold   = MONSTER2_GOLD;
        attackDamage  = MONSTER2_DAMAGE;
        break;

    case MonsterType::MONSTER3:
        monsterHealth = MONSTER2_HEALTH;
        monsterSpeed  = MONSTER2_SPEED;
        monsterGold   = MONSTER3_GOLD;
        attackDamage  = MONSTER2_DAMAGE;
        break;

    case MonsterType::MONSTER4:
        monsterHealth = MONSTER4_HEALTH;
        monsterSpeed  = MONSTER4_SPEED;
        monsterGold   = MONSTER4_GOLD;
        attackDamage  = MONSTER4_DAMAGE;
        break;

    case MonsterType::BAT_TO_PLAYER:
        monsterHealth = BAT_HEALTH;
        monsterSpeed  = BAT_SPEED;
        monsterGold   = BAT_GOLD;
        attackDamage  = BAT_DAMAGE;
        break;

    case MonsterType::BAT_TO_CAMP:
        monsterHealth = BAT_HEALTH;
        monsterSpeed  = BAT_SPEED;
        monsterGold   = BAT_GOLD;
        attackDamage  = BAT_DAMAGE;
        break;
    }
}

// 怪物析构函数
Monster::~Monster()
{
    // 断开所有连接，防止悬空指针和信号延迟执行
    QObject::disconnect(this, nullptr, nullptr, nullptr);
    //qDebug() << "[Monster::~Monster] destroyed:" << static_cast<void*>(this) << "type:" << int(type);
}

/* =====================================================
 * 初始化
 * ===================================================== */

void Monster::initializeMonster()
{
    // 设置怪物的变换原点为中心点，便于旋转和翻转
    setTransformOriginPoint(16, 16);  // 32x32尺寸的中心点

    if (gMap) {
        path.resize(gMap->monsterPath.size());
        for (int i = 0; i < (int)path.size(); i++) {
            QPointF point = gMap->gridToPixel(
                gMap->monsterPath[i].x(),
                gMap->monsterPath[i].y()
                );
            path[i].x = point.x();
            path[i].y = point.y();
        }

        monsterPosition.x = path[0].x;
        monsterPosition.y = path[0].y;
        setPos(monsterPosition.x - TILESIZE / 2.0,
               monsterPosition.y - TILESIZE / 2.0);
    }

    // 设置初始显示的图片
    if (!animationFrames.isEmpty()) {
        setPixmap(animationFrames.first());
    } else if (!attackFrames.isEmpty()) {
        setPixmap(attackFrames.first());
    }
}

/* =====================================================
 * 绘制 / 受伤 / 死亡
 * ===================================================== */

// 自定义绘制：已经死亡的怪物不再绘制
void Monster::paint(QPainter* painter,
                    const QStyleOptionGraphicsItem* option,
                    QWidget* widget)
{
    if (isDead()) {
        return;
    }
    QGraphicsPixmapItem::paint(painter, option, widget);
}

// 怪物受到伤害处理
void Monster::takeDamage(float damage)
{
    if (isDying()) return;
    if (isDead())  return;

    showDamageEffect(damage);

    monsterHealth -= damage;

    if (monsterHealth <= 0) {
        onDeath();
    }
}

void Monster::showDamageEffect(float damage)
{
    QPointer<Monster> safeThis(this);  // 安全指针，防止对象被删除

    if (!safeThis) return;

    // 创建红色闪烁特效
    QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect(safeThis);
    effect->setColor(Qt::red);
    effect->setStrength(0.8);
    safeThis->setGraphicsEffect(effect);

    // 创建伤害文字
    QGraphicsTextItem* damageText =
        new QGraphicsTextItem(QString::number(damage), safeThis);
    damageText->setDefaultTextColor(Qt::red);
    damageText->setFont(QFont("Arial", 10, QFont::Bold));
    damageText->setPos(-10, -20);

    // 文字上浮动画
    QPropertyAnimation* anim = new QPropertyAnimation(damageText, "pos");
    anim->setDuration(500);
    anim->setStartValue(damageText->pos());
    anim->setEndValue(damageText->pos() + QPointF(0, -30));
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    // 使用 QPointer 包装 effect 与 damageText，避免在对象已删除时再次操作
    QPointer<QGraphicsEffect> safeEffect(effect);
    QPointer<QGraphicsTextItem> safeDamageText(damageText);

    // 定时移除特效和文字（不传 receiver，使用 safeThis 保护）
    QTimer::singleShot(200, [safeThis, safeEffect, safeDamageText]() {
        if (!safeThis) return;

        // 移除特效（确保 effect 仍然存在）
        if (safeEffect && safeThis->graphicsEffect() == safeEffect) {
            safeThis->setGraphicsEffect(nullptr);
        }

        // 删除文字
        if (safeDamageText) {
            safeDamageText->deleteLater();
        }

        // 删除特效对象
        if (safeEffect) {
            safeEffect->deleteLater();
        }
    });
}

void Monster::markDead()
{
    if (isDead()) {
        // 防止重复调用markDead导致重复发出信号
        return;
    }
    
    currentState = MonsterState::DEAD;
    //qDebug() << "[Monster::markDead]" << static_cast<void*>(this) << "gold=" << monsterGold;
    // 发出死亡信号，但不直接删除对象
    // 让外部管理器处理清理工作
    emit died(type);
}

// 基类的 onDeath 默认实现
void Monster::onDeath()
{
    if (isDying()) {
        return;
    }

    currentState = MonsterState::DYING;

    setEnabled(false);

    qDebug() << "[Monster::onDeath]" << static_cast<void*>(this) << "starting death animation";
    playDeathAnimation();
}

/* =====================================================
 * 动画
 * ===================================================== */

void Monster::playDeathAnimation()
{
    if (deathFrames.isEmpty()) {
        setOpacity(0.0);
        setVisible(false);
        markDead();
        return;
    }

    QPointer<Monster> safeThis(this);  // 安全指针，防止对象被删除

    const int frameCount = deathFrames.size();
    const int frameInterval = 200; // 每帧间隔 200ms

    for (int i = 0; i < frameCount; ++i) {
        QTimer::singleShot(i * frameInterval, [safeThis, i, frameCount]() {

            // 检查对象是否仍然存在
            if (!safeThis)
                return;

            // 避免索引越界
            if (i < 0 || i >= safeThis->deathFrames.size())
                return;

            // 设置当前帧
            safeThis->setPixmap(safeThis->deathFrames[i]);
            safeThis->setOpacity(1.0 - 1.0 * i / frameCount);
            safeThis->update();

            // 最后一帧：隐藏并标记死亡
            if (i == frameCount - 1) {
                safeThis->setOpacity(0.0);
                safeThis->setVisible(false);
                safeThis->markDead();
            }
        });
    }
}

void Monster::updateAnimationFrame()
{
    if (isDying()) return;
    if (isDead())  return;

    QVector<QPixmap>* currentFrames = nullptr;
    int* currentIndex = &currentFrameIndex;
    int frameCount = 0;

    if (isAttacking() && !attackFrames.isEmpty()) {
        currentFrames = &attackFrames;
        frameCount = attackFrames.size();
    } else if (!animationFrames.isEmpty()) {
        currentFrames = &animationFrames;
        frameCount = animationFrames.size();
    }

    if (currentFrames && !currentFrames->isEmpty() && frameCount > 0) {
        *currentIndex = (*currentIndex + 1) % frameCount;
        setPixmap(currentFrames->at(*currentIndex));
    }
}

/* =====================================================
 * 移动 / 攻击
 * ===================================================== */

void Monster::moveToNextPosition()
{
    if (isDead() || isDying() || currentState == MonsterState::ATTACKING)
        return;

    if (reachedEnd || currentTargetIndex >= (int)path.size()) {
        reachedEnd = true;
        emit reachedDestination();
        startAttack();
        return;
    }

    Vector2 currentPos = monsterPosition;
    Vector2 targetPos  = path[currentTargetIndex];

    Vector2 direction = (targetPos - currentPos).normalized();
    double distance   = currentPos.distanceTo(targetPos);
    double moveDistance =
        monsterSpeed * (MONSTERMOVE / 1000.0);

    if (fabs(direction.x) > fabs(direction.y)) {
        if (direction.x > 0) {
            setRotation(-90);
        } else {
            setRotation(90);
        }
    } else {
        if (direction.y > 0) {
            setRotation(0);
        } else {
            setRotation(180);
        }
    }

    if (moveDistance >= distance) {
        monsterPosition = targetPos;
        currentTargetIndex++;

        if (currentTargetIndex >= (int)path.size()) {
            reachedEnd = true;
            emit reachedDestination();
            startAttack();
            return;
        }

        double extraDistance = moveDistance - distance;
        if (extraDistance > 0) {
            Vector2 nextTargetPos = path[currentTargetIndex];
            Vector2 nextDirection =
                (nextTargetPos - monsterPosition).normalized();
            monsterPosition =
                monsterPosition + nextDirection * extraDistance;
        }
    } else {
        monsterPosition =
            monsterPosition + direction * moveDistance;
    }

    setPos(monsterPosition.x - 16,
           monsterPosition.y - 16);
}

// 开始攻击营地
void Monster::startAttack()
{
    if (isDead())  return;
    if (isDying()) return;

    currentState = MonsterState::ATTACKING;

    if (!attackFrames.isEmpty()) {
        currentFrameIndex = 0;
        setPixmap(attackFrames.first());
        updateAnimationFrame();
    }

    emit startedAttackingCamp();
}

// 攻击定时器回调
void Monster::onAttackTimerTimeout()
{
    if (isDead() || isDying()) {
        return;
    }
    
    if (isAttacking()) {
        performAttack();
    }
}

void Monster::performAttack()
{
    QPointer<Monster> safeThis(this);  // 安全指针，防止对象被删除

    if (!safeThis) return;             // 对象已删除
    if (safeThis->isDead() || safeThis->isDying()) return;

    // 触发攻击事件
    emit safeThis->attackedCamp(safeThis->attackDamage);
    qDebug() << "performAttack" << QTime::currentTime() << safeThis->attackDamage;

    // 放大动画效果
    safeThis->setScale(1.2);
    QTimer::singleShot(ATTACKDELTA, [safeThis]() {
        if (!safeThis) return;
        if (safeThis->isDead() || safeThis->isDying()) return;
        if (!safeThis->scene()) return;

        safeThis->setScale(1.0);
    });
}
