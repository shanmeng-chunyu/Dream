#include "LevelEditorWidget.h"
#include "ConfigHelper.h"
#include <QIcon>
#include <QFontDatabase>
#include <QPainter>
#include <QPaintEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QFormLayout>
#include <QSplitter>
#include <QFile>
#include <QDebug>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QDir>

LevelEditorWidget::LevelEditorWidget(QWidget* parent)
    : QWidget(parent),
      mainSplitter(nullptr),
      levelNameEdit(nullptr),
      saveButton(nullptr), loadButton(nullptr),
      waveListWidget(nullptr), enemyInWaveListWidget(nullptr),
      addWaveButton(nullptr), removeWaveButton(nullptr),
      addEnemyToWaveButton(nullptr), removeEnemyFromWaveButton(nullptr),
      wave_enemyTypeComboBox(nullptr), wave_enemyCountSpinBox(nullptr),
      wave_enemyThumbnailLabel(nullptr),
      availableTowersListWidget(nullptr),
      tower_typeComboBox(nullptr), tower_thumbnailLabel(nullptr),
      tower_warningLabel(nullptr),
      m_firstEnemyType("bug") // 默认值，会被 loadPrototypes 覆盖
{

    this->setWindowIcon(QIcon(":/background/resources/background/title.png"));// 设置窗口图标

    if (!m_backgroundPixmap.load(":/background/resources/background/third.png")) {
        qWarning() << "LevelEditorWidget: 未能加载背景图片!";
    } //加载背景图片

    int fontId = QFontDatabase::addApplicationFont(":/font/resources/font/font.ttf");
    if (fontId != -1) {
        QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (!fontFamilies.isEmpty()) {
            m_fontFamily = fontFamilies.at(0); // 1. 存储字体族名
        }
    } else {
        qWarning() << "LevelEditorWidget: 未能加载字体 ':/font/resource/font/font.ttf'";
    }

    // 1. 加载所有敌人和塔的原型数据
    loadPrototypes();

    // 2. 构建UI
    setupUI();
    loadButton->hide();
    // 3. --- 连接通用信号 ---
    connect(saveButton, &QPushButton::clicked, this, &LevelEditorWidget::saveLevel);
    connect(loadButton, &QPushButton::clicked, this, &LevelEditorWidget::loadLevel);

    // 4. --- 连接波次编辑器信号 ---
    connect(addWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::addWave);
    connect(removeWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::removeWave);
    connect(waveListWidget, &QListWidget::currentItemChanged, this, &LevelEditorWidget::onWaveSelectionChanged);

    connect(addEnemyToWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::addEnemyToWave);
    connect(removeEnemyFromWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::removeEnemyFromWave);
    connect(enemyInWaveListWidget, &QListWidget::currentItemChanged, this, &LevelEditorWidget::onEnemyInWaveSelectionChanged);

    // 连接敌人详情编辑控件
    connect(wave_enemyTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LevelEditorWidget::onWaveEnemyTypeChanged);
    connect(wave_enemyCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &LevelEditorWidget::onWaveEnemyCountChanged);

    // 5. --- 连接防御塔选择器信号 ---
    connect(availableTowersListWidget, &QListWidget::currentItemChanged, this, &LevelEditorWidget::onTowerSlotSelectionChanged);
    connect(tower_typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &LevelEditorWidget::onTowerTypeChanged);


    // 6. --- 初始化UI状态 ---
    onWaveSelectionChanged();
    onTowerSlotSelectionChanged();
}

/**
 * @brief 从 master JSON 文件加载敌人和塔的原型数据
 */
void LevelEditorWidget::loadPrototypes() {
    m_enemyPrototypes.clear();
    m_towerPrototypes.clear();
    m_enemyTypeOrder.clear();
    m_towerTypeOrder.clear();   // <--- 清空顺序列表

    // --- 在此填充你的 enemy_data.json 和 tower_data.json 的实际路径 ---
    // 提示: 最好使用 Qt 资源文件 (qrc) 路径，例如 ":/data/enemy_data.json"
    // 为了方便测试，这里暂时使用你上传的文件名。
    QString enemyDataPath = getConfigFile("enemy_data.json");
    QString towerDataPath = getConfigFile("tower_data.json");
    // -------------------------------------------------------------

    // 加载敌人
    // --- 加载防御塔 ---
    QFile enemyFile(enemyDataPath);
    if (enemyFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(enemyFile.readAll());
        QJsonArray enemies = doc.object()["master_enemies"].toArray();
        for (const QJsonValue& val : enemies) {
            QJsonObject obj = val.toObject();
            QString type = obj["type"].toString();
            if (type.isEmpty()) continue;
            m_enemyPrototypes[type] = obj;        // 1. 填充 Map (用于查找)
            m_enemyTypeOrder.append(type);    // 2. 填充 List (用于排序)
        }
        enemyFile.close();

    } else {
        qWarning() << "LevelEditorWidget: Failed to load" << enemyDataPath;
    }

    if (!m_enemyTypeOrder.isEmpty()) {
        m_firstEnemyType = m_enemyTypeOrder.first();
    }

    // 加载防御塔
    QFile towerFile(towerDataPath);
    if (towerFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(towerFile.readAll());
        QJsonArray towers = doc.object()["master_towers"].toArray();
        for (const QJsonValue& val : towers) {
            QJsonObject obj = val.toObject();
            QString type = obj["type"].toString();
            if (type.isEmpty()) continue;
            m_towerPrototypes[type] = obj;      // 1. 填充 Map
            m_towerTypeOrder.append(type);  // 2. 填充 List
        }
        towerFile.close();
    } else {
        qWarning() << "LevelEditorWidget: Failed to load" << towerDataPath;
    }
}

