#include "_UI/EncyclopediaDialog.h"
#include <QGroupBox>
#include <QFont>

EncyclopediaDialog::EncyclopediaDialog(QWidget *parent)
    : QDialog(parent) {
    setupUI();
    setWindowTitle("塔防游戏图鉴系统");
    setMinimumSize(800, 600);
}

void EncyclopediaDialog::setupUI() {
    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 创建标签页
    tabWidget = new QTabWidget(this);

    // 设置怪物标签页
    setupMonsterTab();

    // 设置炮塔标签页
    setupTowerTab();

    // 添加标签页
    tabWidget->addTab(monsterTab, "怪物图鉴");
    tabWidget->addTab(towerTab, "炮塔图鉴");

    mainLayout->addWidget(tabWidget);
}

void EncyclopediaDialog::setupMonsterTab() {
    monsterTab = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(monsterTab);

    // 左侧列表
    monsterList = new QListWidget();
    monsterList->setMaximumWidth(200);

    // 添加怪物项
    QVector<MonsterEncyclopediaInfo> monsterInfos = EncyclopediaData::getAllMonsterInfos();
    for(const auto &info : monsterInfos) {
        QListWidgetItem *item = new QListWidgetItem(info.name);
        item->setData(Qt::UserRole, info.name);
        monsterList->addItem(item);
    }

    // 右侧详细信息区域
    monsterDetails = new QWidget();
    QVBoxLayout *detailsLayout = new QVBoxLayout(monsterDetails);

    // 创建怪物专用的显示组件
    monsterImageLabel = new QLabel();
    monsterImageLabel->setAlignment(Qt::AlignCenter);
    monsterImageLabel->setMinimumSize(100, 100);

    monsterNameLabel = new QLabel();
    QFont nameFont = monsterNameLabel->font();
    nameFont.setPointSize(16);
    nameFont.setBold(true);
    monsterNameLabel->setFont(nameFont);

    monsterDescriptionLabel = new QLabel();
    monsterDescriptionLabel->setWordWrap(true);

    monsterStatsLabel = new QLabel();
    monsterStatsLabel->setWordWrap(true);

    monsterSpecialAbilityLabel = new QLabel();
    monsterSpecialAbilityLabel->setWordWrap(true);

    // 添加到布局
    detailsLayout->addWidget(monsterImageLabel);
    detailsLayout->addWidget(monsterNameLabel);
    detailsLayout->addWidget(monsterDescriptionLabel);

    // 属性分组框
    QGroupBox *statsGroup = new QGroupBox("属性");
    QVBoxLayout *statsLayout = new QVBoxLayout(statsGroup);
    statsLayout->addWidget(monsterStatsLabel);
    detailsLayout->addWidget(statsGroup);

    QGroupBox *abilityGroup = new QGroupBox("特殊能力");
    QVBoxLayout *abilityLayout = new QVBoxLayout(abilityGroup);
    abilityLayout->addWidget(monsterSpecialAbilityLabel);
    detailsLayout->addWidget(abilityGroup);

    detailsLayout->addStretch();

    // 连接信号
    connect(monsterList, &QListWidget::itemClicked,
            this, &EncyclopediaDialog::onMonsterItemClicked);

    layout->addWidget(monsterList);
    layout->addWidget(monsterDetails, 1);

    // 默认显示第一个怪物
    if(monsterList->count() > 0) {
        monsterList->setCurrentRow(0);
        onMonsterItemClicked(monsterList->item(0));
    }
}

