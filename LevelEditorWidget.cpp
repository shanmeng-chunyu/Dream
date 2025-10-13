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

LevelEditorWidget::LevelEditorWidget(QWidget* parent) : QWidget(parent) {
    setupUI();

    // �����źźͲ�
    connect(saveButton, &QPushButton::clicked, this, &LevelEditorWidget::saveLevel);
    connect(loadButton, &QPushButton::clicked, this, &LevelEditorWidget::loadLevel);
    connect(addWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::addWave);
    connect(removeWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::removeWave);
    connect(waveListWidget, &QListWidget::currentItemChanged, this, &LevelEditorWidget::onWaveSelectionChanged);
    connect(addEnemyToWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::addEnemyToWave);
    connect(removeEnemyFromWaveButton, &QPushButton::clicked, this, &LevelEditorWidget::removeEnemyFromWave);
    connect(enemyInWaveListWidget, &QListWidget::currentItemChanged, this, &LevelEditorWidget::onEnemySelectionChanged);

    // ���ӵ�������༭����źţ�ʵ���Զ�����
    connect(enemyTypeLineEdit, &QLineEdit::textChanged, this, &LevelEditorWidget::updateSelectedEnemy);
    connect(enemyCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &LevelEditorWidget::updateSelectedEnemy);
    connect(enemyIntervalSpinBox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &LevelEditorWidget::updateSelectedEnemy);

    onWaveSelectionChanged(); // ��ʼ��״̬
}

void LevelEditorWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    tabWidget = new QTabWidget(this);

    // --- �������α༭�� Tab ---
    waveTab = new QWidget();
    auto* waveTabLayout = new QHBoxLayout(waveTab);

    // ��ࣺ�����б�
    auto* waveGroup = new QGroupBox("Waves");
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

    // �м䣺��ǰ���εĵ����б�
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

    // �Ҳࣺѡ�����˵���ϸ��Ϣ
    auto* detailsGroup = new QGroupBox("Enemy Details");
    auto* detailsLayout = new QGridLayout();
    detailsLayout->addWidget(new QLabel("Type:"), 0, 0);
    enemyTypeLineEdit = new QLineEdit();
    detailsLayout->addWidget(enemyTypeLineEdit, 0, 1);
    detailsLayout->addWidget(new QLabel("Count:"), 1, 0);
    enemyCountSpinBox = new QSpinBox();
    enemyCountSpinBox->setRange(1, 999);
    detailsLayout->addWidget(enemyCountSpinBox, 1, 1);
    detailsLayout->addWidget(new QLabel("Interval (s):"), 2, 0);
    enemyIntervalSpinBox = new QDoubleSpinBox();
    enemyIntervalSpinBox->setRange(0.1, 60.0);
    enemyIntervalSpinBox->setSingleStep(0.1);
    detailsLayout->addWidget(enemyIntervalSpinBox, 2, 1);
    detailsGroup->setLayout(detailsLayout);

    waveTabLayout->addWidget(waveGroup, 1);
    waveTabLayout->addWidget(enemyGroup, 1);
    waveTabLayout->addWidget(detailsGroup, 1);
    tabWidget->addTab(waveTab, "Wave Editor");

    // --- �����ؿ����� Tab ---
    configTab = new QWidget();
    auto* configLayout = new QGridLayout(configTab);
    configLayout->addWidget(new QLabel("Level Name:"), 0, 0);
    levelNameEdit = new QLineEdit("Custom Level");
    configLayout->addWidget(levelNameEdit, 0, 1);
    // �˴�������Ӹ�����������ʼ��Դ������ֵ���Լ����õķ������͵����б�
    configLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding), 1, 0);
    tabWidget->addTab(configTab, "Game Config");

    mainLayout->addWidget(tabWidget);

    // --- �ײ���ť ---
    auto* bottomButtonLayout = new QHBoxLayout();
    saveButton = new QPushButton("Save Level to File...");
    loadButton = new QPushButton("Load Level from File...");
    bottomButtonLayout->addStretch();
    bottomButtonLayout->addWidget(loadButton);
    bottomButtonLayout->addWidget(saveButton);
    mainLayout->addLayout(bottomButtonLayout);
}

void LevelEditorWidget::addWave() {
    int waveCount = waveListWidget->count();
    auto* item = new QListWidgetItem(QString("Wave %1").arg(QString::number(waveCount + 1)));
    // ʹ��setData�洢һ��QJsonArray����ʾ��һ���ĵ���
    item->setData(Qt::UserRole, QJsonArray());
    waveListWidget->addItem(item);
    waveListWidget->setCurrentItem(item);
}

void LevelEditorWidget::removeWave() {
    delete waveListWidget->takeItem(waveListWidget->currentRow());
}

void LevelEditorWidget::onWaveSelectionChanged() {
    enemyInWaveListWidget->clear();
    clearEnemyDetails();

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

    onEnemySelectionChanged();
}