/**
 * @brief 构建编辑器的主UI布局
 */
void LevelEditorWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);

    QString baseStyle = QString(
            "QWidget {"
            "    font-family: '%1';"
            "    font-size: 14px;"
            "    color: #333333;"
            "}"
        ).arg(m_fontFamily);

    this->setStyleSheet(baseStyle +
        "QGroupBox {"
        "    background-color: rgba(255, 255, 255, 0.15);" // 15% 不透明度的白色背景
        "    border-radius: 5px;" // (可选) 圆角
        "}"
        "QListWidget {"
        "    background-color: rgba(255, 255, 255, 0.1);" // 列表背景更透明一些
        "}"
        "QSplitter::handle {"
        "    background-color: rgba(128, 128, 128, 0.3);" // 半透明的手柄
        "}"
        "QScrollBar:vertical {"
        "    background: rgba(0, 0, 0, 0.3); /* 滚动条底槽: 半透明黑色 */"
        "    width: 10px;"
        "    margin: 0px 0px 0px 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "    background: rgba(128, 128, 128, 0.7); /* 滑块: 半透明灰色 */"
        "    min-height: 20px;"
        "    border-radius: 5px; /* 圆角 */"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "    /* 隐藏上下箭头 */"
        "    height: 0px;"
        "    background: none;"
        "}"
        "QListWidget::item:selected {"
        "background-color: rgba(0, 150, 200, 0.5); /* 半透明的蓝色高亮 */"
        "border: 1px solid #00aaff;"
        "color: white;" /* 选中时文字变白 */
        "}"
    );

    // 顶部：关卡名
    // auto* levelNameLayout = new QHBoxLayout();
    // levelNameLayout->addWidget(new QLabel("Level Name:"));
    // levelNameEdit = new QLineEdit("Custom Level");
    // levelNameLayout->addWidget(levelNameEdit);
    // mainLayout->addLayout(levelNameLayout);

    // 中间：主分割器 (波次 vs 塔)
    mainSplitter = new QSplitter(Qt::Vertical, this);

    // 上半部分：波次编辑器
    mainSplitter->addWidget(createWaveEditorGroup());

    // 下半部分：防御塔选择器
    mainSplitter->addWidget(createTowerSelectionGroup());

    // 设置分割比例 (3/4 vs 1/4)
    mainSplitter->setSizes({600, 200}); // 初始大小
    mainSplitter->setStretchFactor(0, 2); // 波次编辑器占 3 份
    mainSplitter->setStretchFactor(1, 2); // 塔选择器占 1 份

    mainLayout->addWidget(mainSplitter, 1); // 允许分割器伸展

    // 底部：保存/加载按钮
    auto* bottomButtonLayout = new QHBoxLayout();
    saveButton = new QPushButton("保存关卡配置");
    loadButton = new QPushButton("加载关卡配置");
    bottomButtonLayout->addStretch();
    bottomButtonLayout->addWidget(loadButton);
    bottomButtonLayout->addWidget(saveButton);
    mainLayout->addLayout(bottomButtonLayout);
}

/**
 * @brief 创建波次编辑器 (上半部分)
 */
