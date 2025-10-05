#include "GameManager.h"
#include <QGraphicsScene>

// ��ʼ����̬ʵ��ָ��
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
    // ������
    m_gameTimer->stop();
}

void GameManager::initialize(QGraphicsScene *scene) {
    m_scene = scene;
    // ������ҳ�ʼ״̬
    m_player->setInitialState(100, 500);
    // ������Ϸѭ��������ÿ16�������һ�Σ�Լ60FPS��
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

    // ��Ϸ��ѭ���߼�
    // 1. ����������ϷԪ�أ��������ˡ��ӵ���
    // 2. �����ײ
    // 3. ������˲���
    // 4. ����UI

    // ʾ�����ó����е����пɶ�����ǰ��
    // QGraphicsScene::advance() ����ó���������item��advance()����
    m_scene->advance();
}
