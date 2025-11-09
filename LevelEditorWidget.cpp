#include "LevelEditorWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTabWidget>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QFormLayout>

LevelEditorWidget::LevelEditorWidget(QWidget* parent) : QWidget(parent) {
    setupUI();

    // --- 通用信号 ---
    connect(saveButton, &QPushButton::clicked, this, &LevelEditorWidget::saveLevel);
    connect(loadButton, &QPushButton::clicked, this, &LevelEditorWidget::loadLevel);

    // --- 波次编辑器 Tab 信号 ---
    connect(addWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::addWave);
    connect(removeWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::removeWave);
    connect(waveListWidget, &QListWidget::currentItemChanged, this, &LevelEditorWidget::onWaveSelectionChanged);
    connect(addEnemyToWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::addEnemyToWave);
    connect(removeEnemyFromWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::removeEnemyFromWave);
    connect(enemyInWaveListWidget, &QListWidget::currentItemChanged, this, &LevelEditorWidget::onEnemyInWaveSelectionChanged);

    // "波次"详情编辑框信号
    connect(wave_enemyTypeLineEdit, &QLineEdit::textChanged, this, &LevelEditorWidget::updateSelectedEnemyInWave);
    // 数量和间隔的信号不再连接到 update 函数，因为它们是只读的

    // --- 游戏配置 Tab 信号 ---
    // 防御塔
    connect(addTowerButton, &QPushButton::clicked, this, &LevelEditorWidget::addAvailableTower);
    connect(removeTowerButton, &QPushButton::clicked, this, &LevelEditorWidget::removeAvailableTower);
    connect(availableTowersListWidget, &QListWidget::currentItemChanged, this, &LevelEditorWidget::onAvailableTowerChanged);

    // enemies
    connect(addEnemyButton, &QPushButton::clicked, this, &LevelEditorWidget::addAvailableEnemy);
    connect(removeEnemyButton, &QPushButton::clicked, this, &LevelEditorWidget::removeAvailableEnemy);
    connect(availableEnemiesListWidget, &QListWidget::currentItemChanged, this, &LevelEditorWidget::onAvailableEnemyChanged);

    onWaveSelectionChanged(); // 初始化状态
}

void LevelEditorWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    tabWidget = new QTabWidget(this);

    // 创建两个 Tab
    setupWaveTab();
    setupConfigTab();

    mainLayout->addWidget(tabWidget);

    // --- 底部按钮 ---
    auto* bottomButtonLayout = new QHBoxLayout();
    saveButton = new QPushButton("Save Level to File...");
    loadButton = new QPushButton("Load Level from File...");
    bottomButtonLayout->addStretch();
    bottomButtonLayout->addWidget(loadButton);
    bottomButtonLayout->addWidget(saveButton);
    mainLayout->addLayout(bottomButtonLayout);
}