void EncyclopediaDialog::setupTowerTab() {
    towerTab = new QWidget();
    QHBoxLayout *layout = new QHBoxLayout(towerTab);

    // 左侧列表
    towerList = new QListWidget();
    towerList->setMaximumWidth(200);

    // 添加炮塔项
    QVector<TowerEncyclopediaInfo> towerInfos = EncyclopediaData::getAllTowerInfos();
    for(const auto &info : towerInfos) {
        QListWidgetItem *item = new QListWidgetItem(info.name);
        item->setData(Qt::UserRole, info.name);
        towerList->addItem(item);
    }

    // 右侧详细信息区域
    towerDetails = new QWidget();
    QVBoxLayout *detailsLayout = new QVBoxLayout(towerDetails);

    // 为炮塔标签页创建专用的显示组件
    towerImageLabel = new QLabel();
    towerImageLabel->setAlignment(Qt::AlignCenter);
    towerImageLabel->setMinimumSize(100, 100);

    towerNameLabel = new QLabel();
    QFont towerNameFont = towerNameLabel->font();
    towerNameFont.setPointSize(16);
    towerNameFont.setBold(true);
    towerNameLabel->setFont(towerNameFont);

    towerDescriptionLabel = new QLabel();
    towerDescriptionLabel->setWordWrap(true);

    towerStatsLabel = new QLabel();
    towerStatsLabel->setWordWrap(true);

    towerSpecialAbilityLabel = new QLabel();
    towerSpecialAbilityLabel->setWordWrap(true);

    // 添加到布局
    detailsLayout->addWidget(towerImageLabel);
    detailsLayout->addWidget(towerNameLabel);
    detailsLayout->addWidget(towerDescriptionLabel);

    // 属性分组框 - 修复：需要将towerStatsLabel添加到布局中
    QGroupBox *towerStatsGroup = new QGroupBox("属性");
    QVBoxLayout *towerStatsLayout = new QVBoxLayout(towerStatsGroup);
    towerStatsLayout->addWidget(towerStatsLabel); // 添加这行
    detailsLayout->addWidget(towerStatsGroup);

    QGroupBox *towerAbilityGroup = new QGroupBox("特殊能力");
    QVBoxLayout *towerAbilityLayout = new QVBoxLayout(towerAbilityGroup);
    towerAbilityLayout->addWidget(towerSpecialAbilityLabel);
    detailsLayout->addWidget(towerAbilityGroup);

    detailsLayout->addStretch();

    // 连接信号
    connect(towerList, &QListWidget::itemClicked,
            this, &EncyclopediaDialog::onTowerItemClicked);

    layout->addWidget(towerList);
    layout->addWidget(towerDetails, 1);

    // 默认显示第一个炮塔
    if(towerList->count() > 0) {
        towerList->setCurrentRow(0);
        onTowerItemClicked(towerList->item(0));
    }
}


void EncyclopediaDialog::onMonsterItemClicked(QListWidgetItem *item) {
    QString monsterName = item->text();

    // 根据名称获取怪物信息
    QVector<MonsterEncyclopediaInfo> infos = EncyclopediaData::getAllMonsterInfos();
    for(const auto &info : infos) {
        if(info.name == monsterName) {
            showMonsterDetails(info);
            break;
        }
    }
}

void EncyclopediaDialog::onTowerItemClicked(QListWidgetItem *item) {
    QString towerName = item->text();

    // 根据名称获取炮塔信息
    QVector<TowerEncyclopediaInfo> infos = EncyclopediaData::getAllTowerInfos();
    for(const auto &info : infos) {
        if(info.name == towerName) {
            showTowerDetails(info);
            break;
        }
    }
}

void EncyclopediaDialog::showMonsterDetails(const MonsterEncyclopediaInfo &info) {
    // 加载图片
    QPixmap pixmap(info.imagePath);
    if(!pixmap.isNull()) {
        pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        monsterImageLabel->setPixmap(pixmap);
    }

    monsterNameLabel->setText(info.name);
    monsterDescriptionLabel->setText(info.description);

    // 设置属性文本
    QString statsText = QString(
                            "生命值: %1\n"
                            "移动速度: %2\n"
                            "金币奖励: %3\n"
                            "攻击伤害: %4"
                            ).arg(info.health).arg(info.speed).arg(info.goldReward).arg(info.attackDamage);

    monsterStatsLabel->setText(statsText);
    monsterSpecialAbilityLabel->setText(info.specialAbility);
}

void EncyclopediaDialog::showTowerDetails(const TowerEncyclopediaInfo &info) {
    // 加载图片
    QPixmap pixmap(info.imagePath);
    if(!pixmap.isNull()) {
        pixmap = pixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        towerImageLabel->setPixmap(pixmap);
    }

    towerNameLabel->setText(info.name);
    towerDescriptionLabel->setText(info.description);

    // 设置属性文本
    QString statsText = QString(
                            "生命值: %1\n"
                            "建造费用: %2\n"
                            "攻击范围: %3\n"
                            "攻击间隔: %4毫秒\n"
                            "子弹伤害: %5\n"
                            "子弹速度: %6"
                            ).arg(info.health).arg(info.buildCost).arg(info.attackRange)
                            .arg(info.attackInterval).arg(info.bulletDamage).arg(info.bulletSpeed);

    towerStatsLabel->setText(statsText);
    towerSpecialAbilityLabel->setText(info.specialAbility);
}
