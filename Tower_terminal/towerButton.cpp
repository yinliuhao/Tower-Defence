#include "towerButton.h"
#include <QDebug>
#include "utils.h"

TowerButton::TowerButton(QString str, QWidget *parent)
    : QPushButton{parent}, name(str)
{
    buttonSize = QSize(BTNWIDTH, BTNHEIGHT);
    setFixedSize(BTNWIDTH, BTNHEIGHT);
    setNormalPic();
}

//普通图片
void TowerButton::setNormalPic()
{
    QString path = ":/picture/Tower/" + name + ".png";
    this->setIcon(QIcon(path));
    this->setIconSize(buttonSize);
}

//鼠标光标放置在按钮上 显示的图片
void TowerButton::setSelectPic()
{
    QString path = ":/picture/Tower/select_" + name + ".png";
    this->setIcon(QIcon(path));
    this->setIconSize(buttonSize);
}

//资源不足时显示的图片
void TowerButton::setLackPic()
{
    QString path = ":/picture/Tower/lack_" + name + ".png";
    this->setIcon(QIcon(path));
    this->setIconSize(buttonSize);
}

//光标进入函数
void TowerButton::enterEvent(QEnterEvent *)
{
    setSelectPic();
    qDebug() << "鼠标进入了";
}

//光标离开函数
void TowerButton::leaveEvent(QEvent *)
{
    setNormalPic();
    qDebug() << "鼠标出来了";
}
