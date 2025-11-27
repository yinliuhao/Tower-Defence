#include "playerUI.h"
#include <qstring.h>
#include "utils.h"

//由于波次的结构为 num num ‘/’ num num
//其他状态栏的结构为num num num num num num
//后续没有为波次的初始化，内存数据更新，显示数据同步提供新的算法


//int值转换为array【5】
void PlayerUI::itoa(int value, std::vector<int>& out)
{
    if (int(out.size()) != maxDig) return;
    if (value < 0) value = 0;
    if (value > maxNum) value = maxNum;
    for(int i = maxDig - 1; i >= 0; --i)
    {
        out[i] = value % 10;
        value /= 10;
    }
}

//array【5】转换为int
int PlayerUI::atoi(const std::vector<int>& out)
{
    int result = 0;
    for(int i = 0; i < maxDig; ++i)
    {
        result = result * 10 + out[i];
    }
    return result;
}

//arra有【5】高精度加法
void PlayerUI::add(int value, std::vector<int>& out)
{
    if (int(out.size()) != maxDig) return;
    int result = atoi(out) + value;
    itoa(result, out);
}

//array【5】高精度减法
bool PlayerUI::sub(int value, std::vector<int>& out)
{
    if (int(out.size()) != maxDig) return false;
    int result = atoi(out) - value;
    if(result < 0) return false;
    itoa(result, out);
    return true;
}

//存储数字和‘/’图片
void PlayerUI::loadNumTextures()
{
    numTextures.resize(11);
    for (int i = 0; i < 10; ++i)
    {
        numTextures[i].load(QString(":/picture/UI/%1.png").arg(i));
    }
    numTextures[10].load(QString(":/picture/UI/slash.png"));
}

//存储状态（生命，金钱，波次）图片
void PlayerUI::loadStatuTextures()
{
    statuTextures.resize(3);
    statuTextures[0].load(":/picture/UI/health.png");
    statuTextures[1].load(":/picture/UI/money.png");
    statuTextures[2].load(":/picture/UI/wave.png");
}

//存储UI边框图片
void PlayerUI::loadFrame()
{
    picFrame.load(":/picture/UI/uiFrame.png");
    frame = new QLabel(this);
    frame->setPixmap(picFrame);
    frame->setFixedSize(width, height);
    frame->setScaledContents(true);
}

//初始化整个UI界面
void PlayerUI::initStatusBar(int numWid, int numHei, int statuWid, int statuHei)
{
    //加载边框图片，设置边框图片位置
    loadFrame();
    frame->move(0, 0);

    //加载数字图片，设置数字图片位置
    //加载状态头图片，设置状态头图片位置
    loadNumTextures();
    loadStatuTextures();

    statusBar.resize(statuBarNum);
    for(int i = 0; i < statuBarNum; i++)
    {
        statusBar[i].resize(maxDig + 1);
        QLabel* statu = new QLabel(this);
        statu->setPixmap(statuTextures[i]);
        statu->setFixedSize(statuWid, statuHei);
        statu->setScaledContents(true);
        statusBar[i][0] = statu;
        statusBar[i][0]->move(STATUX, STATUY + GAPY * i);

        for (int j = 0; j < maxDig; ++j)
        {
            QLabel* lbl = new QLabel(this);
            lbl->setPixmap(numTextures[0]);
            lbl->setFixedSize(numWid, numHei);
            lbl->setScaledContents(true);
            statusBar[i][j + 1] = lbl;
            statusBar[i][j + 1]->move(NUMX + NUMGAPX * j, NUMY + GAPY * i);
        }
    }

    //更改wave状态栏的中间数字为‘/’
    delete statusBar[2][3];
    QLabel* lbl = new QLabel(this);
    lbl->setPixmap(numTextures[10]);
    lbl->setFixedSize(numWid, numHei);
    lbl->setScaledContents(true);
    statusBar[2][3] = lbl;
    statusBar[2][3]->move(NUMX + NUMGAPX * 2, NUMY + GAPY * 2);
}

//将单个状态栏的显示数据与内存数据同步
void PlayerUI::updateSglBar(const std::vector<int>& digits, int n)
{
    for (int i = 1; i < maxDig + 1; ++i)
    {
        int d = digits[i - 1];
        statusBar[n][i]->setPixmap(numTextures[d]);
    }
}

//更新单个显示栏的内存数据
bool PlayerUI::updateSglNum(std::vector<int>& digits, int num)
{
    if(num > 0)
    {
        add(num, digits);
        return true;
    }
    else
    {
        return sub(num, digits);
    }
}

//将所有状态栏的显示数据与内存数据同步
void PlayerUI::updateUI(const std::vector<int>& health, const std::vector<int>& money, const std::vector<int>& wave)
{
    updateSglBar(health, 0);
    updateSglBar(money, 1);
    updateSglBar(wave, 2);
}

PlayerUI::PlayerUI(QWidget *parent)
    : QWidget{parent},
    width(BARWIDTH),
    height(BARHEIGHT),
    maxDig(DIGIT),
    statuBarNum(BARNUM)
{
    //初始化数值上限
    maxNum = 0;
    for(int i = 0; i <maxDig; i++)
    {
        maxNum = maxNum * 10 + 9;
    }

    //初始化数组数据
    money.resize(maxDig);
    health.resize(maxDig);
    this->wave.resize(maxDig);
    itoa(INITMONEY, money);
    itoa(INITHEALTH, health);
    itoa(INITWAVE, this->wave);

    //设置状态栏大小，初始化状态栏
    this->setFixedSize(width, height);
    initStatusBar(NUMWIDTH, NUMHEIGHT, STATUWIDTH, STATUHEIGHT);
    this->setStyleSheet("QWidget { background-color: black; }");
}