void LevelEditorWidget::setupWaveTab() {
    waveTab = new QWidget();
    auto* waveTabLayout = new QHBoxLayout(waveTab);

    // 左侧：波次列表
    auto* waveGroup = new QGroupBox("Waves (Read-Only Structure)");
    auto* waveGroupLayout = new QVBoxLayout();
    waveListWidget = new QListWidget();
    auto* waveButtonLayout = new QHBoxLayout();
    addWaveButton = new QPushButton("Add Wave");
    removeWaveButton = new QPushButton("Remove Wave");
    waveButtonLayout->addWidget(addWaveButton);
    waveButtonLayout->addWidget(removeWaveButton);
    waveGroupLayout->addWidget(waveListWidget);
    waveGroupLayout->addLayout(waveButtonLayout);
    waveGroup->setLayout(waveGroupLayout);

    // 中间：当前波次的敌人列表
    auto* enemyGroup = new QGroupBox("Enemies in Selected Wave");
    auto* enemyGroupLayout = new QVBoxLayout();
    enemyInWaveListWidget = new QListWidget();
    auto* enemyButtonLayout = new QHBoxLayout();
    addEnemyToWaveButton = new QPushButton("Add Enemy");
    removeEnemyFromWaveButton = new QPushButton("Remove Enemy");
    enemyButtonLayout->addWidget(addEnemyToWaveButton);
    enemyButtonLayout->addWidget(removeEnemyFromWaveButton);
    enemyGroupLayout->addWidget(enemyInWaveListWidget);
    enemyGroupLayout->addLayout(enemyButtonLayout);
    enemyGroup->setLayout(enemyGroupLayout);

    // 右侧：选定敌人的详细信息
    auto* detailsGroup = new QGroupBox("Enemy Details (Fixed Count/Interval)");
    auto* detailsLayout = new QGridLayout();
    detailsLayout->addWidget(new QLabel("Type:"), 0, 0);
    wave_enemyTypeLineEdit = new QLineEdit();
    detailsLayout->addWidget(wave_enemyTypeLineEdit, 0, 1);

    detailsLayout->addWidget(new QLabel("Count (Fixed):"), 1, 0);
    wave_enemyCountSpinBox = new QSpinBox();
    wave_enemyCountSpinBox->setRange(1, 999);
    wave_enemyCountSpinBox->setReadOnly(true); // <-- 核心修改：设为只读
    detailsLayout->addWidget(wave_enemyCountSpinBox, 1, 1);

    detailsLayout->addWidget(new QLabel("Interval (Fixed):"), 2, 0);
    wave_enemyIntervalSpinBox = new QDoubleSpinBox();
    wave_enemyIntervalSpinBox->setRange(0.1, 60.0);
    wave_enemyIntervalSpinBox->setSingleStep(0.1);
    wave_enemyIntervalSpinBox->setReadOnly(true); // <-- 核心修改：设为只读
    detailsLayout->addWidget(wave_enemyIntervalSpinBox, 2, 1);
    detailsGroup->setLayout(detailsLayout);

    waveTabLayout->addWidget(waveGroup, 1);
    waveTabLayout->addWidget(enemyGroup, 1);
    waveTabLayout->addWidget(detailsGroup, 1);
    tabWidget->addTab(waveTab, "Wave Editor");
}

