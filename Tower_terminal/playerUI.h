#ifndef PLAYERUI_H
#define PLAYERUI_H

#include <QWidget>
#include <vector>
#include <qlabel.h>
#include <qpixmap.h>
#include "resource.h"

enum UiResource
{
    CAMPHEALTH = 0,
    PLAYERHEALTH = 1,
    WAVE = 2,
    WOOD = 3,
    BRONZE = 4,
    SILVER = 5,
    GOLD = 6,
    STONE = 7,
};


class PlayerUI : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerUI(QWidget *parent = nullptr);
    int getWidth()  {return width;}
    int getHeight()  {return height;}
    std::vector<int> itov(int value);
    int vtoi(const std::vector<int> out);
    void add(int value, UiResource index);
    bool sub(int value, UiResource index);
    void initStatusBar(int numWid, int numHei, int statuWid, int statuHei);
    void loadStatuTextures();
    void loadNumTextures();
    void updateSglBar(UiResource index);
    void updateUI();
    void loadFrame();

public slots:
    void collectResource(ResourceType type);

private:
    int statuBarNum;  //状态栏数目

    int campHealth;  //基地生命
    int playerHealth;   //玩家生命
    int wave;   //波次
    int wood;   //木材数量
    int stone;   //石材数量
    int bronze;  //铜锭数量
    int silver;  //银锭数量
    int gold;   //金锭数量
    int resin;  //树脂数量

    int width;  //UI宽度
    int height;  //UI高度

    int maxDig;  //存储最高位数
    int maxNum;  //存储最大数值


    std::vector<std::vector<QLabel*>> statusBar;    //存储状态栏标签
    QPixmap picFrame;  //存储边框图
    QLabel* frame;   //存储边框标签
    int numWidth;    //数字图片宽度
    int numHeight;   //数字图片高度
    int statuWidth;  //状态栏头 宽度
    int statuHeight;  //状态栏头 高度
    std::vector<QPixmap> numTextures;   //存储数字图片
    std::vector<QPixmap> statuTextures;   //存储状态栏头图片

signals:
    void gameOver();
    void campHurted();
    void playerHurted();
};

#endif // PLAYERUI_H
