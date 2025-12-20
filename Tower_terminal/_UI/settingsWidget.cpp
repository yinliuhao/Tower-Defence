#include "_UI/settingsWidget.h"
#include <QDebug>
#include <QKeyEvent>
#include <QKeySequence>

/**
 * @brief SettingsWidget构造函数
 * 
 * 初始化设置窗口，设置标题并加载UI组件
 * 
 * @param parent 父窗口指针
 */
SettingsWidget::SettingsWidget(QWidget *parent) :
    QWidget(parent)
{
    // 设置窗口标题
    this->setWindowTitle("Settings");
    
    // 初始化UI组件
    initUI();
    
    // 根据当前设置更新UI显示
    updateUI();
}

/**
 * @brief SettingsWidget析构函数
 * 
 * 清理资源，由于所有UI组件都是this的子对象，会被自动释放
 */
SettingsWidget::~SettingsWidget()
{
    // Qt的父子对象机制会自动处理所有子对象的释放
}

/**
 * @brief 初始化UI组件
 * 
 * 创建并配置所有设置界面的UI组件，包括布局、滑块、按钮等
 * 将各个组件组织成合理的界面结构
 */
void SettingsWidget::initUI()
{
    // 创建主布局，设置为当前窗口的布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 创建音效设置组（占位符 - 待实现）
    QGroupBox *soundGroup = new QGroupBox("Sound Settings (Placeholder)", this);
    QVBoxLayout *soundLayout = new QVBoxLayout(soundGroup);
    
    // 创建音效音量控制（占位符）
    QHBoxLayout *soundEffectLayout = new QHBoxLayout();
    QLabel *soundEffectLabel = new QLabel("Sound Effects:", this);
    soundEffectSlider = new QSlider(Qt::Horizontal, this);
    soundEffectSlider->setRange(0, 100);  // 设置音量范围0-100
    soundEffectLayout->addWidget(soundEffectLabel);
    soundEffectLayout->addWidget(soundEffectSlider);
    soundLayout->addLayout(soundEffectLayout);
    
    // 创建音乐音量控制（占位符）
    QHBoxLayout *musicLayout = new QHBoxLayout();
    QLabel *musicLabel = new QLabel("Music:", this);
    musicSlider = new QSlider(Qt::Horizontal, this);
    musicSlider->setRange(0, 100);  // 设置音量范围0-100
    musicLayout->addWidget(musicLabel);
    musicLayout->addWidget(musicSlider);
    soundLayout->addLayout(musicLayout);
    
    // 将音效设置组添加到主布局
    mainLayout->addWidget(soundGroup);
    
    // 创建控制设置组
    QGroupBox *controlGroup = new QGroupBox("Control Settings", this);
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    
    // 创建反转Y轴控制按钮
    invertYAxisButton = new QPushButton("Invert Y Axis", this);
    invertYAxisButton->setCheckable(true);  // 设置为可复选按钮
    controlLayout->addWidget(invertYAxisButton);
    
    // 添加按键绑定设置
    QLabel *keyBindingsLabel = new QLabel("Key Bindings:", this);
    controlLayout->addWidget(keyBindingsLabel);
    
    // 创建按键绑定按钮
    QHBoxLayout *keyUpLayout = new QHBoxLayout();
    keyUpLayout->addWidget(new QLabel("Move Up:", this));
    keyMoveUpButton = new QPushButton(this);
    keyMoveUpButton->setMinimumWidth(100);
    connect(keyMoveUpButton, &QPushButton::clicked, this, &SettingsWidget::onKeyButtonClicked);
    keyUpLayout->addWidget(keyMoveUpButton);
    controlLayout->addLayout(keyUpLayout);
    
    QHBoxLayout *keyDownLayout = new QHBoxLayout();
    keyDownLayout->addWidget(new QLabel("Move Down:", this));
    keyMoveDownButton = new QPushButton(this);
    keyMoveDownButton->setMinimumWidth(100);
    connect(keyMoveDownButton, &QPushButton::clicked, this, &SettingsWidget::onKeyButtonClicked);
    keyDownLayout->addWidget(keyMoveDownButton);
    controlLayout->addLayout(keyDownLayout);
    
    QHBoxLayout *keyLeftLayout = new QHBoxLayout();
    keyLeftLayout->addWidget(new QLabel("Move Left:", this));
    keyMoveLeftButton = new QPushButton(this);
    keyMoveLeftButton->setMinimumWidth(100);
    connect(keyMoveLeftButton, &QPushButton::clicked, this, &SettingsWidget::onKeyButtonClicked);
    keyLeftLayout->addWidget(keyMoveLeftButton);
    controlLayout->addLayout(keyLeftLayout);
    
    QHBoxLayout *keyRightLayout = new QHBoxLayout();
    keyRightLayout->addWidget(new QLabel("Move Right:", this));
    keyMoveRightButton = new QPushButton(this);
    keyMoveRightButton->setMinimumWidth(100);
    connect(keyMoveRightButton, &QPushButton::clicked, this, &SettingsWidget::onKeyButtonClicked);
    keyRightLayout->addWidget(keyMoveRightButton);
    controlLayout->addLayout(keyRightLayout);
    
    QHBoxLayout *keyRollLayout = new QHBoxLayout();
    keyRollLayout->addWidget(new QLabel("Roll:", this));
    keyRollButton = new QPushButton(this);
    keyRollButton->setMinimumWidth(100);
    connect(keyRollButton, &QPushButton::clicked, this, &SettingsWidget::onKeyButtonClicked);
    keyRollLayout->addWidget(keyRollButton);
    controlLayout->addLayout(keyRollLayout);
    
    QHBoxLayout *keyCollectLayout = new QHBoxLayout();
    keyCollectLayout->addWidget(new QLabel("Collect:", this));
    keyCollectButton = new QPushButton(this);
    keyCollectButton->setMinimumWidth(100);
    connect(keyCollectButton, &QPushButton::clicked, this, &SettingsWidget::onKeyButtonClicked);
    keyCollectLayout->addWidget(keyCollectButton);
    controlLayout->addLayout(keyCollectLayout);

    QHBoxLayout *keyResourceLayout = new QHBoxLayout();
    keyResourceLayout->addWidget(new QLabel("Resource Panel:", this));
    keyResourcePanelButton = new QPushButton(this);
    keyResourcePanelButton->setMinimumWidth(100);
    connect(keyResourcePanelButton, &QPushButton::clicked, this, &SettingsWidget::onKeyButtonClicked);
    keyResourceLayout->addWidget(keyResourcePanelButton);
    controlLayout->addLayout(keyResourceLayout);

    QHBoxLayout *keyTowerLayout = new QHBoxLayout();
    keyTowerLayout->addWidget(new QLabel("Tower Panel:", this));
    keyTowerPanelButton = new QPushButton(this);
    keyTowerPanelButton->setMinimumWidth(100);
    connect(keyTowerPanelButton, &QPushButton::clicked, this, &SettingsWidget::onKeyButtonClicked);
    keyTowerLayout->addWidget(keyTowerPanelButton);
    controlLayout->addLayout(keyTowerLayout);
    
    waitingForKeyButton = nullptr;  // 初始化为空，表示没有等待按键输入
    
    // 将控制设置组添加到主布局
    mainLayout->addWidget(controlGroup);
    
    // 创建显示设置组
    QGroupBox *displayGroup = new QGroupBox("Display Settings", this);
    QVBoxLayout *displayLayout = new QVBoxLayout(displayGroup);
    
    // 创建显示网格控制按钮
    showGridButton = new QPushButton("Show Grid", this);
    showGridButton->setCheckable(true);  // 设置为可复选按钮
    displayLayout->addWidget(showGridButton);
    
    // 将显示设置组添加到主布局
    mainLayout->addWidget(displayGroup);
    
    // 创建难度设置组（占位符 - 待实现）
    QGroupBox *difficultyGroup = new QGroupBox("Difficulty (Placeholder)", this);
    QHBoxLayout *difficultyLayout = new QHBoxLayout(difficultyGroup);
    
    // 创建难度按钮
    difficultyButton1 = new QPushButton("Easy", this);
    difficultyButton1->setCheckable(true);
    difficultyButton2 = new QPushButton("Normal", this);
    difficultyButton2->setCheckable(true);
    difficultyButton3 = new QPushButton("Hard", this);
    difficultyButton3->setCheckable(true);
    
    // 创建按钮组，确保难度按钮互斥
    QButtonGroup *difficultyGroupBtn = new QButtonGroup(this);
    difficultyGroupBtn->addButton(difficultyButton1, 1);  // 简单难度
    difficultyGroupBtn->addButton(difficultyButton2, 2);  // 普通难度
    difficultyGroupBtn->addButton(difficultyButton3, 3);  // 困难难度
    
    // 将难度按钮添加到布局
    difficultyLayout->addWidget(difficultyButton1);
    difficultyLayout->addWidget(difficultyButton2);
    difficultyLayout->addWidget(difficultyButton3);
    
    // 将难度设置组添加到主布局
    mainLayout->addWidget(difficultyGroup);
    
    // 创建底部按钮布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    
    // 创建保存按钮并连接信号槽
    saveButton = new QPushButton("Save", this);
    connect(saveButton, &QPushButton::clicked, this, &SettingsWidget::on_saveButton_clicked);
    
    // 创建取消按钮并连接信号槽
    cancelButton = new QPushButton("Cancel", this);
    connect(cancelButton, &QPushButton::clicked, this, &SettingsWidget::on_cancelButton_clicked);
    
    // 将按钮添加到底部布局
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    
    // 将底部布局添加到主布局
    mainLayout->addLayout(buttonLayout);
    
    // 设置窗口布局和固定大小
    setLayout(mainLayout);
    setFixedSize(400, 600);  // 增加高度以容纳按键绑定控件
    setFocusPolicy(Qt::StrongFocus);  // 允许接收键盘事件
}

