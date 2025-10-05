#include "MainWindow.h"
#include "GameManager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    // ���ô��ڴ�С
    setFixedSize(800, 600);
    setWindowTitle("Dream Guardian");

    // 1. ��������
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 800, 600);

    // 2. ������ͼ
    m_view = new QGraphicsView(m_scene, this);
    m_view->setFixedSize(800, 600);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 3. ����ͼ�������Ĵ��ڲ���
    setCentralWidget(m_view);

    // 4. ��ʼ����������Ϸ������
    GameManager::instance()->initialize(m_scene);
}

MainWindow::~MainWindow() {
    // C++�ĸ��Ӷ�����ƻ��Զ�����scene��view���ڴ��ͷ�
}
