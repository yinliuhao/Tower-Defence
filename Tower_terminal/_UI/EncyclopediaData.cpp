#include "_UI/EncyclopediaData.h"

MonsterEncyclopediaInfo EncyclopediaData::getMonsterInfo(int monsterType) {
    MonsterEncyclopediaInfo info;

    switch(monsterType) {
    case 1: // MONSTER1
        info.name = "普通怪物";
        info.description = "基础怪物类型，移动速度中等，生命值较低";
        info.health = 100; // MONSTER1_HEALTH
        info.speed = 2;    // MONSTER1_SPEED
        info.goldReward = 10; // MONSTER1_GOLD
        info.attackDamage = 5; // MONSTER1_DAMAGE
        info.imagePath = ":/picture/monster1/(1).png";
        info.specialAbility = "无特殊能力";
        break;

    case 2: // MONSTER2
        info.name = "快速怪物";
        info.description = "移动速度较快，但生命值较低";
        info.health = 80;  // MONSTER2_HEALTH
        info.speed = 4;    // MONSTER2_SPEED
        info.goldReward = 15; // MONSTER2_GOLD
        info.attackDamage = 3; // MONSTER2_DAMAGE
        info.imagePath = ":/picture/monster2/(1).png";
        info.specialAbility = "移动速度快";
        break;

    case 3: // MONSTER3
        info.name = "重型怪物";
        info.description = "生命值较高，但移动速度较慢";
        info.health = 200; // MONSTER3_HEALTH
        info.speed = 1;    // MONSTER3_SPEED
        info.goldReward = 25; // MONSTER3_GOLD
        info.attackDamage = 8; // MONSTER3_DAMAGE
        info.imagePath = ":/picture/monster3/(1).png";
        info.specialAbility = "高生命值";
        break;

    case 4: // MONSTER4 - 新增第四种怪物
        info.name = "魔法怪物";
        info.description = "具有魔法抗性的怪物，对魔法攻击有减免";
        info.health = 150; // MONSTER4_HEALTH
        info.speed = 3;    // MONSTER4_SPEED
        info.goldReward = 20; // MONSTER4_GOLD
        info.attackDamage = 6; // MONSTER4_DAMAGE
        info.imagePath = ":/picture/monster4/(1).png";
        info.specialAbility = "魔法抗性";
        break;

    default:
        info.name = "未知怪物";
        info.description = "未知怪物类型";
        info.health = 0;
        info.speed = 0;
        info.goldReward = 0;
        info.attackDamage = 0;
        info.imagePath = "";
        info.specialAbility = "无";
    }

    return info;
}

TowerEncyclopediaInfo EncyclopediaData::getTowerInfo(int towerType) {
    TowerEncyclopediaInfo info;

    switch(towerType) {
    case 1: // TOWER1
        info.name = "箭塔";
        info.description = "基础防御塔，攻击速度中等，伤害较低";
        info.health = 100; // TOWER1_MAHEALTH
        info.buildCost = 50; // TOWER1_BUILCOST
        info.attackRange = 150; // TOWER1_ATTACKRANGE
        info.attackInterval = 1000; // TOWER1_ATTACKINTERVAL
        info.bulletDamage = 10;
        info.bulletSpeed = 8;
        info.imagePath = ":/picture/tower/archer_level1_frame1.png";
        info.specialAbility = "基础攻击";
        break;

    case 2: // TOWER2
        info.name = "魔法塔";
        info.description = "中级防御塔，攻击范围较大，伤害中等";
        info.health = 150; // TOWER2_MAHEALTH
        info.buildCost = 100; // TOWER2_BUILCOST
        info.attackRange = 200; // TOWER2_ATTACKRANGE
        info.attackInterval = 1500; // TOWER2_ATTACKINTERVAL
        info.bulletDamage = 15;
        info.bulletSpeed = 10;
        info.imagePath =  ":/picture/tower/cannon_level1.png";
        info.specialAbility = "魔法攻击";
        break;

    case 3: // TOWER3
        info.name = "炮塔";
        info.description = "高级防御塔，攻击力强，但攻击速度较慢";
        info.health = 200; // TOWER3_MAHEALTH
        info.buildCost = 200; // TOWER3_BUILCOST
        info.attackRange = 180; // TOWER3_ATTACKRANGE
        info.attackInterval = 2000; // TOWER3_ATTACKINTERVAL
        info.bulletDamage = 20;
        info.bulletSpeed = 12;
        info.imagePath = ":/picture/tower/mortar_level1.png";
        info.specialAbility = "高伤害攻击";
        break;

    default:
        info.name = "未知炮塔";
        info.description = "未知炮塔类型";
        info.health = 0;
        info.buildCost = 0;
        info.attackRange = 0;
        info.attackInterval = 0;
        info.bulletDamage = 0;
        info.bulletSpeed = 0;
        info.imagePath = "";
        info.specialAbility = "无";
    }

    return info;
}

QVector<MonsterEncyclopediaInfo> EncyclopediaData::getAllMonsterInfos() {
    QVector<MonsterEncyclopediaInfo> infos;
    for(int i = 1; i <= 4; i++) { // 修改为4，包含所有四种怪物
        infos.append(getMonsterInfo(i));
    }
    return infos;
}

QVector<TowerEncyclopediaInfo> EncyclopediaData::getAllTowerInfos() {
    QVector<TowerEncyclopediaInfo> infos;
    for(int i = 1; i <= 3; i++) {
        infos.append(getTowerInfo(i));
    }
    return infos;
}