/**
 * @brief 更新UI显示
 * 
 * 根据currentSettings更新所有UI组件的状态，确保UI与当前设置一致
 */
void SettingsWidget::updateUI()
{
    // 更新音量滑块位置
    soundEffectSlider->setValue(currentSettings.soundEffectVolume);
    musicSlider->setValue(currentSettings.musicVolume);
    
    // 更新复选按钮状态
    invertYAxisButton->setChecked(currentSettings.invertYAxis);
    showGridButton->setChecked(currentSettings.showGrid);
    
    // 更新按键绑定按钮文本
    keyMoveUpButton->setText(keyToString(currentSettings.keyMoveUp));
    keyMoveDownButton->setText(keyToString(currentSettings.keyMoveDown));
    keyMoveLeftButton->setText(keyToString(currentSettings.keyMoveLeft));
    keyMoveRightButton->setText(keyToString(currentSettings.keyMoveRight));
    keyRollButton->setText(keyToString(currentSettings.keyRoll));
    keyCollectButton->setText(keyToString(currentSettings.keyCollect));
    keyResourcePanelButton->setText(keyToString(currentSettings.keyResourcePanel));
    keyTowerPanelButton->setText(keyToString(currentSettings.keyTowerPanel));

    
    // 根据难度设置更新难度按钮状态
    switch (currentSettings.difficulty) {
        case 1:  // 简单难度
            difficultyButton1->setChecked(true);
            break;
        case 2:  // 普通难度
            difficultyButton2->setChecked(true);
            break;
        case 3:  // 困难难度
            difficultyButton3->setChecked(true);
            break;
    }
}

