#ifndef ENCYCLOPEDIADIALOG_H
#define ENCYCLOPEDIADIALOG_H
#pragma once
#include <QDialog>
#include <QTabWidget>
#include <QListWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include "EncyclopediaData.h"

class EncyclopediaDialog : public QDialog {
    Q_OBJECT

public:
    EncyclopediaDialog(QWidget *parent = nullptr);

private slots:
    void onMonsterItemClicked(QListWidgetItem *item);
    void onTowerItemClicked(QListWidgetItem *item);

private:
    void setupUI();
    void setupMonsterTab();
    void setupTowerTab();
    void showMonsterDetails(const MonsterEncyclopediaInfo &info);
    void showTowerDetails(const TowerEncyclopediaInfo &info);

    QTabWidget *tabWidget;

    // 怪物标签页组件
    QWidget *monsterTab;
    QListWidget *monsterList;
    QWidget *monsterDetails;
    QLabel *monsterImageLabel;
    QLabel *monsterNameLabel;
    QLabel *monsterDescriptionLabel;
    QLabel *monsterStatsLabel;
    QLabel *monsterSpecialAbilityLabel;

    // 炮塔标签页组件
    QWidget *towerTab;
    QListWidget *towerList;
    QWidget *towerDetails;
    QLabel *towerImageLabel;
    QLabel *towerNameLabel;
    QLabel *towerDescriptionLabel;
    QLabel *towerStatsLabel;
    QLabel *towerSpecialAbilityLabel;
};
#endif // ENCYCLOPEDIADIALOG_H
