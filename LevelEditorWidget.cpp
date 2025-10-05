#include "LevelEditorWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QFileDialog>
#include <QPushButton>
#include <QVBoxLayout>

LevelEditorWidget::LevelEditorWidget(QWidget *parent) :
    QWidget(parent) {
    // 1. �����ؼ�
    m_saveButton = new QPushButton("Save Level", this);
    m_loadButton = new QPushButton("Load Level", this);

    // 2. �������ֹ�����
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_saveButton);
    layout->addWidget(m_loadButton);
    layout->addStretch(); // ���һ��������ð�ť�����ڶ���
    setLayout(layout);

    // 3. �����źźͲ�
    connect(m_saveButton, &QPushButton::clicked, this, &LevelEditorWidget::onSaveButtonPressed);
    connect(m_loadButton, &QPushButton::clicked, this, &LevelEditorWidget::onLoadButtonPressed);

    // ���ô��ڱ���
    setWindowTitle("Level Editor");
}

LevelEditorWidget::~LevelEditorWidget() {
    // ���ڿؼ��Ͳ��ֶ������� this ��Ϊ������Qt���Զ��������ǵ��ڴ棬
    // ��������ͨ������Ҫд delete ��䡣
}

void LevelEditorWidget::onSaveButtonPressed() {
    QString fileName = QFileDialog::getSaveFileName(this, "Save Level", "", "JSON Files (*.json);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }
    // ʵ�ֱ����߼�...
}

void LevelEditorWidget::onLoadButtonPressed() {
    QString fileName = QFileDialog::getOpenFileName(this, "Load Level", "", "JSON Files (*.json);;All Files (*)");
    if (fileName.isEmpty()) {
        return;
    }
    // ʵ�ּ����߼�...
}

void LevelEditorWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // ���Ƶ���·��
    painter.setPen(QPen(Qt::green, 3));
    if (m_enemyPath.size() > 1) {
        for (size_t i = 0; i < m_enemyPath.size() - 1; ++i) {
            painter.drawLine(m_enemyPath[i], m_enemyPath[i + 1]);
        }
    }

    // ����·����
    painter.setBrush(Qt::darkGreen);
    for (const auto &point: m_enemyPath) {
        painter.drawEllipse(point, 5, 5);
    }
}

void LevelEditorWidget::mousePressEvent(QMouseEvent *event) {
    // ʾ����������·����
    // ����Ҫȷ��������ڰ�ť��
    if (!childAt(event->pos())) {
         if (event->button() == Qt::LeftButton) {
            m_enemyPath.push_back(event->pos());
            update(); // �����ػ�
        }
    }
    QWidget::mousePressEvent(event);
}

