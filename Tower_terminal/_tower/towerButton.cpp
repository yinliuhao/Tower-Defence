#include "_tower/towerButton.h"
#include <QDebug>
#include "_others/utils.h"

TowerButton::TowerButton(QString str, TowerType type, QWidget *parent)
    : QPushButton{parent}, name(str), type(type)
{
    buttonSize = QSize(BTNWIDTH, BTNHEIGHT);
    setFixedSize(BTNWIDTH, BTNHEIGHT);
    setNormalPic();
}

//普通图片
void TowerButton::setNormalPic()
{
    QString path = ":/picture/towerButton/" + name + ".png";
    this->setIcon(QIcon(path));
    this->setIconSize(buttonSize);
}

//鼠标光标放置在按钮上 显示的图片
void TowerButton::setSelectPic()
{
    QString path = ":/picture/towerButton/select_" + name + ".png";
    this->setIcon(QIcon(path));
    this->setIconSize(buttonSize);
}

//资源不足时显示的图片
void TowerButton::setLackPic()
{
    QString path = ":/picture/towerButton/lack_" + name + ".png";
    this->setIcon(QIcon(path));
    this->setIconSize(buttonSize);
}

//光标进入函数
void TowerButton::enterEvent(QEnterEvent *)
{
    if(!isEnough){
        setLackPic();
    }
    else{
        setSelectPic();
    }
    //qDebug() << "鼠标进入了";
}

//光标离开函数
void TowerButton::leaveEvent(QEvent *)
{
    if(!isEnough){
        setLackPic();
    }
    else{
        setNormalPic();
    }
    //qDebug() << "鼠标出来了";
}

void TowerButton::setEnough(bool on)
{
    isEnough = on;
    if(!isEnough){
        setLackPic();
    }
    else
    {
        if(underMouse()){
            setSelectPic();
        }
        else{
            setNormalPic();
        }
    }
}
