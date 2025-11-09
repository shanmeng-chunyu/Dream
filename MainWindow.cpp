#include "MainWindow.h"
#include "GameManager.h"
#include "widget_menu.h"
#include "widget_choose_level.h"
#include "widget_level_loading.h"
#include "widget_post_game.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    // 设置窗口大小
    setFixedSize(800, 600);
    setWindowTitle("Dream Guardian");

    // 1. 创建场景
    m_scene = new QGraphicsScene(this);
    m_scene->setSceneRect(0, 0, 800, 600);

    // 2. 创建视图
    m_view = new QGraphicsView(m_scene, this);
    m_view->setFixedSize(800, 600);
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 3. 将视图设置中心窗口部件
    setCentralWidget(m_view);

    // 4. 初始化并启动游戏管理器
    GameManager::instance()->init(m_scene);

}

MainWindow::~MainWindow() {
    // C++的父子对象机制会自动处理scene和view的内存释放
}
