#include "LevelEditorWidget.h"
#include <QtWidgets> // �������б�׼�ؼ���ͷ�ļ�
#include <QFileDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDebug>
#include <algorithm> // for std::remove_if

LevelEditorWidget::LevelEditorWidget(QWidget *parent) :
    QWidget(parent),
    currentMode(EditMode::PlacePath), // Ĭ���Ƿ���·��ģʽ
    gridSize(20, 15),
    currentMouseGridPos(-1, -1)
{
    setupUI();
    setMouseTracking(true); // ������������ʵʱ����Ԥ��
    // Ϊ�༭������һ�����ʵ�Ĭ�ϴ�С����С��С
    setMinimumSize(800, 600);
    setWindowTitle("�ξ��ػ��� - �ؿ��༭��");
}

void LevelEditorWidget::setupUI() {
    // --- 1. �����ؼ� ---
    loadButton = new QPushButton("���عؿ�");
    saveButton = new QPushButton("����ؿ�");
    statusLabel = new QLabel("��ǰģʽ: ����·��");
    statusLabel->setFixedHeight(30);

    // ����һ����ť�飬ȷ����ѡ��ť�Ļ�����
    QButtonGroup* modeGroup = new QButtonGroup(this);
    pathModeButton = new QRadioButton("����·��");
    towerModeButton = new QRadioButton("��������");
    eraseModeButton = new QRadioButton("��Ƥ��");
    pathModeButton->setChecked(true); // Ĭ��ѡ��

    modeGroup->addButton(pathModeButton);
    modeGroup->addButton(towerModeButton);
    modeGroup->addButton(eraseModeButton);

    // --- 2. �������� ---
    // �Ҳ�Ŀ�����岼��
    QVBoxLayout* controlPanelLayout = new QVBoxLayout();
    controlPanelLayout->addWidget(loadButton);
    controlPanelLayout->addWidget(saveButton);
    controlPanelLayout->addSpacing(20);
    controlPanelLayout->addWidget(new QLabel("�༭ģʽ:"));
    controlPanelLayout->addWidget(pathModeButton);
    controlPanelLayout->addWidget(towerModeButton);
    controlPanelLayout->addWidget(eraseModeButton);
    controlPanelLayout->addStretch(); // ����������ÿؼ�������
    controlPanelLayout->addWidget(statusLabel);


    // ����һ��QWidget��Ϊ������������
    QWidget* controlPanel = new QWidget();
    controlPanel->setLayout(controlPanelLayout);
    controlPanel->setFixedWidth(200); // �̶����������

    // ����������
    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->addStretch(); // ���ĵ�ͼ������������������ռλ
    mainLayout->addWidget(controlPanel); // �Ҳ��ǿ������

    // --- 3. �����źŲ� ---
    connect(loadButton, &QPushButton::clicked, this, &LevelEditorWidget::onLoadButtonPressed);
    connect(saveButton, &QPushButton::clicked, this, &LevelEditorWidget::onSaveButtonPressed);
    // ���Ӱ�ť����źţ����ⰴť��������ᴥ��
    connect(modeGroup, &QButtonGroup::buttonClicked, this, &LevelEditorWidget::onModeChanged);
}


void LevelEditorWidget::onModeChanged() {
    if (pathModeButton->isChecked()) {
        currentMode = EditMode::PlacePath;
        statusLabel->setText("��ǰģʽ: ����·��");
    } else if (towerModeButton->isChecked()) {
        currentMode = EditMode::PlaceTowerBase;
        statusLabel->setText("��ǰģʽ: ��������");
    } else if (eraseModeButton->isChecked()) {
        currentMode = EditMode::Erase;
        statusLabel->setText("��ǰģʽ: ��Ƥ��");
    }
    update(); // �л�ģʽ���ػ棬�Ը���Ԥ����ɫ��
}

void LevelEditorWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // �༭����ľ��η�Χ (�����ܿ�ȼ�ȥ���������)
    QRect drawArea(0, 0, this->width() - 200, this->height());

    float cellWidth = (float)drawArea.width() / gridSize.width();
    float cellHeight = (float)drawArea.height() / gridSize.height();

    // ���Ʊ��� (��ѡ����ǳ��ɫ���)
    painter.fillRect(drawArea, QColor(240, 240, 240));

    // ����������
    painter.setPen(QColor(200, 200, 200));
    for (int i = 0; i <= gridSize.width(); ++i) {
        painter.drawLine(i * cellWidth, 0, i * cellWidth, drawArea.height());
    }
    for (int i = 0; i <= gridSize.height(); ++i) {
        painter.drawLine(0, i * cellHeight, drawArea.width(), i * cellHeight);
    }

    // �����ѷ��õ�·����
    painter.setBrush(QColor(100, 150, 255, 200)); // ��ɫ
    painter.setPen(Qt::NoPen);
    for (const auto& p : path) {
        QRectF cellRect(p.x() * cellWidth, p.y() * cellHeight, cellWidth, cellHeight);
        painter.drawRect(cellRect);
    }

    // �����ѷ��õ�����
    painter.setBrush(QColor(255, 200, 100, 200)); // ��ɫ
    for (const auto& b : towerBases) {
        QRectF cellRect(b.x() * cellWidth, b.y() * cellHeight, cellWidth, cellHeight);
        painter.drawEllipse(cellRect.adjusted(4, 4, -4, -4)); // ����СԲ��������
    }

    // ���������ͣԤ��
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
    if (!drawArea.contains(event->pos())) return; // ����ڿ�������ϣ�����

    float cellWidth = (float)drawArea.width() / gridSize.width();
    float cellHeight = (float)drawArea.height() / gridSize.height();

    int col = event->pos().x() / cellWidth;
    int row = event->pos().y() / cellHeight;
    QPoint gridPos(col, row);

    switch (currentMode) {
        case EditMode::PlacePath:
            // ��ֹ�ظ����
            if (std::find(path.begin(), path.end(), gridPos) == path.end()) {
                path.push_back(gridPos);
            }
            break;
        case EditMode::PlaceTowerBase:
            // ��ֹ�ظ����
            if (std::find(towerBases.begin(), towerBases.end(), gridPos) == towerBases.end()) {
                towerBases.push_back(gridPos);
            }
            break;
        case EditMode::Erase:
            // �Ƴ�·����
            path.erase(std::remove_if(path.begin(), path.end(),
                [&](const QPoint& p){ return p == gridPos; }), path.end());
            // �Ƴ�����
            towerBases.erase(std::remove_if(towerBases.begin(), towerBases.end(),
                [&](const QPoint& b){ return b == gridPos; }), towerBases.end());
            break;
    }
    update(); // �����ػ�
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
            update(); // ֻ�е�����ƶ����µĸ���ʱ���ػ�
        }
    } else {
        if (currentMouseGridPos.x() != -1) {
            currentMouseGridPos = QPoint(-1, -1); // �Ƴ�������
            update();
        }
    }
}


// --- save/load �������ֲ��� ---
void LevelEditorWidget::onSaveButtonPressed() {
    QString filePath = QFileDialog::getSaveFileName(this, "����ؿ�", "", "JSON Files (*.json)");
    if (filePath.isEmpty()) return;
    saveLevelToFile(filePath);
}

void LevelEditorWidget::onLoadButtonPressed() {
    QString filePath = QFileDialog::getOpenFileName(this, "���عؿ�", "", "JSON Files (*.json)");
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
        file.write(doc.toJson(QJsonDocument::Indented)); // ʹ��Indented��ʽ�����׶�
        file.close();
        qInfo() << "�ؿ��ѳɹ�������" << filePath;
    } else {
        qWarning() << "�޷�����ؿ���" << filePath;
    }
}

void LevelEditorWidget::loadLevelFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "�޷��򿪹ؿ��ļ�:" << filePath;
        return;
    }
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (jsonDoc.isNull() || !jsonDoc.isObject()) {
        qWarning() << "����JSONʧ�ܻ��ļ�����һ��JSON����:" << filePath;
        return;
    }
    QJsonObject rootObj = jsonDoc.object();

    // ��յ�ǰ����
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

    qInfo() << "�ؿ�" << filePath << "�ѳɹ����ء�";
    update(); // ǿ���ػ洰������ʾ�¼��صĹؿ�
}