QGroupBox* LevelEditorWidget::createWaveEditorGroup() {
    auto* waveGroup = new QGroupBox("波次编辑");
    auto* waveTabLayout = new QHBoxLayout(waveGroup);

    // 左侧：波次列表
    auto* waveListLayout = new QVBoxLayout();
    waveListWidget = new QListWidget();
    auto* waveButtonLayout = new QHBoxLayout();
    addWaveButton = new QPushButton("添加波次");
    removeWaveButton = new QPushButton("删除波次");
    waveButtonLayout->addWidget(addWaveButton);
    waveButtonLayout->addWidget(removeWaveButton);
    waveListLayout->addWidget(waveListWidget);
    waveListLayout->addLayout(waveButtonLayout);

    // 中间：当前波次的敌人列表
    auto* enemyListLayout = new QVBoxLayout();
    enemyInWaveListWidget = new QListWidget();
    auto* enemyButtonLayout = new QHBoxLayout();
    addEnemyToWaveButton = new QPushButton("新增敌人");
    removeEnemyFromWaveButton = new QPushButton("删除敌人");
    enemyButtonLayout->addWidget(addEnemyToWaveButton);
    enemyButtonLayout->addWidget(removeEnemyFromWaveButton);
    enemyListLayout->addWidget(enemyInWaveListWidget);
    enemyListLayout->addLayout(enemyButtonLayout);

    // 右侧：选定敌人的详细信息
    auto* detailsLayout = new QFormLayout();
    wave_enemyTypeComboBox = new QComboBox();
    // 填充敌人种类
    for (const QString& type : m_enemyTypeOrder) {
        // 从 Map 中按 type 取出 Object
        const QJsonObject& obj = m_enemyPrototypes.value(type);
        wave_enemyTypeComboBox->addItem(obj["name"].toString(), obj["type"].toString());
    }

    wave_enemyCountSpinBox = new QSpinBox();
    wave_enemyCountSpinBox->setRange(1, 999);
    wave_enemyCountSpinBox->setValue(10);

    wave_enemyThumbnailLabel = new QLabel("[Enemy Thumbnail]");
    wave_enemyThumbnailLabel->setFixedSize(200, 200);
    wave_enemyThumbnailLabel->setScaledContents(true);
    wave_enemyThumbnailLabel->setAlignment(Qt::AlignCenter);
    wave_enemyThumbnailLabel->setFrameShape(QFrame::Box);
    wave_enemyDescriptionLabel = new QLabel("[Enemy Description]");
    wave_enemyDescriptionLabel->setWordWrap(true); // 允许自动换行
    wave_enemyDescriptionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    detailsLayout->addRow("怪物类型:", wave_enemyTypeComboBox);
    detailsLayout->addRow("怪物数量:", wave_enemyCountSpinBox);
    detailsLayout->addRow("", wave_enemyThumbnailLabel);
    detailsLayout->addRow("描述:", wave_enemyDescriptionLabel);

    // 组合三栏
    waveTabLayout->addLayout(waveListLayout, 1);
    waveTabLayout->addLayout(enemyListLayout, 2);
    waveTabLayout->addLayout(detailsLayout, 1);

    return waveGroup;
}

/**
 * @brief 创建防御塔选择器 (下半部分)
 */
QGroupBox* LevelEditorWidget::createTowerSelectionGroup() {
    auto* towerGroup = new QGroupBox("选择防御塔");
    auto* towerGroupLayout = new QHBoxLayout(towerGroup);

    // 左侧：固定的4个槽位
    availableTowersListWidget = new QListWidget();
    for (int i = 1; i <= 4; ++i) {
        auto* item = new QListWidgetItem(QString("防御塔%1").arg(QString::number(i)));
        item->setData(Qt::UserRole, QJsonObject()); // 存储 QJsonObject { "type": "..." }
        availableTowersListWidget->addItem(item);
    }
    availableTowersListWidget->setFixedWidth(150);

    // 中间：选择栏
    auto* towerSelectLayout = new QFormLayout();
    tower_typeComboBox = new QComboBox();
    tower_typeComboBox->addItem("None", "None");
    // 迭代
    for (const QString& type : m_towerTypeOrder) {
        // 从 Map 中按 type 取出 Object
        const QJsonObject& obj = m_towerPrototypes.value(type);
        tower_typeComboBox->addItem(obj["name"].toString(), obj["type"].toString());
    }

    tower_warningLabel = new QLabel("Cannot select duplicate tower type!");
    tower_warningLabel->setStyleSheet("color: red;");
    tower_warningLabel->setVisible(false);
    tower_warningLabel->setWordWrap(true);
    tower_statsLabel = new QLabel("[Cost / Range]");
    tower_descriptionLabel = new QLabel("[Tower Description]");
    tower_descriptionLabel->setWordWrap(true); // 允许自动换行
    tower_descriptionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    towerSelectLayout->addRow("防御塔类型:", tower_typeComboBox);
    towerSelectLayout->addRow(tower_warningLabel);
    towerSelectLayout->addRow(tower_statsLabel);
    towerSelectLayout->addRow(tower_descriptionLabel);

    // 右侧：缩略图
    tower_thumbnailLabel = new QLabel("[Tower Thumbnail]");
    tower_thumbnailLabel->setFixedSize(100, 100);
    tower_thumbnailLabel->setScaledContents(true);
    tower_thumbnailLabel->setAlignment(Qt::AlignCenter);
    tower_thumbnailLabel->setFrameShape(QFrame::Box);

    // 组合三栏
    towerGroupLayout->addWidget(availableTowersListWidget, 1);
    towerGroupLayout->addLayout(towerSelectLayout, 2);
    towerGroupLayout->addWidget(tower_thumbnailLabel, 1);

    return towerGroup;
}


// --- Wave Editor Slots ---