void LevelEditorWidget::setupConfigTab() {
    configTab = new QWidget();
    auto* configLayout = new QVBoxLayout(configTab);

    // --- 关卡名 ---
    auto* levelNameLayout = new QHBoxLayout();
    levelNameLayout->addWidget(new QLabel("Level Name:"));
    levelNameEdit = new QLineEdit("Custom Level");
    levelNameLayout->addWidget(levelNameEdit);
    configLayout->addLayout(levelNameLayout);

    // --- 可用防御塔 ---
    auto* towerGroup = new QGroupBox("Available Towers (Editable)");
    auto* towerGroupLayout = new QHBoxLayout();
    availableTowersListWidget = new QListWidget();
    towerGroupLayout->addWidget(availableTowersListWidget, 1);

    // 防御塔详情
    towerDetailsLayout = new QFormLayout();
    towerPropertyWidgets["type"] = new QLineEdit();
    towerPropertyWidgets["name"] = new QLineEdit();
    towerPropertyWidgets["cost"] = new QSpinBox();
    towerPropertyWidgets["pixmap"] = new QLineEdit();
    towerPropertyWidgets["bullet_pixmap"] = new QLineEdit();
    towerPropertyWidgets["damage"] = new QSpinBox();
    towerPropertyWidgets["range"] = new QDoubleSpinBox();
    towerPropertyWidgets["fire_rate"] = new QDoubleSpinBox();
    // 设置范围
    qobject_cast<QSpinBox*>(towerPropertyWidgets["cost"])->setRange(0, 9999);
    qobject_cast<QSpinBox*>(towerPropertyWidgets["damage"])->setRange(0, 9999);
    qobject_cast<QDoubleSpinBox*>(towerPropertyWidgets["range"])->setRange(0.0, 99.0);
    qobject_cast<QDoubleSpinBox*>(towerPropertyWidgets["fire_rate"])->setRange(0.0, 99.0);

    // 添加到Form
    for(const QString& key : towerPropertyWidgets.keys()) {
        towerDetailsLayout->addRow(key + ":", towerPropertyWidgets[key]);
        // 连接信号
        connect(qobject_cast<QLineEdit*>(towerPropertyWidgets[key]), &QLineEdit::textChanged, this, &LevelEditorWidget::updateSelectedTower);
        connect(qobject_cast<QSpinBox*>(towerPropertyWidgets[key]), QOverload<int>::of(&QSpinBox::valueChanged), this, &LevelEditorWidget::updateSelectedTower);
        connect(qobject_cast<QDoubleSpinBox*>(towerPropertyWidgets[key]), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LevelEditorWidget::updateSelectedTower);
    }
    towerGroupLayout->addLayout(towerDetailsLayout, 2);

    // 按钮
    auto* towerButtons = new QVBoxLayout();
    addTowerButton = new QPushButton("Add Tower");
    removeTowerButton = new QPushButton("Remove Tower");
    towerButtons->addWidget(addTowerButton);
    towerButtons->addWidget(removeTowerButton);
    towerGroupLayout->addLayout(towerButtons);
    towerGroup->setLayout(towerGroupLayout);
    configLayout->addWidget(towerGroup);

    // --- 可用敌人 ---
    auto* enemyGroup = new QGroupBox("Available Enemies (Editable)");
    auto* enemyGroupLayout = new QHBoxLayout();
    availableEnemiesListWidget = new QListWidget();
    enemyGroupLayout->addWidget(availableEnemiesListWidget, 1);

    // 敌人详情
    enemyDetailsLayout = new QFormLayout();
    enemyPropertyWidgets["type"] = new QLineEdit();
    enemyPropertyWidgets["name"] = new QLineEdit();
    enemyPropertyWidgets["pixmap"] = new QLineEdit();
    enemyPropertyWidgets["health"] = new QSpinBox();
    enemyPropertyWidgets["speed"] = new QDoubleSpinBox();
    enemyPropertyWidgets["damage"] = new QSpinBox();
    // 设置范围
    qobject_cast<QSpinBox*>(enemyPropertyWidgets["health"])->setRange(1, 99999);
    qobject_cast<QDoubleSpinBox*>(enemyPropertyWidgets["speed"])->setRange(0.1, 99.0);
    qobject_cast<QSpinBox*>(enemyPropertyWidgets["damage"])->setRange(0, 9999);

    for(const QString& key : enemyPropertyWidgets.keys()) {
        enemyDetailsLayout->addRow(key + ":", enemyPropertyWidgets[key]);
        // 连接信号
        connect(qobject_cast<QLineEdit*>(enemyPropertyWidgets[key]), &QLineEdit::textChanged, this, &LevelEditorWidget::updateSelectedEnemy);
        connect(qobject_cast<QSpinBox*>(enemyPropertyWidgets[key]), QOverload<int>::of(&QSpinBox::valueChanged), this, &LevelEditorWidget::updateSelectedEnemy);
        connect(qobject_cast<QDoubleSpinBox*>(enemyPropertyWidgets[key]), QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LevelEditorWidget::updateSelectedEnemy);
    }
    enemyGroupLayout->addLayout(enemyDetailsLayout, 2);

    // 按钮
    auto* enemyButtons = new QVBoxLayout();
    addEnemyButton = new QPushButton("Add Enemy");
    removeEnemyButton = new QPushButton("Remove Enemy");
    enemyButtons->addWidget(addEnemyButton);
    enemyButtons->addWidget(removeEnemyButton);
    enemyGroupLayout->addLayout(enemyButtons);
    enemyGroup->setLayout(enemyGroupLayout);
    configLayout->addWidget(enemyGroup);

    tabWidget->addTab(configTab, "Game Config (Editable)");
}

void LevelEditorWidget::addWave() {
    int waveCount = waveListWidget->count();
    auto* item = new QListWidgetItem(QString("Wave %1").arg(QString::number(waveCount + 1)));
    item->setData(Qt::UserRole, QJsonArray());
    waveListWidget->addItem(item);
    waveListWidget->setCurrentItem(item);
}

