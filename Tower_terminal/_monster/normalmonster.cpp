#include "_monster/normalmonster.h"

//----monster1----
Monster1::Monster1(QGraphicsItem* parent):Monster(MonsterType::MONSTER1,parent){
    Monster1::loadAnimationFrames();
    Monster1::loadAttackFrames();
    Monster1::loadDeathFrames();
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

void Monster1::loadDeathFrames()
{
    deathFrames.clear();

    QStringList deathFrameFiles;
    deathFrameFiles << ":/picture/monster1/(13).png"
                    << ":/picture/monster1/(14).png"
                    << ":/picture/monster1/(15).png"
                    << ":/picture/monster1/(16).png";

    for(const QString& filePath : deathFrameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            frame = frame.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            deathFrames.append(frame);
        }
    }
}

//----monster2----
Monster2::Monster2(QGraphicsItem* parent):Monster(MonsterType::MONSTER2,parent){
    Monster2::loadAnimationFrames();
    Monster2::loadAttackFrames();
    Monster2::loadDeathFrames();
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

void Monster2::loadDeathFrames()
{
    deathFrames.clear();

    QStringList deathFrameFiles;
    deathFrameFiles << ":/picture/monster2/(13).png"
                    << ":/picture/monster2/(14).png"
                    << ":/picture/monster2/(15).png"
                    << ":/picture/monster2/(16).png";

    for(const QString& filePath : deathFrameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            frame = frame.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            deathFrames.append(frame);
        }
    }
}

//----monster3----
Monster3::Monster3(QGraphicsItem* parent):Monster(MonsterType::MONSTER3,parent){
    Monster3::loadAnimationFrames();
    Monster3::loadAttackFrames();
    Monster3::loadDeathFrames();
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

void Monster3::loadDeathFrames()
{
    deathFrames.clear();

    QStringList deathFrameFiles;
    deathFrameFiles << ":/picture/monster3/(13).png"
                    << ":/picture/monster3/(14).png"
                    << ":/picture/monster3/(15).png"
                    << ":/picture/monster3/(16).png";

    for(const QString& filePath : deathFrameFiles) {
        QPixmap frame(filePath);
        if(!frame.isNull()) {
            frame = frame.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            deathFrames.append(frame);
        }
    }
}