void LevelEditorWidget::addWave() {
    int waveCount = waveListWidget->count();
    auto* item = new QListWidgetItem(QString("波次%1").arg(QString::number(waveCount + 1)));
    item->setData(Qt::UserRole, QJsonArray()); // 波次数据存储为敌人数组
    waveListWidget->addItem(item);
    waveListWidget->setCurrentItem(item);
}

void LevelEditorWidget::removeWave() {
    delete waveListWidget->takeItem(waveListWidget->currentRow());
}

void LevelEditorWidget::onWaveSelectionChanged() {
    enemyInWaveListWidget->clear();
    updateWaveEnemyDetailsUI(nullptr); // 清空并禁用详情

    QListWidgetItem* currentWave = waveListWidget->currentItem();
    if (!currentWave) {
        addEnemyToWaveButton->setEnabled(false);
        removeEnemyFromWaveButton->setEnabled(false);
        return;
    }

    addEnemyToWaveButton->setEnabled(true);

    // 加载该波次的敌人列表
    QJsonArray enemies = currentWave->data(Qt::UserRole).toJsonArray();
    for (const QJsonValue& enemyValue : enemies) {
        QJsonObject enemyObj = enemyValue.toObject();
        QString type = enemyObj["type"].toString(m_firstEnemyType);
        int count = enemyObj["count"].toInt(1);
        QString name = m_enemyPrototypes.value(type).value("name").toString(type);
        auto* item = new QListWidgetItem(QString("%1 x%2").arg(name).arg(QString::number(count)));
        item->setData(Qt::UserRole, enemyObj);
        enemyInWaveListWidget->addItem(item);
    }

    onEnemyInWaveSelectionChanged();
}

void LevelEditorWidget::addEnemyToWave() {
    QListWidgetItem* currentWave = waveListWidget->currentItem();
    if (!currentWave) return;

    // 需求：新增默认为 种类1 (m_firstEnemyType)，数量1
    QJsonObject newEnemy;
    newEnemy["type"] = m_firstEnemyType;
    newEnemy["count"] = 1;
    newEnemy["interval"] = 1.0; // 需求：间隔固定，我们在此硬编码

    QString name = m_enemyPrototypes.value(m_firstEnemyType).value("name").toString(m_firstEnemyType);
    auto* item = new QListWidgetItem(QString("%1 x%2").arg(name).arg(QString::number(1)));
    item->setData(Qt::UserRole, newEnemy);
    enemyInWaveListWidget->addItem(item);

    // 更新 wave item 的数据
    QJsonArray enemies = currentWave->data(Qt::UserRole).toJsonArray();
    enemies.append(newEnemy);
    currentWave->setData(Qt::UserRole, enemies);

    enemyInWaveListWidget->setCurrentItem(item);
}

void LevelEditorWidget::removeEnemyFromWave() {
    QListWidgetItem* currentWave = waveListWidget->currentItem();
    QListWidgetItem* currentEnemy = enemyInWaveListWidget->currentItem();
    if (!currentWave || !currentEnemy) return;

    int row = enemyInWaveListWidget->currentRow();
    delete enemyInWaveListWidget->takeItem(row);

    // 更新 wave item 的数据
    QJsonArray enemies = currentWave->data(Qt::UserRole).toJsonArray();
    enemies.removeAt(row);
    currentWave->setData(Qt::UserRole, enemies);
}

void LevelEditorWidget::onEnemyInWaveSelectionChanged() {
    updateWaveEnemyDetailsUI(enemyInWaveListWidget->currentItem());
}

/**
 * @brief (辅助) 更新敌人详情UI
 */
void LevelEditorWidget::updateWaveEnemyDetailsUI(QListWidgetItem* item) {
    if (!item) {
        wave_enemyTypeComboBox->setEnabled(false);
        wave_enemyCountSpinBox->setEnabled(false);
        removeEnemyFromWaveButton->setEnabled(false);
        wave_enemyTypeComboBox->setCurrentIndex(0);
        wave_enemyCountSpinBox->setValue(0);
        wave_enemyThumbnailLabel->clear();
        wave_enemyDescriptionLabel->clear();
        return;
    }

    wave_enemyTypeComboBox->setEnabled(true);
    wave_enemyCountSpinBox->setEnabled(true);
    removeEnemyFromWaveButton->setEnabled(true);

    QJsonObject enemyObj = item->data(Qt::UserRole).toJsonObject();
    QString type = enemyObj["type"].toString(m_firstEnemyType);

    // 暂停信号，防止触发槽函数
    wave_enemyTypeComboBox->blockSignals(true);
    wave_enemyCountSpinBox->blockSignals(true);

    // 1. 查找 ComboBox 中 'type' 对应的索引
    int index = wave_enemyTypeComboBox->findData(type);
    // 2. 设置 ComboBox 的当前索引
    wave_enemyTypeComboBox->setCurrentIndex(index != -1 ? index : 0);

    // 更新缩略图
    QString pixmapPath = getPixmapPath(m_enemyPrototypes, enemyObj["type"].toString());
    if (!pixmapPath.isEmpty()) {
        wave_enemyThumbnailLabel->setPixmap(QPixmap(pixmapPath));
    } else {
        wave_enemyThumbnailLabel->setText("");
    }

    // 更新描述
    // (我们从 m_enemyPrototypes 中获取完整的原型, 而不是 item 的数据, 因为 item 的数据可能不全)
    QJsonObject proto = m_enemyPrototypes.value(type);
    wave_enemyDescriptionLabel->setText(proto.value("description").toString());

    // 恢复信号
    wave_enemyTypeComboBox->blockSignals(false);
    wave_enemyCountSpinBox->blockSignals(false);
}

