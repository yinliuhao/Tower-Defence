#include "playerUI.h"
#include <qstring.h>
#include "utils.h"

//int值转换为vector
std::vector<int> PlayerUI::itov(int value)
{
    std::vector<int> result;
    result.resize(maxDig);
    if (value > maxNum) value = maxNum;
    for(int i = maxDig - 1; i >= 0; --i)
    {
        result[i] = value % 10;
        value /= 10;
    }
    return result;
}

//vector转换为int
int PlayerUI::vtoi(const std::vector<int> out)
{
    int result = 0;
    for(int i = 0; i < maxDig; ++i)
    {
        result = result * 10 + out[i];
    }
    return result;
}

void PlayerUI::add(int value, int index)
{
    switch(index)
    {
    case CAMPHEALTH:
        campHealth += value;
        updateSglBar(CAMPHEALTH);
        break;
    case PLAYERHEALTH:
        playerHealth += value;
        updateSglBar(PLAYERHEALTH);
        break;
    case MONEY:
        money += value;
        updateSglBar(MONEY);
        break;
    case WAVE:
        wave += value;
        updateSglBar(WAVE);
        break;
    case WOOD:
        wood += value;
        updateSglBar(WOOD);
        break;
    case STONE:
        stone += value;
        updateSglBar(STONE);
        break;
    case BRONZE:
        bronze += value;
        updateSglBar(BRONZE);
        break;
    case SILVER:
        silver += value;
        updateSglBar(SILVER);
        break;
    case GOLD:
        gold += value;
        updateSglBar(GOLD);
        break;
    }
}

bool PlayerUI::sub(int value, int index)
{
    switch(index)
    {
    case CAMPHEALTH:
        campHealth -= value;
        if(campHealth < 0)
        {
            emit gameOver();
            return false;
        }
        else
        {
            updateSglBar(CAMPHEALTH);
            emit campHurted();
        }
        break;
    case PLAYERHEALTH:
        playerHealth -= value;
        if(playerHealth < 0)
        {
            emit gameOver();
            return false;
        }
        else
        {
            updateSglBar(CAMPHEALTH);
            emit playerHurted();
        }
        break;
    case MONEY:
        money -= value;
        if(money < 0) return false;
        updateSglBar(MONEY);
        break;
    case WAVE:
        wave -= value;
        if(wave < 0) return false;
        updateSglBar(WAVE);
        break;
    case WOOD:
        wood -= value;
        if(wood < 0) return false;
        updateSglBar(WOOD);
        break;
    case STONE:
        stone -= value;
        if(stone < 0) return false;
        updateSglBar(STONE);
        break;
    case BRONZE:
        bronze -= value;
        if(bronze < 0) return false;
        updateSglBar(BRONZE);
        break;
    case SILVER:
        silver -= value;
        if(silver < 0) return false;
        updateSglBar(SILVER);
        break;
    case GOLD:
        gold -= value;
        if(gold < 0) return false;
        updateSglBar(GOLD);
        break;
    }
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
}

//将单个状态栏的显示数据与内存数据同步
void PlayerUI::updateSglBar(int index)
{
    std::vector<int> result;
    switch(index)
    {
    case 1:
        result = itov(campHealth);
        break;
    case 2:
        result = itov(money);
        break;
    case 3:
        result = itov(wave);
        break;
    }

    for (int i = 1; i < maxDig + 1; ++i)
    {
        int d = result[i - 1];
        statusBar[index][i]->setPixmap(numTextures[d]);
    }
}

//将所有状态栏的显示数据与内存数据同步
void PlayerUI::updateUI()
{
    updateSglBar(CAMPHEALTH);
    updateSglBar(MONEY);
    updateSglBar(WAVE);
}

PlayerUI::PlayerUI(QWidget *parent)
    : QWidget{parent},
    statuBarNum(BARNUM),
    money(INITMONEY),
    campHealth(INITCAMPHEALTH),
    playerHealth(INITPLAYERHEALTH),
    wave(INITWAVE),
    wood(INITWOOD),
    stone(INITSTONE),
    bronze(INITBRONZE),
    silver(INITSILVER),
    gold(INITGOLD),
    width(BARWIDTH),
    height(BARHEIGHT),
    maxDig(DIGIT)
{
    //初始化数值上限
    maxNum = 0;
    for(int i = 0; i <maxDig; i++)
    {
        maxNum = maxNum * 10 + 9;
    }

    //设置状态栏大小，初始化状态栏
    this->setFixedSize(width, height);
    initStatusBar(NUMWIDTH, NUMHEIGHT, STATUWIDTH, STATUHEIGHT);
    this->setStyleSheet("QWidget { background-color: black; }");
}
