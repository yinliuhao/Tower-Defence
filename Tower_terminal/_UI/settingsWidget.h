#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

// Qt核心头文件
#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include "saveConflictWidget.h"  // 包含设置冲突界面类

class QKeyEvent;  // 前向声明

/**
 * @brief 游戏设置数据结构
 * 
 * 用于存储游戏的各种设置参数，包括音效、音乐、控制、显示和难度等
 */
struct GameSettings {
    int soundEffectVolume;  // 音效音量，范围0-100
    int musicVolume;        // 音乐音量，范围0-100
    bool invertYAxis;       // 是否反转Y轴控制
    bool showGrid;          // 是否显示网格（调试用）
    int difficulty;         // 游戏难度，范围1-3（1简单，2普通，3困难）
    
    // 玩家控制按键绑定
    int keyMoveUp;          // 向上移动按键
    int keyMoveDown;        // 向下移动按键
    int keyMoveLeft;        // 向左移动按键
    int keyMoveRight;       // 向右移动按键
    int keyRoll;            // 翻滚按键
    int keyCollect;         // 收集按键
    int keyResourcePanel;   // 资源面板按键
    int keyTowerPanel;      // 建筑面板按键
    
    /**
     * @brief 默认构造函数
     * 
     * 初始化默认设置：
     * - 音效音量：50
     * - 音乐音量：50
     * - 不反转Y轴
     * - 显示网格
     * - 普通难度（2）
     * - 默认按键：WASD + Shift + Space + Q + Tab
     */
    GameSettings() : 
        soundEffectVolume(50), 
        musicVolume(50), 
        invertYAxis(false), 
        showGrid(true), 
        difficulty(2),
        keyMoveUp(Qt::Key_W),
        keyMoveDown(Qt::Key_S),
        keyMoveLeft(Qt::Key_A),
        keyMoveRight(Qt::Key_D),
        keyRoll(Qt::Key_Shift),
        keyCollect(Qt::Key_Space),
        keyResourcePanel(Qt::Key_Q),
        keyTowerPanel(Qt::Key_Tab) {}
};

/**
 * @brief 设置界面类
 * 
 * 用于显示和管理游戏设置的用户界面，提供设置项的调整和保存功能
 */
class SettingsWidget : public QWidget
{
    Q_OBJECT  // Qt元对象宏，启用信号槽功能

public:
    /**
     * @brief 构造函数
     * 
     * @param parent 父窗口指针
     */
    explicit SettingsWidget(QWidget *parent = nullptr);
    
    /**
     * @brief 析构函数
     */
    ~SettingsWidget();
    
    /**
     * @brief 获取当前设置
     * 
     * @return 当前设置的GameSettings对象
     */
    GameSettings getSettings() const;
    
    /**
     * @brief 设置初始设置
     * 
     * @param settings 要设置的GameSettings对象
     */
    void setSettings(const GameSettings &settings);


    bool conflict();

  
        

private slots:
    /**
     * @brief 保存按钮点击事件处理
     * 
     * 当用户点击保存按钮时调用，获取当前UI设置并发送settingsSaved信号
     */
    void on_saveButton_clicked();
    
    /**
     * @brief 取消按钮点击事件处理
     * 
     * 当用户点击取消按钮时调用，直接关闭窗口不保存设置
     */
    void on_cancelButton_clicked();

signals:
    /**
     * @brief 设置保存信号
     * 
     * 当用户保存设置时发出，携带保存的GameSettings对象
     */
    void settingsSaved(const GameSettings &settings);

private:
    // 设置数据
    GameSettings currentSettings;  // 当前游戏设置
    
    // UI组件
    QSlider *soundEffectSlider;    // 音效音量滑块
    QSlider *musicSlider;          // 音乐音量滑块
    QPushButton *invertYAxisButton;// 反转Y轴按钮
    QPushButton *showGridButton;   // 显示网格按钮
    QPushButton *difficultyButton1;// 简单难度按钮
    QPushButton *difficultyButton2;// 普通难度按钮
    QPushButton *difficultyButton3;// 困难难度按钮
    QPushButton *saveButton;       // 保存按钮
    QPushButton *cancelButton;     // 取消按钮
    
    // 按键绑定UI组件
    QPushButton *keyMoveUpButton;      // 向上移动按键按钮
    QPushButton *keyMoveDownButton;    // 向下移动按键按钮
    QPushButton *keyMoveLeftButton;    // 向左移动按键按钮
    QPushButton *keyMoveRightButton;   // 向右移动按键按钮
    QPushButton *keyRollButton;        // 翻滚按键按钮
    QPushButton *keyCollectButton;     // 收集按键按钮
    QPushButton *keyResourcePanelButton;
    QPushButton *keyTowerPanelButton;
    QPushButton *waitingForKeyButton; // 当前等待按键输入的按钮（用于按键绑定）
    
    /**
     * @brief 初始化UI组件
     * 
     * 创建和配置所有UI组件，设置布局和连接信号槽
     */
    void initUI();
    
    /**
     * @brief 更新UI显示
     * 
     * 根据currentSettings更新所有UI组件的状态
     */
    void updateUI();
    
    /**
     * @brief 将Qt键码转换为字符串显示
     * 
     * @param key Qt键码
     * @return 按键名称字符串
     */
    QString keyToString(int key) const;
    
    /**
     * @brief 按键绑定按钮点击事件处理
     * 当用户点击按键绑定按钮时，进入等待按键输入状态
     */
    void onKeyButtonClicked();
    
    /**
     * @brief 键盘按键事件处理（用于捕获按键绑定）
     * 
     * @param event 键盘事件
     */
    void keyPressEvent(QKeyEvent *event) override;
    
    /**
     * @brief 检查按键冲突
     * 
     * 遍历所有按键绑定，检查是否有重复的按键
     * 如果有冲突，将冲突的按钮显示为红色
     */
    void checkKeyConflicts();


    SaveConflictWidget* saveConflictWidget;  // 设置冲突指针
};

#endif // SETTINGSWIDGET_H