void LevelEditorWidget::onWaveEnemyTypeChanged(int index) {
    QListWidgetItem* enemyItem = enemyInWaveListWidget->currentItem();
    QListWidgetItem* waveItem = waveListWidget->currentItem();
    if (!enemyItem || !waveItem || index == -1) return;

    // 1. 从 ComboBox 的 'index' 获取 'type'
    QString type = wave_enemyTypeComboBox->itemData(index).toString();

    QJsonObject enemyData = enemyItem->data(Qt::UserRole).toJsonObject();
    enemyData["type"] = type;

    int row = enemyInWaveListWidget->row(enemyItem);
    updateWaveItemData(waveItem, row, enemyData);

    // 更新缩略图
    QString pixmapPath = getPixmapPath(m_enemyPrototypes, type);
    if (!pixmapPath.isEmpty()) {
        wave_enemyThumbnailLabel->setPixmap(QPixmap(pixmapPath));
    } else {
        wave_enemyThumbnailLabel->setText("");
    }

    QJsonObject proto = m_enemyPrototypes.value(type);
    wave_enemyDescriptionLabel->setText(proto.value("description").toString());

}

void LevelEditorWidget::onWaveEnemyCountChanged(int count) {
    QListWidgetItem* enemyItem = enemyInWaveListWidget->currentItem();
    QListWidgetItem* waveItem = waveListWidget->currentItem();
    if (!enemyItem || !waveItem) return;

    QJsonObject enemyData = enemyItem->data(Qt::UserRole).toJsonObject();
    enemyData["count"] = count;

    int row = enemyInWaveListWidget->row(enemyItem);
    updateWaveItemData(waveItem, row, enemyData);
}

/**
 * @brief (辅助) 更新列表项和父波次的数据
 */
void LevelEditorWidget::updateWaveItemData(QListWidgetItem* waveItem, int enemyRow, const QJsonObject& enemyData) {
    QListWidgetItem* enemyItem = enemyInWaveListWidget->item(enemyRow);
    if (!enemyItem) return;
    // 我们需要 'name' 来显示
    QString type = enemyData["type"].toString();
    QString name = m_enemyPrototypes.value(type).value("name").toString(type); // 找不到 'name' 时回退到 'type'

    // 1. 更新敌人列表项 (enemyInWaveListWidget)
    enemyItem->setText(QString("%1 x%2").arg(name).arg(QString::number(enemyData["count"].toInt())));
    enemyItem->setData(Qt::UserRole, enemyData);

    // 2. 更新波次列表项 (waveListWidget)
    QJsonArray enemies = waveItem->data(Qt::UserRole).toJsonArray();
    if (enemyRow >= 0 && enemyRow < enemies.size()) {
        enemies[enemyRow] = enemyData;
        waveItem->setData(Qt::UserRole, enemies);
    }
}


// --- Tower Selector Slots ---

void LevelEditorWidget::onTowerSlotSelectionChanged() {
    updateTowerDetailsUI(availableTowersListWidget->currentItem());
}

/**
 * @brief (辅助) 更新塔详情UI
 */
void LevelEditorWidget::updateTowerDetailsUI(QListWidgetItem* item) {
    tower_warningLabel->setVisible(false);

    if (!item) {
        tower_typeComboBox->setEnabled(false);
        tower_thumbnailLabel->clear();
        tower_statsLabel->clear();
        tower_descriptionLabel->clear();
        return;
    }

    tower_typeComboBox->setEnabled(true);

    // 暂停信号
    tower_typeComboBox->blockSignals(true);

    // 从槽位数据中获取塔类型
    QJsonObject towerData = item->data(Qt::UserRole).toJsonObject();
    QString type = towerData.value("type").toString("None");

    // 1. 查找 'type' 对应的索引
    int index = tower_typeComboBox->findData(type);
    // 2. 设置 ComboBox 的当前索引
    tower_typeComboBox->setCurrentIndex(index != -1 ? index : 0);

    // 更新缩略图
    QString pixmapPath = getPixmapPath(m_towerPrototypes, type);
    if (!pixmapPath.isEmpty()) {
        tower_thumbnailLabel->setPixmap(QPixmap(pixmapPath));
    } else {
        tower_thumbnailLabel->setText("");
    }

    // 更新属性和描述
    if (type == "None" || !m_towerPrototypes.contains(type)) {
        tower_statsLabel->clear();
        tower_descriptionLabel->clear();
    } else {
        QJsonObject proto = m_towerPrototypes.value(type);
        int cost = proto.value("cost").toInt();
        double range = proto.value("range").toDouble();
        QString desc = proto.value("description").toString();

        tower_statsLabel->setText(QString("消耗资源: %1 | 攻击范围: %2").arg(QString::number(cost)).arg(QString::number(range)));
        tower_descriptionLabel->setText(desc);
    }

    // 更新下拉列表的可用性
    updateTowerTypeComboBox();

    // 恢复信号
    tower_typeComboBox->blockSignals(false);
}


