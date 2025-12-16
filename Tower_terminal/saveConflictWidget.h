#pragma once
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QMessageBox>

class QKeyEvent;  // 前向声明

class SaveConflictWidget: public QWidget
{
    Q_OBJECT  // Qt元对象宏，启用信号槽功能
public:
    
    
    explicit SaveConflictWidget(QWidget* parent = nullptr) {

        this->setWindowTitle("SaveConflict!");

        // 初始化UI组件
        initUI();
    }

    void initUI() {

        helpText = new QLabel(this);
        helpText->setText(
            "\n"
            "        你的按键设置存在冲突！\n"
            "        请检查亮红色的设置按键\n"
            "        按右上角的叉叉关闭次窗口\n"
            "\n"
        );
        helpText->setAlignment(Qt::AlignCenter);
        helpText->setWordWrap(true);

        // 设置窗口布局和固定大小
       // setLayout(mainLayout);
        setFixedSize(200, 90);  

    }

    ~SaveConflictWidget() {};

 

public slots:
    
    void on_cancelButton_clicked()
    {
        emit on_cancelButton_clicked();

        // 关闭设置窗口
        this->close();
       
    }
private:
    QPushButton* acceptButton;      // 向上移动按键按钮
    QLabel* helpText;
};




