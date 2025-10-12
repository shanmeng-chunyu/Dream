#include "LevelEditorWidget.h"
#include <QtWidgets> // 包含所有标准控件的头文件
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <algorithm> // for std::remove_if

LevelEditorWidget::LevelEditorWidget(QWidget *parent) :
    QWidget(parent),
    currentMode(EditMode::PlacePath), // 默认是放置路径模式
    gridSize(20, 15),
    currentMouseGridPos(-1, -1)
{
    setupUI();
    setMouseTracking(true); // 开启鼠标跟踪以实时更新预览
    // 为编辑器设置一个合适的默认大小和最小大小
    setMinimumSize(800, 600);
    setWindowTitle("梦境守护者 - 关卡编辑器");
}

void LevelEditorWidget::setupUI() {
    // --- 1. 创建控件 ---
    loadButton = new QPushButton("加载关卡");
    saveButton = new QPushButton("保存关卡");
    statusLabel = new QLabel("当前模式: 放置路径");
    statusLabel->setFixedHeight(30);

    // 创建一个按钮组，确保单选按钮的互斥性
    QButtonGroup* modeGroup = new QButtonGroup(this);
    pathModeButton = new QRadioButton("放置路径");
    towerModeButton = new QRadioButton("放置塔基");
    eraseModeButton = new QRadioButton("橡皮擦");
    pathModeButton->setChecked(true); // 默认选中

    modeGroup->addButton(pathModeButton);
    modeGroup->addButton(towerModeButton);
    modeGroup->addButton(eraseModeButton);

    // --- 2. 创建布局 ---
    // 右侧的控制面板布局
    QVBoxLayout* controlPanelLayout = new QVBoxLayout();
    controlPanelLayout->addWidget(loadButton);
    controlPanelLayout->addWidget(saveButton);
    controlPanelLayout->addSpacing(20);
    controlPanelLayout->addWidget(new QLabel("编辑模式:"));
    controlPanelLayout->addWidget(pathModeButton);
    controlPanelLayout->addWidget(towerModeButton);
    controlPanelLayout->addWidget(eraseModeButton);
    controlPanelLayout->addStretch(); // 添加伸缩，让控件都靠上
    controlPanelLayout->addWidget(statusLabel);


    // 创建一个QWidget作为控制面板的容器
    QWidget* controlPanel = new QWidget();
    controlPanel->setLayout(controlPanelLayout);
    controlPanel->setFixedWidth(200); // 固定控制面板宽度

    // 窗口主布局
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addStretch(); // 左侧的地图绘制区域，先用伸缩项占位
    mainLayout->addWidget(controlPanel); // 右侧是控制面板

    // --- 3. 连接信号槽 ---
    connect(loadButton, &QPushButton::clicked, this, &LevelEditorWidget::onLoadButtonPressed);
    connect(saveButton, &QPushButton::clicked, this, &LevelEditorWidget::onSaveButtonPressed);
    // 连接按钮组的信号，任意按钮被点击都会触发
    connect(modeGroup, &QButtonGroup::buttonClicked, this, &LevelEditorWidget::onModeChanged);
}


void LevelEditorWidget::onModeChanged() {
    if (pathModeButton->isChecked()) {
        currentMode = EditMode::PlacePath;
        statusLabel->setText("当前模式: 放置路径");
    } else if (towerModeButton->isChecked()) {
        currentMode = EditMode::PlaceTowerBase;
        statusLabel->setText("当前模式: 放置塔基");
    } else if (eraseModeButton->isChecked()) {
        currentMode = EditMode::Erase;
        statusLabel->setText("当前模式: 橡皮擦");
    }
    update(); // 切换模式后重绘，以更新预览颜色等
}

void LevelEditorWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 编辑区域的矩形范围 (窗口总宽度减去控制面板宽度)
    QRect drawArea(0, 0, this->width() - 200, this->height());

    float cellWidth = (float)drawArea.width() / gridSize.width();
    float cellHeight = (float)drawArea.height() / gridSize.height();

    // 绘制背景 (可选，用浅灰色填充)
    painter.fillRect(drawArea, QColor(240, 240, 240));

    // 绘制网格线
    painter.setPen(QColor(200, 200, 200));
    for (int i = 0; i <= gridSize.width(); ++i) {
        painter.drawLine(i * cellWidth, 0, i * cellWidth, drawArea.height());
    }
    for (int i = 0; i <= gridSize.height(); ++i) {
        painter.drawLine(0, i * cellHeight, drawArea.width(), i * cellHeight);
    }

    // 绘制已放置的路径点
    painter.setBrush(QColor(100, 150, 255, 200)); // 蓝色
    painter.setPen(Qt::NoPen);
    for (const auto& p : path) {
        QRectF cellRect(p.x() * cellWidth, p.y() * cellHeight, cellWidth, cellHeight);
        painter.drawRect(cellRect);
    }

    // 绘制已放置的塔基
    painter.setBrush(QColor(255, 200, 100, 200)); // 橙色
    for (const auto& b : towerBases) {
        QRectF cellRect(b.x() * cellWidth, b.y() * cellHeight, cellWidth, cellHeight);
        painter.drawEllipse(cellRect.adjusted(4, 4, -4, -4)); // 画个小圆代表塔基
    }

    // 绘制鼠标悬停预览
    if (currentMouseGridPos.x() >= 0 && currentMouseGridPos.y() >= 0) {
        QColor previewColor;
        switch (currentMode) {
            case EditMode::PlacePath:      previewColor = QColor(100, 150, 255, 100); break;
            case EditMode::PlaceTowerBase: previewColor = QColor(255, 200, 100, 100); break;
            case EditMode::Erase:          previewColor = QColor(255, 100, 100, 150); break;
        }
        painter.setBrush(previewColor);
        QRectF previewRect(currentMouseGridPos.x() * cellWidth, currentMouseGridPos.y() * cellHeight, cellWidth, cellHeight);
        painter.drawRect(previewRect);
    }
}

void LevelEditorWidget::mousePressEvent(QMouseEvent *event) {
    QRect drawArea(0, 0, this->width() - 200, this->height());
    if (!drawArea.contains(event->pos())) return; // 点击在控制面板上，忽略

    float cellWidth = (float)drawArea.width() / gridSize.width();
    float cellHeight = (float)drawArea.height() / gridSize.height();

    int col = event->pos().x() / cellWidth;
    int row = event->pos().y() / cellHeight;
    QPoint gridPos(col, row);

    switch (currentMode) {
        case EditMode::PlacePath:
            // 防止重复添加
            if (std::find(path.begin(), path.end(), gridPos) == path.end()) {
                path.push_back(gridPos);
            }
            break;
        case EditMode::PlaceTowerBase:
            // 防止重复添加
            if (std::find(towerBases.begin(), towerBases.end(), gridPos) == towerBases.end()) {
                towerBases.push_back(gridPos);
            }
            break;
        case EditMode::Erase:
            // 移除路径点
            path.erase(std::remove_if(path.begin(), path.end(),
                [&](const QPoint& p){ return p == gridPos; }), path.end());
            // 移除塔基
            towerBases.erase(std::remove_if(towerBases.begin(), towerBases.end(),
                [&](const QPoint& b){ return b == gridPos; }), towerBases.end());
            break;
    }
    update(); // 请求重绘
}

void LevelEditorWidget::mouseMoveEvent(QMouseEvent* event) {
    QRect drawArea(0, 0, this->width() - 200, this->height());
    if (drawArea.contains(event->pos())) {
        float cellWidth = (float)drawArea.width() / gridSize.width();
        float cellHeight = (float)drawArea.height() / gridSize.height();
        int col = event->pos().x() / cellWidth;
        int row = event->pos().y() / cellHeight;
        QPoint newPos(col, row);
        if (newPos != currentMouseGridPos) {
            currentMouseGridPos = newPos;
            update(); // 只有当鼠标移动到新的格子时才重绘
        }
    } else {
        if (currentMouseGridPos.x() != -1) {
            currentMouseGridPos = QPoint(-1, -1); // 移出绘制区
            update();
        }
    }
}