void LevelEditorWidget::onTowerTypeChanged(int index) {
    QListWidgetItem* currentSlot = availableTowersListWidget->currentItem();
    if (!currentSlot || index == -1) return;

    QString type = tower_typeComboBox->itemData(index).toString();

    // 检查唯一性
    for (int i = 0; i < availableTowersListWidget->count(); ++i) {
        QListWidgetItem* item = availableTowersListWidget->item(i);
        if (item == currentSlot) continue; // 跳过自己

        QJsonObject data = item->data(Qt::UserRole).toJsonObject();
        QString itemType = data.value("type").toString();

        if (!type.isEmpty() && type != "None" && itemType == type) {
            // 冲突！
            tower_warningLabel->setVisible(true);
            // 恢复到之前的值
            tower_typeComboBox->blockSignals(true);
            QJsonObject oldData = currentSlot->data(Qt::UserRole).toJsonObject();
            QString oldType = oldData.value("type").toString("None");
            int oldIndex = tower_typeComboBox->findData(oldType); // 查找旧 'type' 的索引
            tower_typeComboBox->setCurrentIndex(oldIndex != -1 ? oldIndex : 0); // 设置为旧索引
            tower_typeComboBox->blockSignals(false);
            return;
        }
    }

    // 唯一性检查通过
    tower_warningLabel->setVisible(false);

    // 更新槽位的数据和文本
    QJsonObject newData;
    if (type != "None") {
        newData["type"] = type;
        // 我们需要 'name' 来显示
        QString name = m_towerPrototypes.value(type).value("name").toString(type);
        currentSlot->setText(QString("防御塔%1: %2").arg(QString::number(availableTowersListWidget->row(currentSlot) + 1)).arg(name));
    } else {
        // newData 保持为空
        currentSlot->setText(QString("防御塔%1").arg(QString::number(availableTowersListWidget->row(currentSlot) + 1)));
    }
    currentSlot->setData(Qt::UserRole, newData);


    // 更新缩略图
    QString pixmapPath = getPixmapPath(m_towerPrototypes, type);
    if (!pixmapPath.isEmpty()) {
        tower_thumbnailLabel->setPixmap(QPixmap(pixmapPath));
    } else {
        tower_thumbnailLabel->setText("");
    }

    // 更新属性和描述
    if (type == "None" || !m_towerPrototypes.contains(type)) {
        tower_statsLabel->clear();
        tower_descriptionLabel->clear();
    } else {
        QJsonObject proto = m_towerPrototypes.value(type);
        int cost = proto.value("cost").toInt();
        double range = proto.value("range").toDouble();
        QString desc = proto.value("description").toString();

        tower_statsLabel->setText(QString("消耗资源: %1 | 攻击范围: %2").arg(QString::number(cost)).arg(QString::number(range)));
        tower_descriptionLabel->setText(desc);
    }

    // 更新所有下拉框的可用性
    updateTowerTypeComboBox();
}

/**
 * @brief (辅助) 更新塔类型下拉框，禁用已被其他槽位选择的项
 */
void LevelEditorWidget::updateTowerTypeComboBox() {
    // 1. 获取所有已被占用的塔类型
    QStringList usedTypes;
    for (int i = 0; i < availableTowersListWidget->count(); ++i) {
        QJsonObject data = availableTowersListWidget->item(i)->data(Qt::UserRole).toJsonObject();
        QString type = data.value("type").toString();
        if (!type.isEmpty() && type != "None") {
            usedTypes.append(type);
        }
    }

    // 2. 获取当前槽位正在选择的类型
    QString currentSlotType = "None";
    if (availableTowersListWidget->currentItem()) {
        QJsonObject data = availableTowersListWidget->currentItem()->data(Qt::UserRole).toJsonObject();
        currentSlotType = data.value("type").toString("None");
    }

    // 3. 遍历下拉框，设置可用性
    tower_typeComboBox->blockSignals(true);
    for (int i = 0; i < tower_typeComboBox->count(); ++i) {
        QString itemType = tower_typeComboBox->itemData(i).toString();
        if (itemType == "None") {
            continue; // "None" 选项总是可用
        }

        bool usedByOthers = usedTypes.contains(itemType) && (itemType != currentSlotType);

        // Qt 6/5 兼容方式设置 item 可用性
        QStandardItemModel* model = qobject_cast<QStandardItemModel*>(tower_typeComboBox->model());
        if(model) {
            model->item(i)->setEnabled(!usedByOthers);
        }
    }
    tower_typeComboBox->blockSignals(false);
}