void LevelEditorWidget::removeWave() {
    delete waveListWidget->takeItem(waveListWidget->currentRow());
}

void LevelEditorWidget::onWaveSelectionChanged() {
    enemyInWaveListWidget->clear();
    clearEnemyInWaveDetails();

    QListWidgetItem* currentWave = waveListWidget->currentItem();
    if (!currentWave) {
        addEnemyToWaveButton->setEnabled(false);
        removeEnemyFromWaveButton->setEnabled(false);
        return;
    }

    addEnemyToWaveButton->setEnabled(true);

    QJsonArray enemies = currentWave->data(Qt::UserRole).toJsonArray();
    for (const QJsonValue& enemyValue : enemies) {
        QJsonObject enemyObj = enemyValue.toObject();
        QString type = enemyObj["type"].toString("default");
        int count = enemyObj["count"].toInt(1);
        auto* item = new QListWidgetItem(QString("%1 (x%2)").arg(type).arg(QString::number(count)));
        item->setData(Qt::UserRole, enemyObj);
        enemyInWaveListWidget->addItem(item);
    }

    onEnemyInWaveSelectionChanged();
}

void LevelEditorWidget::addEnemyToWave() {
    QListWidgetItem* currentWave = waveListWidget->currentItem();
    if (!currentWave) return;

    QJsonObject newEnemy;
    newEnemy["type"] = "new_enemy";
    newEnemy["count"] = 10;   // 固定的数量
    newEnemy["interval"] = 1.0; // 固定的间隔

    auto* item = new QListWidgetItem(QString("%1 (x%2)").arg("new_enemy").arg(QString::number(10)));
    item->setData(Qt::UserRole, newEnemy);
    enemyInWaveListWidget->addItem(item);

    // 更新wave的数据
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

    QJsonArray enemies = currentWave->data(Qt::UserRole).toJsonArray();
    enemies.removeAt(row);
    currentWave->setData(Qt::UserRole, enemies);
}

void LevelEditorWidget::onEnemyInWaveSelectionChanged() {
    QListWidgetItem* currentEnemy = enemyInWaveListWidget->currentItem();
    if (!currentEnemy) {
        clearEnemyInWaveDetails();
        wave_enemyTypeLineEdit->setEnabled(false);
        wave_enemyCountSpinBox->setEnabled(false);
        wave_enemyIntervalSpinBox->setEnabled(false);
        removeEnemyFromWaveButton->setEnabled(false);
        return;
    }

    wave_enemyTypeLineEdit->setEnabled(true);
    wave_enemyCountSpinBox->setEnabled(true); // 启用以显示
    wave_enemyIntervalSpinBox->setEnabled(true); // 启用以显示
    removeEnemyFromWaveButton->setEnabled(true);

    QJsonObject enemyObj = currentEnemy->data(Qt::UserRole).toJsonObject();

    wave_enemyTypeLineEdit->blockSignals(true);
    wave_enemyCountSpinBox->blockSignals(true);
    wave_enemyIntervalSpinBox->blockSignals(true);

    wave_enemyTypeLineEdit->setText(enemyObj["type"].toString());
    wave_enemyCountSpinBox->setValue(enemyObj["count"].toInt());
    wave_enemyIntervalSpinBox->setValue(enemyObj["interval"].toDouble());

    wave_enemyTypeLineEdit->blockSignals(false);
    wave_enemyCountSpinBox->blockSignals(false);
    wave_enemyIntervalSpinBox->blockSignals(false);
}

void LevelEditorWidget::clearEnemyInWaveDetails() {
    wave_enemyTypeLineEdit->clear();
    wave_enemyCountSpinBox->setValue(0);
    wave_enemyIntervalSpinBox->setValue(0.0);
}

