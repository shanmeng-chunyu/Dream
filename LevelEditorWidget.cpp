#include "LevelEditorWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>

LevelEditorWidget::LevelEditorWidget(QWidget *parent) :
    QWidget(parent) {
    // 1. 创建控件
    m_saveButton = new QPushButton("Save Level", this);
    m_loadButton = new QPushButton("Load Level", this);

    // 2. 创建布局管理器
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_saveButton);
    layout->addWidget(m_loadButton);
    layout->addStretch(); // 添加一个伸缩项，让按钮保持在顶部
    setLayout(layout);

    // 3. 连接信号和槽
    connect(m_saveButton, &QPushButton::clicked, this, &LevelEditorWidget::onSaveButtonPressed);
    connect(m_loadButton, &QPushButton::clicked, this, &LevelEditorWidget::onLoadButtonPressed);

    // 设置窗口标题
    setWindowTitle("Level Editor");
}

LevelEditorWidget::~LevelEditorWidget() {
    // 由于控件和布局都设置了 this 作为父对象，Qt会自动管理它们的内存，
    // 所以这里通常不需要写 delete 语句。
}

void LevelEditorWidget::onSaveButtonPressed() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Level", "", "JSON Files (*.json);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }
    // 实现保存逻辑...
}

void LevelEditorWidget::onLoadButtonPressed() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Level", "", "JSON Files (*.json);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }
    // 实现加载逻辑...
}

void LevelEditorWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制敌人路径
    painter.setPen(QPen(Qt::green, 3));
    if (m_enemyPath.size() > 1) {
        for (size_t i = 0; i < m_enemyPath.size() - 1; ++i) {
            painter.drawLine(m_enemyPath[i], m_enemyPath[i + 1]);
        }
    }

    // 绘制路径点
    painter.setBrush(Qt::darkGreen);
    for (const auto &point: m_enemyPath) {
        painter.drawEllipse(point, 5, 5);
    }
}

void LevelEditorWidget::mousePressEvent(QMouseEvent *event) {
    // 示例：点击添加路径点
    // 我们要确保点击不在按钮上
    if (!childAt(event->pos())) {
         if (event->button() == Qt::LeftButton) {
            m_enemyPath.push_back(event->pos());
            update(); // 请求重绘
        }
    }
    QWidget::mousePressEvent(event);
}

