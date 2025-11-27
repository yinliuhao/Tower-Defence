#ifndef PLAYERUI_H
#define PLAYERUI_H

#include <QWidget>
#include <vector>
#include <qlabel.h>
#include <qpixmap.h>

class PlayerUI : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerUI(QWidget *parent = nullptr);
    int getWidth()  {return width;}
    int getHeight()  {return height;}
    void itoa(int value, std::vector<int>& out);
    void add(int value, std::vector<int>& out);
    int atoi(const std::vector<int>& out);
    bool sub(int value, std::vector<int>& out);
    void initStatusBar(int numWid, int numHei, int statuWid, int statuHei);
    void loadStatuTextures();
    void loadNumTextures();
    void updateSglBar(const std::vector<int>& digits, int n);
    void updateUI(const std::vector<int>& health, const std::vector<int>& money, const std::vector<int>& wave);
    bool updateSglNum(std::vector<int>& digits, int num);
    void loadFrame();

private:
    std::vector<int> money;  //数组式金钱
    std::vector<int> health;  //数组式生命
    std::vector<int> wave;   //数组式波次

    int width;  //UI宽度
    int height;  //UI高度

    int maxDig;  //存储最高位数
    int maxNum;  //存储最大数值

    int statuBarNum;  //状态栏数目
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
};

#endif // PLAYERUI_H