/**
 * @brief 获取当前UI中的设置
 * 
 * 从各个UI组件中读取设置值，构建并返回GameSettings对象
 * 
 * @return 当前UI中的GameSettings对象
 */
GameSettings SettingsWidget::getSettings() const
{
    GameSettings settings;
    
    // 从滑块获取音量设置
    settings.soundEffectVolume = soundEffectSlider->value();
    settings.musicVolume = musicSlider->value();
    
    // 从复选按钮获取控制和显示设置
    settings.invertYAxis = invertYAxisButton->isChecked();
    settings.showGrid = showGridButton->isChecked();
    
    // 获取按键绑定设置（从currentSettings中获取，因为按键绑定在keyPressEvent中更新）
    settings.keyMoveUp = currentSettings.keyMoveUp;
    settings.keyMoveDown = currentSettings.keyMoveDown;
    settings.keyMoveLeft = currentSettings.keyMoveLeft;
    settings.keyMoveRight = currentSettings.keyMoveRight;
    settings.keyRoll = currentSettings.keyRoll;
    settings.keyCollect = currentSettings.keyCollect;
    settings.keyResourcePanel = currentSettings.keyResourcePanel;
    settings.keyTowerPanel = currentSettings.keyTowerPanel;
    
    // 获取难度设置
    if (difficultyButton1->isChecked())
        settings.difficulty = 1;  // 简单
    else if (difficultyButton2->isChecked())
        settings.difficulty = 2;  // 普通
    else if (difficultyButton3->isChecked())
        settings.difficulty = 3;  // 困难
    
    return settings;
}

/**
 * @brief 设置初始设置
 * 
 * 设置当前设置并更新UI显示
 * 
 * @param settings 要应用的GameSettings对象
 */
void SettingsWidget::setSettings(const GameSettings& settings)
{
    if (!conflict()) {
        // 更新当前设置
        currentSettings = settings;
        qDebug() << "set1";
        // 根据新设置更新UI
        updateUI();
    }
}