// --- save/load 函数保持不变 ---
void LevelEditorWidget::onSaveButtonPressed() {
    QString filePath = QFileDialog::getSaveFileName(this, "保存关卡", "", "JSON Files (*.json)");
    if (filePath.isEmpty()) return;
    saveLevelToFile(filePath);
}

void LevelEditorWidget::onLoadButtonPressed() {
    QString filePath = QFileDialog::getOpenFileName(this, "加载关卡", "", "JSON Files (*.json)");
    if (filePath.isEmpty()) return;
    loadLevelFromFile(filePath);
}

void LevelEditorWidget::saveLevelToFile(const QString& filePath) {
    QJsonObject rootObj;
    rootObj["backgroundPath"] = backgroundPath;
    QJsonObject gridSizeObj;
    gridSizeObj["cols"] = gridSize.width();
    gridSizeObj["rows"] = gridSize.height();
    rootObj["gridSize"] = gridSizeObj;
    QJsonArray pathArray;
    for (const auto& point : path) {
        QJsonObject pointObj;
        pointObj["col"] = point.x();
        pointObj["row"] = point.y();
        pathArray.append(pointObj);
    }
    rootObj["pathPoints"] = pathArray;
    QJsonArray towerBasesArray;
    for (const auto& base : towerBases) {
        QJsonObject baseObj;
        baseObj["col"] = base.x();
        baseObj["row"] = base.y();
        towerBasesArray.append(baseObj);
    }
    rootObj["towerBases"] = towerBasesArray;
    QJsonArray wavesArray;
    for (const auto& wave : waves) {
        QJsonObject waveObj;
        waveObj["enemyType"] = wave.enemyType;
        waveObj["enemyCount"] = wave.enemyCount;
        waveObj["spawnInterval"] = wave.spawnInterval;
        wavesArray.append(waveObj);
    }
    rootObj["waves"] = wavesArray;
    QJsonDocument doc(rootObj);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson(QJsonDocument::Indented)); // 使用Indented格式，更易读
        file.close();
        qInfo() << "关卡已成功保存至" << filePath;
    } else {
        qWarning() << "无法保存关卡至" << filePath;
    }
}

void LevelEditorWidget::loadLevelFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开关卡文件:" << filePath;
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "解析JSON失败或文件不是一个JSON对象:" << filePath;
        return;
    }
    QJsonObject rootObj = jsonDoc.object();

    // 清空当前数据
    path.clear();
    towerBases.clear();
    waves.clear();

    backgroundPath = rootObj["backgroundPath"].toString();
    QJsonObject gridSizeObj = rootObj["gridSize"].toObject();
    gridSize.setWidth(gridSizeObj["cols"].toInt());
    gridSize.setHeight(gridSizeObj["rows"].toInt());

    QJsonArray pathArray = rootObj["pathPoints"].toArray();
    for (const QJsonValue& val : pathArray) {
        QJsonObject pointObj = val.toObject();
        path.emplace_back(pointObj["col"].toInt(), pointObj["row"].toInt());
    }

    QJsonArray towerBasesArray = rootObj["towerBases"].toArray();
    for (const QJsonValue& val : towerBasesArray) {
        QJsonObject pointObj = val.toObject();
        towerBases.emplace_back(pointObj["col"].toInt(), pointObj["row"].toInt());
    }

    QJsonArray wavesArray = rootObj["waves"].toArray();
    for (const QJsonValue& val : wavesArray) {
        QJsonObject waveObj = val.toObject();
        waves.push_back({
            waveObj["enemyType"].toInt(),
            waveObj["enemyCount"].toInt(),
            waveObj["spawnInterval"].toInt()
        });
    }

    qInfo() << "关卡" << filePath << "已成功加载。";
    update(); // 强制重绘窗口以显示新加载的关卡
}