void LevelEditorWidget::updateSelectedEnemyInWave() {
    QListWidgetItem* currentWave = waveListWidget->currentItem();
    QListWidgetItem* currentEnemyItem = enemyInWaveListWidget->currentItem();
    if (!currentWave || !currentEnemyItem) return;

    // 只更新 "type"，因为 count 和 interval 是只读的
    QJsonObject updatedEnemy = currentEnemyItem->data(Qt::UserRole).toJsonObject();
    updatedEnemy["type"] = wave_enemyTypeLineEdit->text();

    currentEnemyItem->setText(QString("%1 (x%2)").arg(updatedEnemy["type"].toString()).arg(QString::number(updatedEnemy["count"].toInt())));
    currentEnemyItem->setData(Qt::UserRole, updatedEnemy);

    int row = enemyInWaveListWidget->currentRow();
    QJsonArray enemies = currentWave->data(Qt::UserRole).toJsonArray();
    enemies[row] = updatedEnemy;
    currentWave->setData(Qt::UserRole, enemies);
}


void LevelEditorWidget::saveLevel() {
    QString filePath = QFileDialog::getSaveFileName(this, "Save Level", "", "JSON Files (*.json)");
    if (filePath.isEmpty()) {
        return;
    }

    QJsonObject rootObj;
    rootObj["level_name"] = levelNameEdit->text();

    // 1. 保存波次 (Waves) - (从Wave Tab)
    QJsonArray wavesArray;
    for (int i = 0; i < waveListWidget->count(); ++i) {
        QListWidgetItem* waveItem = waveListWidget->item(i);
        QJsonObject waveObj;
        waveObj["enemies"] = waveItem->data(Qt::UserRole).toJsonArray();
        wavesArray.append(waveObj);
    }
    rootObj["waves"] = wavesArray;

    // 2. 保存可用防御塔 (Available Towers) - (从Config Tab)
    QJsonArray towersArray;
    for (int i = 0; i < availableTowersListWidget->count(); ++i) {
        towersArray.append(availableTowersListWidget->item(i)->data(Qt::UserRole).toJsonObject());
    }
    rootObj["available_towers"] = towersArray;

    // 3. 保存可用敌人 (Available Enemies) - (从Config Tab)
    QJsonArray enemiesArray;
    for (int i = 0; i < availableEnemiesListWidget->count(); ++i) {
        enemiesArray.append(availableEnemiesListWidget->item(i)->data(Qt::UserRole).toJsonObject());
    }
    rootObj["available_enemies"] = enemiesArray;


    // 4. (待办) 保存地图和玩家信息 - 暂时还是占位符
    QJsonObject mapObj;
    mapObj["background"] = "path/to/your/background.png";
    mapObj["path"] = QJsonArray(); // Placeholder
    mapObj["tower_positions"] = QJsonArray(); // Placeholder
    rootObj["map"] = mapObj;

    QJsonObject playerObj;
    playerObj["initial_stability"] = 20; // Placeholder
    playerObj["initial_resource"] = 100; // Placeholder
    rootObj["player"] = playerObj;


    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(rootObj).toJson(QJsonDocument::Indented));
        file.close();
    } else {
        QMessageBox::critical(this, "Error", "Could not save file.");
    }
}

void LevelEditorWidget::loadLevel() {
    QString filePath = QFileDialog::getOpenFileName(this, "Load Level", "", "JSON Files (*.json)");
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Error", "Could not open file.");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject rootObj = doc.object();
    file.close();

    levelNameEdit->setText(rootObj["level_name"].toString("Custom Level"));

    // 1. 加载波次 (Waves) - (到 Wave Tab)
    waveListWidget->clear();
    QJsonArray wavesArray = rootObj["waves"].toArray();
    for (int i = 0; i < wavesArray.size(); ++i) {
        QJsonObject waveObj = wavesArray[i].toObject();
        QJsonArray enemiesArray = waveObj["enemies"].toArray();

        auto* item = new QListWidgetItem(QString("Wave %1").arg(QString::number(i + 1)));
        item->setData(Qt::UserRole, enemiesArray);
        waveListWidget->addItem(item);
    }
    if (waveListWidget->count() > 0) waveListWidget->setCurrentRow(0);
    onWaveSelectionChanged();

    // 2. 加载可用防御塔 (Available Towers) - (到 Config Tab)
    availableTowersListWidget->clear();
    QJsonArray towersArray = rootObj["available_towers"].toArray();
    for(const QJsonValue& val : towersArray) {
        QJsonObject obj = val.toObject();
        auto* item = new QListWidgetItem(obj["type"].toString());
        item->setData(Qt::UserRole, obj);
        availableTowersListWidget->addItem(item);
    }
    if (availableTowersListWidget->count() > 0) availableTowersListWidget->setCurrentRow(0);
    onAvailableTowerChanged();

    // 3. 加载可用敌人 (Available Enemies) - (到 Config Tab)
    availableEnemiesListWidget->clear();
    QJsonArray enemiesArray = rootObj["available_enemies"].toArray();
    for(const QJsonValue& val : enemiesArray) {
        QJsonObject obj = val.toObject();
        auto* item = new QListWidgetItem(obj["type"].toString());
        item->setData(Qt::UserRole, obj);
        availableEnemiesListWidget->addItem(item);
    }
    if (availableEnemiesListWidget->count() > 0) availableEnemiesListWidget->setCurrentRow(0);
    onAvailableEnemyChanged();
}