void LevelEditorWidget::addEnemyToWave() {
    QListWidgetItem* currentWave = waveListWidget->currentItem();
    if (!currentWave) return;

    QJsonObject newEnemy;
    newEnemy["type"] = "new_enemy";
    newEnemy["count"] = 10;
    newEnemy["interval"] = 1.0;

    auto* item = new QListWidgetItem(QString("%1 (x%2)").arg("new_enemy").arg(QString::number(10)));
    item->setData(Qt::UserRole, newEnemy);
    enemyInWaveListWidget->addItem(item);

    // ����wave������
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

void LevelEditorWidget::onEnemySelectionChanged() {
    QListWidgetItem* currentEnemy = enemyInWaveListWidget->currentItem();
    if (!currentEnemy) {
        clearEnemyDetails();
        enemyTypeLineEdit->setEnabled(false);
        enemyCountSpinBox->setEnabled(false);
        enemyIntervalSpinBox->setEnabled(false);
        removeEnemyFromWaveButton->setEnabled(false);
        return;
    }

    enemyTypeLineEdit->setEnabled(true);
    enemyCountSpinBox->setEnabled(true);
    enemyIntervalSpinBox->setEnabled(true);
    removeEnemyFromWaveButton->setEnabled(true);

    QJsonObject enemyObj = currentEnemy->data(Qt::UserRole).toJsonObject();

    // ��ʱ�Ͽ����ӣ������������ʱ��������ѭ��
    enemyTypeLineEdit->blockSignals(true);
    enemyCountSpinBox->blockSignals(true);
    enemyIntervalSpinBox->blockSignals(true);

    enemyTypeLineEdit->setText(enemyObj["type"].toString());
    enemyCountSpinBox->setValue(enemyObj["count"].toInt());
    enemyIntervalSpinBox->setValue(enemyObj["interval"].toDouble());

    // �ָ�����
    enemyTypeLineEdit->blockSignals(false);
    enemyCountSpinBox->blockSignals(false);
    enemyIntervalSpinBox->blockSignals(false);
}

void LevelEditorWidget::clearEnemyDetails() {
    enemyTypeLineEdit->clear();
    enemyCountSpinBox->setValue(0);
    enemyIntervalSpinBox->setValue(0.0);
}

void LevelEditorWidget::updateSelectedEnemy() {
    QListWidgetItem* currentWave = waveListWidget->currentItem();
    QListWidgetItem* currentEnemyItem = enemyInWaveListWidget->currentItem();
    if (!currentWave || !currentEnemyItem) return;

    QJsonObject updatedEnemy;
    updatedEnemy["type"] = enemyTypeLineEdit->text();
    updatedEnemy["count"] = enemyCountSpinBox->value();
    updatedEnemy["interval"] = enemyIntervalSpinBox->value();

    // �����б������ʾ�ı�������
    currentEnemyItem->setText(QString("%1 (x%2)").arg(updatedEnemy["type"].toString()).arg(QString::number(updatedEnemy["count"].toInt())));
    currentEnemyItem->setData(Qt::UserRole, updatedEnemy);

    // ���¸���Wave������
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

    // ע�⣺��ǰ�汾�ı༭�����ṩ��ͼ�༭����
    // ��������ֻ���沨����Ϣ����ͼ��Ϣ��Ҫ�ֶ��������������
    QJsonObject mapObj;
    mapObj["background"] = "path/to/your/background.png";
    mapObj["path"] = QJsonArray(); // Placeholder
    mapObj["tower_positions"] = QJsonArray(); // Placeholder
    rootObj["map"] = mapObj;

    QJsonObject playerObj;
    playerObj["initial_stability"] = 20; // Placeholder
    playerObj["initial_resource"] = 100; // Placeholder
    rootObj["player"] = playerObj;

    QJsonArray wavesArray;
    for (int i = 0; i < waveListWidget->count(); ++i) {
        QListWidgetItem* waveItem = waveListWidget->item(i);
        QJsonObject waveObj;
        waveObj["enemies"] = waveItem->data(Qt::UserRole).toJsonArray();
        wavesArray.append(waveObj);
    }
    rootObj["waves"] = wavesArray;

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

    waveListWidget->clear();
    QJsonArray wavesArray = rootObj["waves"].toArray();
    for (int i = 0; i < wavesArray.size(); ++i) {
        QJsonObject waveObj = wavesArray[i].toObject();
        QJsonArray enemiesArray = waveObj["enemies"].toArray();

        auto* item = new QListWidgetItem(QString("Wave %1").arg(QString::number(i + 1)));
        item->setData(Qt::UserRole, enemiesArray);
        waveListWidget->addItem(item);
    }

    if (waveListWidget->count() > 0) {
        waveListWidget->setCurrentRow(0);
    }
    onWaveSelectionChanged();
}
