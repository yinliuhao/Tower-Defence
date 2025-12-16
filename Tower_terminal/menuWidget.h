#ifndef MENUWIDGET_H
#define MENUWIDGET_H

// Qt核心头文件
#include <QWidget>
#include "settingsWidget.h"  // 包含设置界面类

QT_BEGIN_NAMESPACE
namespace Ui {
    class Widget;  // UI设计器生成的界面类前向声明
}
QT_END_NAMESPACE

/**
 * @brief 菜单界面类
 *
 * 游戏的主菜单界面，提供开始游戏、关闭游戏和设置等功能
 * 管理游戏的初始设置，并与设置界面进行交互
 */
    class Widget : public QWidget
{
    Q_OBJECT  // Qt元对象宏，启用信号槽功能

public:
    /**
     * @brief 构造函数
     *
     * 初始化菜单界面，设置UI组件和初始游戏设置
     *
     * @param parent 父窗口指针
     */
    Widget(QWidget* parent = nullptr);

    /**
     * @brief 析构函数
     *
     * 清理资源，释放UI和设置界面
     */
    ~Widget();

    /**
     * @brief 获取当前游戏设置
     *
     * 获取当前保存的游戏设置对象
     *
     * @return 当前游戏设置的GameSettings对象
     */
    GameSettings getGameSettings() const;

private slots:
    /**
     * @brief 开始游戏按钮点击事件处理
     *
     * 当用户点击开始游戏按钮时调用，创建并显示游戏主界面
     */
    void on_startButton_clicked();

    /**
     * @brief 关闭游戏按钮点击事件处理
     *
     * 当用户点击关闭游戏按钮时调用，关闭整个应用程序
     */
    void on_closeButton_clicked();

    /**
     * @brief 设置按钮点击事件处理
     *
     * 当用户点击设置按钮时调用，显示设置界面
     */
    void on_settingsButton_clicked();

    /**
     * @brief 设置保存事件处理
     *
     * 当用户在设置界面保存设置时调用，更新当前游戏设置
     *
     * @param settings 保存的游戏设置对象
     */
    void onSettingsSaved(const GameSettings& settings);

private:
    Ui::Widget* ui;  // UI设计器生成的界面指针

    SettingsWidget* settingsWidget;  // 设置界面指针

    GameSettings gameSettings;  // 当前游戏设置
};

#endif // MENUWIDGET_H
