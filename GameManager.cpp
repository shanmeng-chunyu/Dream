#include "GameManager.h"
#include <QGraphicsScene>

// 初始化静态实例指针
GameManager *GameManager::m_instance = nullptr;

GameManager *GameManager::instance() {
    if (!m_instance) {
        m_instance = new GameManager();
    }
    return m_instance;
}

GameManager::GameManager(QObject *parent)
    : QObject(parent), m_scene(nullptr), m_waveManager(new WaveManager(this)), m_player(new Player(this)) {
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &GameManager::gameLoop);
}

GameManager::~GameManager() {
    // 清理工作
    m_gameTimer->stop();
}

void GameManager::initialize(QGraphicsScene *scene) {
    m_scene = scene;
    // 设置玩家初始状态
    m_player->setInitialState(100, 500);
    // 启动游戏循环，例如每16毫秒更新一次（约60FPS）
    m_gameTimer->start(16);
}

void GameManager::addTower(Tower *tower) {
    if (m_scene && tower) {
        // m_towers.append(tower);
        m_scene->addItem(tower);
    }
}

void GameManager::removeTower(Tower *tower) {
    if (tower) {
        // m_towers.removeOne(tower);
        delete tower;
    }
}

void GameManager::addEnemy(Enemy *enemy) {
    if (m_scene && enemy) {
        // m_enemies.append(enemy);
        m_scene->addItem(enemy);
    }
}

void GameManager::removeEnemy(Enemy *enemy) {
    if (enemy) {
        // m_enemies.removeOne(enemy);
        delete enemy;
    }
}

void GameManager::addBullet(QGraphicsItem *bullet) {
    if (m_scene && bullet) {
        m_scene->addItem(bullet);
    }
}

void GameManager::removeBullet(QGraphicsItem *bullet) {
    if (bullet) {
        delete bullet;
    }
}

Player *GameManager::getPlayer() const {
    return m_player;
}

void GameManager::gameLoop() {
    if (!m_scene) return;

    // 游戏主循环逻辑
    // 1. 更新所有游戏元素（塔、敌人、子弹）
    // 2. 检测碰撞
    // 3. 处理敌人波次
    // 4. 更新UI

    // 示例：让场景中的所有可动对象前进
    // QGraphicsScene::advance() 会调用场景中所有item的advance()方法
    m_scene->advance();
}
