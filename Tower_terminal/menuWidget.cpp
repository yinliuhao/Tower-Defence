#include "menuWidget.h"
#include "ui_menuWidget.h"
#include "gameWidght.h"

 Widget:: Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui:: Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("Menu");

    //放置图片
    ui->menuLable->setPixmap(QPixmap("://picture//menuBackground//menuBackground.png"));

    //设置按钮边框样式
    ui->startButton->setStyleSheet(
        "QPushButton {"
        "    background: transparent;"
        "    border: 8px solid black;"
        "    color: black;"
        "}"
        );

    ui->closeButton->setStyleSheet(
        "QPushButton {"
        "    background: transparent;"
        "    border: 8px solid black;"
        "    color: black;"
        "}"
        );

    //设置界面标题
    ui->themeLabel->setStyleSheet("color: black;");

}

 Widget::~ Widget()
{
    delete ui;
}

//start按钮槽函数
void  Widget::on_startButton_clicked()
{
    this->close();
    Game *g = new Game();
    g->show();
}

//close按钮槽函数
void  Widget::on_closeButton_clicked()
{
    this->close();
}