// --- Config Tab: Tower Slots ---

void LevelEditorWidget::addAvailableTower() {
    QJsonObject newTower;
    newTower["type"] = "NewTower";
    newTower["name"] = "New Tower";
    newTower["cost"] = 100;

    auto* item = new QListWidgetItem(newTower["type"].toString());
    item->setData(Qt::UserRole, newTower);
    availableTowersListWidget->addItem(item);
    availableTowersListWidget->setCurrentItem(item);
}

void LevelEditorWidget::removeAvailableTower() {
    delete availableTowersListWidget->takeItem(availableTowersListWidget->currentRow());
}

void LevelEditorWidget::onAvailableTowerChanged() {
    QListWidgetItem* item = availableTowersListWidget->currentItem();
    if (!item) {
        clearTowerDetails();
        return;
    }

    QJsonObject obj = item->data(Qt::UserRole).toJsonObject();

    // 暂停信号
    for(QWidget* w : towerPropertyWidgets) w->blockSignals(true);

    qobject_cast<QLineEdit*>(towerPropertyWidgets["type"])->setText(obj["type"].toString());
    qobject_cast<QLineEdit*>(towerPropertyWidgets["name"])->setText(obj["name"].toString());
    qobject_cast<QSpinBox*>(towerPropertyWidgets["cost"])->setValue(obj["cost"].toInt());
    qobject_cast<QLineEdit*>(towerPropertyWidgets["pixmap"])->setText(obj["pixmap"].toString());
    qobject_cast<QLineEdit*>(towerPropertyWidgets["bullet_pixmap"])->setText(obj["bullet_pixmap"].toString());
    qobject_cast<QSpinBox*>(towerPropertyWidgets["damage"])->setValue(obj["damage"].toInt());
    qobject_cast<QDoubleSpinBox*>(towerPropertyWidgets["range"])->setValue(obj["range"].toDouble());
    qobject_cast<QDoubleSpinBox*>(towerPropertyWidgets["fire_rate"])->setValue(obj["fire_rate"].toDouble());

    // 恢复信号
    for(QWidget* w : towerPropertyWidgets) w->blockSignals(false);
}

void LevelEditorWidget::updateSelectedTower() {
    QListWidgetItem* item = availableTowersListWidget->currentItem();
    if (!item) return;

    QJsonObject obj;
    obj["type"] = qobject_cast<QLineEdit*>(towerPropertyWidgets["type"])->text();
    obj["name"] = qobject_cast<QLineEdit*>(towerPropertyWidgets["name"])->text();
    obj["cost"] = qobject_cast<QSpinBox*>(towerPropertyWidgets["cost"])->value();
    obj["pixmap"] = qobject_cast<QLineEdit*>(towerPropertyWidgets["pixmap"])->text();
    obj["bullet_pixmap"] = qobject_cast<QLineEdit*>(towerPropertyWidgets["bullet_pixmap"])->text();
    obj["damage"] = qobject_cast<QSpinBox*>(towerPropertyWidgets["damage"])->value();
    obj["range"] = qobject_cast<QDoubleSpinBox*>(towerPropertyWidgets["range"])->value();
    obj["fire_rate"] = qobject_cast<QDoubleSpinBox*>(towerPropertyWidgets["fire_rate"])->value();

    item->setText(obj["type"].toString());
    item->setData(Qt::UserRole, obj);
}