/**
 * @brief 保存按钮点击事件处理
 * 
 * 当用户点击保存按钮时调用，获取当前UI设置并发送保存信号
 */
void SettingsWidget::on_saveButton_clicked()
{
    if (!conflict()) {

        // 从UI获取最新设置
        currentSettings = getSettings();
        qDebug() << "set2";
        // 发送设置保存信号，通知其他组件设置已更新
        emit settingsSaved(currentSettings);

        // 关闭设置窗口
        this->close();

    }
    else {

        // 创建设置冲突界面（设置为独立窗口，不依附于菜单窗口）
        saveConflictWidget = new SaveConflictWidget;  // nullptr makes it a separate window
        saveConflictWidget->setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint);
        connect(saveConflictWidget, &SaveConflictWidget::on_cancelButton_clicked, this, &SettingsWidget::on_saveButton_clicked);

        
            // 居中显示设置窗口
        saveConflictWidget->move(this->x() + (this->width() - saveConflictWidget->width()) / 2,
                this->y() + (this->height() - saveConflictWidget->height()) / 2);
            // 显示设置界面（作为独立窗口）
        saveConflictWidget->show();
        saveConflictWidget->raise();  // 确保窗口在最前面
        saveConflictWidget->activateWindow();  // 激活窗口以接收键盘事件
        
    }


}



/**
 * @brief 取消按钮点击事件处理
 * 
 * 当用户点击取消按钮时调用，直接关闭窗口不保存设置
 */
void SettingsWidget::on_cancelButton_clicked()
{
    // 直接关闭窗口，不保存任何设置更改
    waitingForKeyButton = nullptr;  // 清除等待状态
    this->close();
}

/**
 * @brief 将Qt键码转换为字符串显示
 * 
 * @param key Qt键码
 * @return 按键名称字符串
 */
QString SettingsWidget::keyToString(int key) const
{
    switch (key) {
        case Qt::Key_W: return "W";
        case Qt::Key_A: return "A";
        case Qt::Key_S: return "S";
        case Qt::Key_D: return "D";
        case Qt::Key_Up: return "↑";
        case Qt::Key_Down: return "↓";
        case Qt::Key_Left: return "←";
        case Qt::Key_Right: return "→";
        case Qt::Key_Shift: return "Shift";
        case Qt::Key_Space: return "Space";
        case Qt::Key_Control: return "Ctrl";
        case Qt::Key_Alt: return "Alt";
        case Qt::Key_Escape: return "Esc";
        case Qt::Key_Return:
        case Qt::Key_Enter: return "Enter";
        case Qt::Key_Tab: return "Tab";
        default:
            if (key >= Qt::Key_0 && key <= Qt::Key_9) {
                return QString::number(key - Qt::Key_0);
            }
            if (key >= Qt::Key_A && key <= Qt::Key_Z) {
                return QChar('A' + (key - Qt::Key_A));
            }
            return QKeySequence(key).toString();
    }
}

/**
 * @brief 按键绑定按钮点击事件处理
 * 当用户点击按键绑定按钮时，进入等待按键输入状态
 */
void SettingsWidget::onKeyButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (!button) return;
    
    // 如果已经有按钮在等待，先取消
    if (waitingForKeyButton && waitingForKeyButton != button) {
        waitingForKeyButton->setStyleSheet("");  // 恢复原样式
    }
    
    // 设置当前按钮为等待状态
    waitingForKeyButton = button;
    button->setText("Press any key...");
    button->setStyleSheet("background-color: yellow;");  // 高亮显示
    setFocus();  // 获取焦点以接收键盘事件
}

/**
 * @brief 键盘按键事件处理（用于捕获按键绑定）
 * 
 * @param event 键盘事件
 */
