#include "playerUI.h"
#include <qstring.h>
#include "utils.h"
#include <QRandomGenerator>

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

void PlayerUI::add(int value, UiResource index)
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

bool PlayerUI::sub(int value, UiResource index)
{
    if(index ==  CAMPHEALTH)
    {
        if(campHealth <= value)
        {
            emit gameOver();
            return false;
        }
        else
        {
            campHealth -= value;
            updateSglBar(CAMPHEALTH);
            emit campHurted();
        }
    }
    else if(index == PLAYERHEALTH)
    {
        if(playerHealth <= value)
        {

            emit gameOver();
            return false;
        }
        else
        {
            playerHealth -= value;
            updateSglBar(PLAYERHEALTH);
            emit playerHurted();
        }
    }
    else if(index == WAVE)
    {
        if(wave < value) return false;
        wave -= value;
        updateSglBar(WAVE);
    }
    else if(index == WOOD)
    {
        if(wood < value) return false;
        wood -= value;
        updateSglBar(WOOD);
    }
    else if(index == STONE)
    {
        if(stone < value) return false;
        stone -= value;
        updateSglBar(STONE);
    }
    else if(index == BRONZE)
    {
        bronze -= value;
        if(bronze < value) return false;
        updateSglBar(BRONZE);
    }
    else if(index == SILVER)
    {
        if(silver < value) return false;
        silver -= value;
        updateSglBar(SILVER);
    }
    else if(index == GOLD)
    {
        if(gold < value) return false;
        gold -= value;
        updateSglBar(GOLD);
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
    statuTextures.resize(statuBarNum);
    statuTextures[0].load(":/picture/UI/campHealth.png");
    statuTextures[1].load(":/picture/UI/playerHealth.png");
    statuTextures[2].load(":/picture/UI/wave.png");
    statuTextures[3].load(":/picture/UI/wood.png");
    statuTextures[4].load(":/picture/UI/bronze.png");
    statuTextures[5].load(":/picture/UI/silver.png");
    statuTextures[6].load(":/picture/UI/gold.png");
    statuTextures[7].load(":/picture/UI/stone.png");
}

//存储UI边框图片
void PlayerUI::loadFrame()
{
    picFrame.load(":/picture/UI/uiFrame.png");
    frame = new QLabel(this);
    frame->setPixmap(picFrame);
    frame->setFixedSize(width, height);
    frame->setScaledContents(true);
    frame->raise();
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
    for(int i = 0; i < statuBarNum / 2; i++)
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

    for(int i = statuBarNum / 2; i < statuBarNum; i++)
    {
        statusBar[i].resize(maxDig + 1);
        QLabel* statu = new QLabel(this);
        statu->setPixmap(statuTextures[i]);
        statu->setFixedSize(statuWid, statuHei);
        statu->setScaledContents(true);
        statusBar[i][0] = statu;
        statusBar[i][0]->move(UIWIDTH / 2.0, STATUY + GAPY * (i - statuBarNum / 2));

        for (int j = 0; j < maxDig; ++j)
        {
            QLabel* lbl = new QLabel(this);
            lbl->setPixmap(numTextures[0]);
            lbl->setFixedSize(numWid, numHei);
            lbl->setScaledContents(true);
            statusBar[i][j + 1] = lbl;
            statusBar[i][j + 1]->move(UIWIDTH / 2.0 + NUMX - STATUX + NUMGAPX * j, NUMY + GAPY * (i - statuBarNum / 2));
        }
    }
}

//将单个状态栏的显示数据与内存数据同步
void PlayerUI::updateSglBar(UiResource index)
{
    std::vector<int> result;
    switch(index)
    {
    case CAMPHEALTH:
        result = itov(campHealth);
        break;
    case PLAYERHEALTH:
        result = itov(playerHealth);
        break;
    case WAVE:
        result = itov(wave);
        break;
    case WOOD:
        result = itov(wood);
        break;
    case STONE:
        result = itov(stone);
        break;
    case BRONZE:
        result = itov(bronze);
        break;
    case SILVER:
        result = itov(silver);
        break;
    case GOLD:
        result = itov(gold);
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
    updateSglBar(PLAYERHEALTH);
    updateSglBar(WAVE);
    updateSglBar(STONE);
    updateSglBar(WOOD);
    updateSglBar(GOLD);
    updateSglBar(SILVER);
    updateSglBar(BRONZE);
}

PlayerUI::PlayerUI(QWidget *parent)
    : QWidget{parent},
    statuBarNum(BARNUM),
    campHealth(INITCAMPHEALTH),
    playerHealth(INITPLAYERHEALTH),
    wave(INITWAVE),
    wood(INITWOOD),
    stone(INITSTONE),
    bronze(INITBRONZE),
    silver(INITSILVER),
    gold(INITGOLD),
    width(UIWIDTH),
    height(UIHEIGHT),
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

    updateUI();
}

void PlayerUI::collectResource(ResourceType type)
{
    QRandomGenerator* random = QRandomGenerator::global();
    int result;
    if(type == ResourceType::GRASS_TREE)
    {
        result = random->bounded(100);
        if(result  >= 0 && result < 60)
            add(2, WOOD);
        else if(result >= 60 && result < 90)
            add(3, WOOD);
        else if(result >= 90)
            add(4, WOOD);
    }
    else if(type == ResourceType::SWAMP_TREE)
    {
        result = random->bounded(100);
        if(result >= 0 && result < 10)
            add(1, WOOD);
        else if(result  >= 10 && result < 80)
            add(2, WOOD);
        else if(result >= 80 && result < 95)
            add(3, WOOD);
        else if(result >= 95)
            add(4, WOOD);
    }
    else if(type == ResourceType::GRASS_STONE)
    {
        result = random->bounded(100);
        if(result >= 0 && result < 60)
            add(2, STONE);
        else if(result  >= 60 && result < 80)
            add(3, STONE);
        else if(result >= 80 && result < 90)
        {
            add(2, WOOD);
            add(1, BRONZE);
        }
        else if(result >= 90 && result < 96)
        {
            add(2, WOOD);
            add(1, SILVER);
        }
        else if(result >= 96)
        {
            add(2, WOOD);
            add(1, GOLD);
        }
    }
    else if(type == ResourceType::SNOW_STONE)
    {
        result = random->bounded(100);
        if(result >= 0 && result < 50)
            add(2, STONE);
        else if(result  >= 50 && result < 70)
            add(3, STONE);
        else if(result >= 70 && result < 90)
        {
            add(4, WOOD);
        }
        else if(result >= 90 && result < 94)
        {
            add(2, WOOD);
            add(1, BRONZE);
        }
        else if(result >= 94 && result < 97)
        {
            add(2, WOOD);
            add(1, SILVER);
        }
        else if(result >= 97 && result < 100)
        {
            add(2, WOOD);
            add(1, GOLD);
        }
    }
    else if(type == ResourceType::SWAMP_STONE)
    {
        result = random->bounded(100);
        if(result >= 0 && result < 50)
            add(2, STONE);
        else if(result  >= 50 && result < 60)
            add(3, STONE);
        else if(result >= 60 && result < 65)
        {
            add(4, WOOD);
        }
        else if(result >= 65 && result < 80)
        {
            add(2, WOOD);
            add(1, BRONZE);
        }
        else if(result >= 80 && result < 92)
        {
            add(2, WOOD);
            add(1, SILVER);
        }
        else if(result >= 92 && result < 100)
        {
            add(2, WOOD);
            add(1, GOLD);
        }
    }
}