void LevelEditorWidget::clearTowerDetails() {
    for(QWidget* w : towerPropertyWidgets) {
        if(auto* le = qobject_cast<QLineEdit*>(w)) le->clear();
        if(auto* sb = qobject_cast<QSpinBox*>(w)) sb->setValue(0);
        if(auto* dsb = qobject_cast<QDoubleSpinBox*>(w)) dsb->setValue(0.0);
    }
}


// --- Config Tab: Enemy Slots ---

void LevelEditorWidget::addAvailableEnemy() {
    QJsonObject newEnemy;
    newEnemy["type"] = "NewEnemy";
    newEnemy["name"] = "New Enemy";
    newEnemy["health"] = 100;

    auto* item = new QListWidgetItem(newEnemy["type"].toString());
    item->setData(Qt::UserRole, newEnemy);
    availableEnemiesListWidget->addItem(item);
    availableEnemiesListWidget->setCurrentItem(item);
}

void LevelEditorWidget::removeAvailableEnemy() {
    delete availableEnemiesListWidget->takeItem(availableEnemiesListWidget->currentRow());
}

void LevelEditorWidget::onAvailableEnemyChanged() {
    QListWidgetItem* item = availableEnemiesListWidget->currentItem();
    if (!item) {
        clearEnemyDetails();
        return;
    }

    QJsonObject obj = item->data(Qt::UserRole).toJsonObject();

    for(QWidget* w : enemyPropertyWidgets) w->blockSignals(true);

    qobject_cast<QLineEdit*>(enemyPropertyWidgets["type"])->setText(obj["type"].toString());
    qobject_cast<QLineEdit*>(enemyPropertyWidgets["name"])->setText(obj["name"].toString());
    qobject_cast<QLineEdit*>(enemyPropertyWidgets["pixmap"])->setText(obj["pixmap"].toString());
    qobject_cast<QSpinBox*>(enemyPropertyWidgets["health"])->setValue(obj["health"].toInt());
    qobject_cast<QDoubleSpinBox*>(enemyPropertyWidgets["speed"])->setValue(obj["speed"].toDouble());
    qobject_cast<QSpinBox*>(enemyPropertyWidgets["damage"])->setValue(obj["damage"].toInt());

    for(QWidget* w : enemyPropertyWidgets) w->blockSignals(false);
}

void LevelEditorWidget::updateSelectedEnemy() {
    QListWidgetItem* item = availableEnemiesListWidget->currentItem();
    if (!item) return;

    QJsonObject obj;
    obj["type"] = qobject_cast<QLineEdit*>(enemyPropertyWidgets["type"])->text();
    obj["name"] = qobject_cast<QLineEdit*>(enemyPropertyWidgets["name"])->text();
    obj["pixmap"] = qobject_cast<QLineEdit*>(enemyPropertyWidgets["pixmap"])->text();
    obj["health"] = qobject_cast<QSpinBox*>(enemyPropertyWidgets["health"])->value();
    obj["speed"] = qobject_cast<QDoubleSpinBox*>(enemyPropertyWidgets["speed"])->value();
    obj["damage"] = qobject_cast<QSpinBox*>(enemyPropertyWidgets["damage"])->value();

    item->setText(obj["type"].toString());
    item->setData(Qt::UserRole, obj);
}

void LevelEditorWidget::clearEnemyDetails() {
     for(QWidget* w : enemyPropertyWidgets) {
        if(auto* le = qobject_cast<QLineEdit*>(w)) le->clear();
        if(auto* sb = qobject_cast<QSpinBox*>(w)) sb->setValue(0);
        if(auto* dsb = qobject_cast<QDoubleSpinBox*>(w)) dsb->setValue(0.0);
    }
}