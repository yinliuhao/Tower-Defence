#ifndef TOWERBUTTON_H
#define TOWERBUTTON_H

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QSize>
#include <QEvent>
#include <QEnterEvent>

class TowerButton : public QPushButton
{
    Q_OBJECT
public:
    explicit TowerButton(QString str, QWidget *parent = nullptr);

    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *) override;
    QSize getSize()  {return buttonSize;}

protected:
    void setNormalPic();
    void setSelectPic();
    void setLackPic();

private:
    QString name;       //炮塔名字
    QSize buttonSize;     //炮塔按钮的宽高

signals:
};

#endif // TOWERBUTTON_H