// --- Save / Load ---

/**
 * @brief (辅助) 从原型数据中安全获取 pixmap 路径
 */
QString LevelEditorWidget::getPixmapPath(const QMap<QString, QJsonObject>& prototypes, const QString& type) {
    if (prototypes.contains(type)) {
        // --- 在此填充从原型对象中获取 pixmap 路径的 Key ---
        // 示例： "pixmap", "icon", "thumbnail"
        // 我将使用在 enemy_data.json 和 tower_data.json 中都存在的 "pixmap"
        return prototypes[type]["pixmap"].toString();
        // ------------------------------------------------
    }
    return QString();
}


void LevelEditorWidget::saveLevel() {

    // 步骤 1：检查防御塔数量 (这部分不变)
    int selectedTowerCount = 0;
    for (int i = 0; i < availableTowersListWidget->count(); ++i) {
        QJsonObject data = availableTowersListWidget->item(i)->data(Qt::UserRole).toJsonObject();
        QString type = data.value("type").toString();
        if (!type.isEmpty() && type != "None") {
            selectedTowerCount++;
        }
    }

    if (selectedTowerCount < 4) {
        QMessageBox::warning(this, "选择未完成", "请选择四座防御塔");
        return; // 终止函数
    }

    // 步骤 2：(修改) 移除文件对话框，强制使用 AppData 路径
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // 确保该目录存在
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath("."); // 递归创建目录
    }

    // 这是你的固定保存路径
    QString filePath = appDataPath + "/custom_level3.json";


    // 步骤 3：加载模板关卡 (level3.json) (这部分不变)
    QString templateLevelPath = getConfigFile("levels/level3.json");
    QFile templateFile(templateLevelPath);
    QJsonObject rootObj;
    if (templateFile.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(templateFile.readAll());
        if (doc.isObject()) {
            rootObj = doc.object();
        } else {
            QMessageBox::critical(this, "模板错误", QString("模板文件 %1 不是一个有效的 JSON 对象。").arg(templateLevelPath));
            return;
        }
        templateFile.close();
    } else {
        QMessageBox::critical(this, "模板错误",
            QString("无法加载关卡模板: %1\n\n请确保该文件已正确添加到 .qrc 资源文件中。").arg(templateLevelPath));
        return;
    }

    // 步骤 4：覆盖模板数据 (这部分不变)
    QJsonArray wavesArray;
    for (int i = 0; i < waveListWidget->count(); ++i) {
        QJsonObject waveObj;
        waveObj["enemies"] = waveListWidget->item(i)->data(Qt::UserRole).toJsonArray();
        wavesArray.append(waveObj);
    }
    // 附加 Boss 波次 (不变)
    QJsonObject bossWave;
    QJsonArray bossEnemies;
    QJsonObject nightmareEnemy;
    nightmareEnemy["type"] = "nightmare";
    nightmareEnemy["count"] = 1;
    nightmareEnemy["interval"] = 0.0;
    bossEnemies.append(nightmareEnemy);
    bossWave["enemies"] = bossEnemies;
    wavesArray.append(bossWave);
    rootObj["waves"] = wavesArray;

    // 覆盖 "available_towers" (不变)
    QJsonArray towersArray;
    for (int i = 0; i < availableTowersListWidget->count(); ++i) {
        QJsonObject data = availableTowersListWidget->item(i)->data(Qt::UserRole).toJsonObject();
        QString type = data.value("type").toString();
        if (!type.isEmpty() && type != "None") {
            towersArray.append(type);
        }
    }
    rootObj["available_towers"] = towersArray;

    // 覆盖 "available_enemies" (不变)
    QJsonArray enemiesArray;
    for (const QString& enemyType : m_enemyPrototypes.keys()) {
        enemiesArray.append(enemyType);
    }
    rootObj["available_enemies"] = enemiesArray;

    // 步骤 5：写入新文件 (现在写入的是 AppData 路径)
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(rootObj).toJson(QJsonDocument::Indented));
        file.close();

        // 发出信号并关闭 (不变)
        emit levelEditingFinished(filePath);
        this->close();
    } else {
        QMessageBox::critical(this, "保存失败", "无法写入目标文件。");
    }
}