void SettingsWidget::keyPressEvent(QKeyEvent *event)
{
    // 如果正在等待按键输入
    if (waitingForKeyButton) {
        int key = event->key();
        
        // 允许使用Shift、Ctrl等功能键作为按键绑定
        // 不再过滤这些键，允许用户将它们设置为快捷键
        /*
        if (key == Qt::Key_Shift || key == Qt::Key_Control || key == Qt::Key_Alt || 
            key == Qt::Key_Meta) {
            QWidget::keyPressEvent(event);
            return;
        }
        */
        
        // 更新对应的设置
        if (waitingForKeyButton == keyMoveUpButton) {
            currentSettings.keyMoveUp = key;
        } else if (waitingForKeyButton == keyMoveDownButton) {
            currentSettings.keyMoveDown = key;
        } else if (waitingForKeyButton == keyMoveLeftButton) {
            currentSettings.keyMoveLeft = key;
        } else if (waitingForKeyButton == keyMoveRightButton) {
            currentSettings.keyMoveRight = key;
        } else if (waitingForKeyButton == keyRollButton) {
            currentSettings.keyRoll = key;
        } else if (waitingForKeyButton == keyCollectButton) {
            currentSettings.keyCollect = key;
        } else if (waitingForKeyButton == keyResourcePanelButton) {
            currentSettings.keyResourcePanel = key;
        } else if (waitingForKeyButton == keyTowerPanelButton) {
            currentSettings.keyTowerPanel = key;
        }
        
        // 更新按钮文本并恢复样式
        waitingForKeyButton->setText(keyToString(key));
        waitingForKeyButton = nullptr;
        
        // 检查按键冲突并更新按钮样式
        checkKeyConflicts();
        
        event->accept();
        return;
    }
    
    // 如果没有等待按键，处理默认键盘事件（如ESC关闭窗口）
    if (event->key() == Qt::Key_Escape) {
        on_cancelButton_clicked();
        event->accept();
        return;
    }
    
    QWidget::keyPressEvent(event);
}

/**
 * @brief 检查按键冲突
 * 
 * 遍历所有按键绑定，检查是否有重复的按键
 * 如果有冲突，将冲突的按钮显示为红色
 */
void SettingsWidget::checkKeyConflicts()
{
    // 重置所有按钮样式
    QList<QPushButton*> keyButtons = {
        keyMoveUpButton, keyMoveDownButton, keyMoveLeftButton, keyMoveRightButton,
        keyRollButton, keyCollectButton, keyResourcePanelButton, keyTowerPanelButton
    };
    
    // 先清除所有按钮的样式
    for (QPushButton* btn : keyButtons) {
        btn->setStyleSheet("");
    }
    
    // 创建映射来记录每个按键对应的按钮列表
    QMap<int, QList<QPushButton*>> keyMap;
    
    // 将所有按键绑定添加到映射中
    keyMap[currentSettings.keyMoveUp].append(keyMoveUpButton);
    keyMap[currentSettings.keyMoveDown].append(keyMoveDownButton);
    keyMap[currentSettings.keyMoveLeft].append(keyMoveLeftButton);
    keyMap[currentSettings.keyMoveRight].append(keyMoveRightButton);
    keyMap[currentSettings.keyRoll].append(keyRollButton);
    keyMap[currentSettings.keyCollect].append(keyCollectButton);
    keyMap[currentSettings.keyResourcePanel].append(keyResourcePanelButton);
    keyMap[currentSettings.keyTowerPanel].append(keyTowerPanelButton);
    
    // 检查是否有按键被重复使用
    for (auto it = keyMap.begin(); it != keyMap.end(); ++it) {
        if (it.value().size() > 1) {
            // 有冲突，将所有冲突的按钮显示为红色
            for (QPushButton* btn : it.value()) {
                btn->setStyleSheet("background-color: red;");
            }  
        }
        else {   
        }
    }
}
/**
 * @brief 检查按键冲突
 *
 * 遍历所有按键绑定，检查是否有重复的按键
 * 如果有冲突，返回true使sava无法保存
 */
bool SettingsWidget::conflict() {
    

    // 创建映射来记录每个按键对应的按钮列表
    QMap<int, QList<QPushButton*>> keyMap;

    // 将所有按键绑定添加到映射中
    keyMap[currentSettings.keyMoveUp].append(keyMoveUpButton);
    keyMap[currentSettings.keyMoveDown].append(keyMoveDownButton);
    keyMap[currentSettings.keyMoveLeft].append(keyMoveLeftButton);
    keyMap[currentSettings.keyMoveRight].append(keyMoveRightButton);
    keyMap[currentSettings.keyRoll].append(keyRollButton);
    keyMap[currentSettings.keyCollect].append(keyCollectButton);
    keyMap[currentSettings.keyResourcePanel].append(keyResourcePanelButton);
    keyMap[currentSettings.keyTowerPanel].append(keyTowerPanelButton);

    // 检查是否有按键被重复使用
    for (auto it = keyMap.begin(); it != keyMap.end(); ++it) {
        if (it.value().size() > 1) {
            // 有冲突
            qDebug() << "setconflict true";
            return true;
        }
        
    }
    return false;
}
