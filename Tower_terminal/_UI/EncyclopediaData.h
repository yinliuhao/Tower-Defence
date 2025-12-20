#ifndef ENCYCLOPEDIADATA_H
#define ENCYCLOPEDIADATA_H
#pragma once
#include <QString>
#include <QPixmap>

// 怪物图鉴信息
struct MonsterEncyclopediaInfo {
    QString name;
    QString description;
    int health;
    int speed;
    int goldReward;
    int attackDamage;
    QString imagePath;
    QString specialAbility;
};

// 炮塔图鉴信息
struct TowerEncyclopediaInfo {
    QString name;
    QString description;
    int health;
    int buildCost;
    int attackRange;
    int attackInterval;
    int bulletDamage;
    int bulletSpeed;
    QString imagePath;
    QString specialAbility;
};

class EncyclopediaData {
public:
    static MonsterEncyclopediaInfo getMonsterInfo(int monsterType);
    static TowerEncyclopediaInfo getTowerInfo(int towerType);

    static QVector<MonsterEncyclopediaInfo> getAllMonsterInfos();
    static QVector<TowerEncyclopediaInfo> getAllTowerInfos();
};
#endif // ENCYCLOPEDIADATA_H