void LevelEditorWidget::loadLevel() {
    QString filePath = QFileDialog::getOpenFileName(this, "Load Level", "", "JSON Files (*.json)");
    if (filePath.isEmpty()) return;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not open file.");
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject rootObj = doc.object();
    file.close();

    // 1. 加载波次 (Waves)
    waveListWidget->clear();
    QJsonArray wavesArray = rootObj["waves"].toArray();
    for (int i = 0; i < wavesArray.size(); ++i) {
        QJsonObject waveObj = wavesArray[i].toObject();
        QJsonArray enemiesArray = waveObj["enemies"].toArray();
        auto* item = new QListWidgetItem(QString("波次%1").arg(QString::number(i + 1)));
        item->setData(Qt::UserRole, enemiesArray);
        waveListWidget->addItem(item);
    }

    // 2. 加载可用防御塔 (Available Towers)
    for (int i = 0; i < 4; ++i) {
        QListWidgetItem* slotItem = availableTowersListWidget->item(i);
        slotItem->setText(QString("防御塔%1").arg(QString::number(i + 1)));
        slotItem->setData(Qt::UserRole, QJsonObject());
    }
    QJsonArray towersArray = rootObj["available_towers"].toArray();
    for(int i=0; i < towersArray.size() && i < 4; ++i) {
        QString type = towersArray[i].toString();
        if (m_towerPrototypes.contains(type)) {
            QListWidgetItem* slotItem = availableTowersListWidget->item(i);
            QJsonObject data;
            data["type"] = type;
            slotItem->setData(Qt::UserRole, data);

            // === UI 文本更新 ===
            QString name = m_towerPrototypes.value(type).value("name").toString(type);
            slotItem->setText(QString("防御塔%1: %2").arg(QString::number(i + 1)).arg(name));
            // =================
        }
    }

    // 3. (忽略) `available_enemies` (不变)

    // 4. 刷新UI (不变)
    if (waveListWidget->count() > 0) waveListWidget->setCurrentRow(0);
    onWaveSelectionChanged();
    if (availableTowersListWidget->count() > 0) availableTowersListWidget->setCurrentRow(0);
    onTowerSlotSelectionChanged();
}

void LevelEditorWidget::paintEvent(QPaintEvent* event) {
    // 首先调用基类的 paintEvent (如果需要，但这里我们完全覆盖)
    // QWidget::paintEvent(event);

    QPainter painter(this);

    if (!m_backgroundPixmap.isNull()) {

        // 1. 设置透明度 (例如 20%)
        //    这 *只* 影响 painter 之后的操作，*不会* 影响子控件
        painter.setOpacity(0.2);

        // 2. 绘制背景图片
        //    我们将图片强制拉伸以填满整个窗口
        //    (m_backgroundPixmap.rect() 是源, this->rect() 是目标)
        painter.drawPixmap(this->rect(), m_backgroundPixmap);

        // 当 'painter' 在函数结束时被销毁，透明度等设置会自动重置
    }
}

void LevelEditorWidget::loadLevelForEditing(const QString& filePath) {
    if (filePath.isEmpty()) return;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "无法打开关卡模板文件。");
        return;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        QMessageBox::critical(this, "Error", "关卡模板文件不是一个有效的 JSON 对象。");
        return;
    }
    QJsonObject rootObj = doc.object();

    // 1. 加载波次 (Waves)
    waveListWidget->clear();
    QJsonArray wavesArray = rootObj["waves"].toArray();
    for (int i = 0; i < wavesArray.size(); ++i) {
        QJsonObject waveObj = wavesArray[i].toObject();
        QJsonArray enemiesArray = waveObj["enemies"].toArray();

        // 【修复编辑器逻辑】: 不加载 nightmare，因为它会在保存时自动附加
        // 检查这个波次是否是 boss 波次
        bool isBossWave = false;
        if (enemiesArray.size() == 1) {
            if (enemiesArray[0].toObject().value("type").toString() == "nightmare") {
                isBossWave = true;
            }
        }

        if (!isBossWave) { // 只加载非 Boss 波次
            auto* item = new QListWidgetItem(QString("波次%1").arg(QString::number(waveListWidget->count() + 1)));
            item->setData(Qt::UserRole, enemiesArray);
            waveListWidget->addItem(item);
        }
    }

    // 2. 加载可用防御塔 (Available Towers)
    for (int i = 0; i < 4; ++i) {
        QListWidgetItem* slotItem = availableTowersListWidget->item(i);
        slotItem->setText(QString("防御塔%1").arg(QString::number(i + 1)));
        slotItem->setData(Qt::UserRole, QJsonObject());
    }
    QJsonArray towersArray = rootObj["available_towers"].toArray();
    for(int i=0; i < towersArray.size() && i < 4; ++i) {
        QString type = towersArray[i].toString();
        if (m_towerPrototypes.contains(type)) {
            QListWidgetItem* slotItem = availableTowersListWidget->item(i);
            QJsonObject data;
            data["type"] = type;
            slotItem->setData(Qt::UserRole, data);
            QString name = m_towerPrototypes.value(type).value("name").toString(type);
            slotItem->setText(QString("防御塔%1: %2").arg(QString::number(i + 1)).arg(name));
        }
    }

    // 3. 刷新UI
    if (waveListWidget->count() > 0) waveListWidget->setCurrentRow(0);
    onWaveSelectionChanged();
    if (availableTowersListWidget->count() > 0) availableTowersListWidget->setCurrentRow(0);
    onTowerSlotSelectionChanged();
}

