#include "GameManager.h"
#include "LevelLoader.h"
#include "Player.h"
#include "WaveManager.h"
#include "GameMap.h"
#include "Enemy.h"
#include "Tower.h"
#include "Bullet.h"
#include "Obstacle.h"

#include <QGraphicsScene>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QLineF>
#include <QtMath>
#include <QMessageBox>

GameManager* GameManager::m_instance = nullptr;

GameManager* GameManager::instance() {
    if (!m_instance) {
        m_instance = new GameManager();
    }
    return m_instance;
}

GameManager::GameManager(QObject* parent)
    : QObject(parent),
      m_scene(nullptr),
      m_gameTimer(new QTimer(this)),
      m_screenSize(800, 600),
      m_player(new Player(this)),
      m_gameMap(new GameMap()),
      m_waveManager(new WaveManager(m_gameMap, this)),
      m_gameIsOver(false)
{
    connect(m_gameTimer, &QTimer::timeout, this, &GameManager::updateGame);
    connect(m_waveManager, &WaveManager::spawnEnemy, this, &GameManager::onSpawnEnemy);
}

GameManager::~GameManager() {
    delete m_gameMap;
    // m_player和m_waveManager因为设置了parent为this，会被Qt自动管理销毁
}

void GameManager::init(QGraphicsScene* scene) {
    m_scene = scene;
}

void GameManager::loadLevel(const QString& levelPath) {
    QFile file(levelPath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject rootObj = doc.object();
    file.close();

    // 使用LevelLoader加载核心数据
    LevelLoader::loadLevel(levelPath, *m_gameMap, *m_waveManager, *m_player);
    loadPrototypes(rootObj);

    // 根据地图数据创建障碍物
    for (const auto& obsData : m_gameMap->getObstacles()) {
        QPixmap pixmap(obsData.pixmapPath);
        auto* obstacle = new Obstacle(obsData.health, obsData.resourceValue, pixmap);

        QPointF absPos(obsData.relativePosition.x() * m_screenSize.width(),
                       obsData.relativePosition.y() * m_screenSize.height());
        obstacle->setPos(absPos);

        m_scene->addItem(obstacle);
        m_obstacles.append(obstacle);
        connect(obstacle, &Obstacle::destroyed, this, &GameManager::onObstacleDestroyed);
    }
}

void GameManager::loadPrototypes(const QJsonObject& rootObj) {
    m_enemyPrototypes.clear();
    QJsonArray enemyArray = rootObj["available_enemies"].toArray();
    for (const QJsonValue& val : enemyArray) {
        QJsonObject obj = val.toObject();
        m_enemyPrototypes[obj["type"].toString()] = obj;
    }

    m_towerPrototypes.clear();
    QJsonArray towerArray = rootObj["available_towers"].toArray();
    for (const QJsonValue& val : towerArray) {
        QJsonObject obj = val.toObject();
        m_towerPrototypes[obj["type"].toString()] = obj;
    }
}

void GameManager::startGame() {
    m_gameIsOver = false;
    m_gameTimer->start(16); // ~60 FPS
    m_waveManager->startNextWave();
}

void GameManager::setScreenSize(const QSizeF& size) {
    m_screenSize = size;
    m_waveManager->setScreenSize(size);
    // 可在此处添加代码，重新计算所有现有实体的位置以适应新窗口大小
}

void GameManager::updateGame() {
    if (m_gameIsOver) return;

    // 移动所有实体
    for (Enemy* enemy : m_enemies) enemy->move();
    for (Bullet* bullet : m_bullets) bullet->move();

    // 更新防御塔目标
    updateTowerTargets();

    // 清理上一帧标记为删除的实体
    cleanupEntities();

    // 检查游戏结束条件
    checkWinLossConditions();
}

void GameManager::onSpawnEnemy(const QString& type, const std::vector<QPointF>& absolutePath) {
    if (!m_enemyPrototypes.contains(type)) return;

    QJsonObject proto = m_enemyPrototypes[type];
    QPixmap pixmap(proto["pixmap"].toString());

    auto* enemy = new Enemy(
        proto["health"].toInt(),
        proto["speed"].toDouble(),
        proto["damage"].toInt(),
        absolutePath,
        pixmap
    );

    m_scene->addItem(enemy);
    m_enemies.append(enemy);

    connect(enemy, &Enemy::reachedEnd, this, &GameManager::onEnemyReachedEnd);
    connect(enemy, &Enemy::died, this, &GameManager::onEnemyDied);
}

void GameManager::buildTower(const QString& type, const QPointF& relativePosition) {
    if (!m_towerPrototypes.contains(type)) return;

    QJsonObject proto = m_towerPrototypes[type];
    if (m_player->spendResource(proto["cost"].toInt())) {
        QPixmap pixmap(proto["pixmap"].toString());
        auto* tower = new Tower(
            proto["damage"].toInt(),
            proto["range"].toDouble() * m_screenSize.width(), // 范围也要转换为绝对值
            proto["fire_rate"].toInt(),
            proto["cost"].toInt(),
            proto["upgradeCost"].toInt(),
            pixmap
        );

        QPointF absPos(relativePosition.x() * m_screenSize.width(),
                       relativePosition.y() * m_screenSize.height());
        tower->setPos(absPos);

        m_scene->addItem(tower);
        m_towers.append(tower);
        connect(tower, &Tower::newBullet, this, &GameManager::onNewBullet);
    }
}


void GameManager::onNewBullet(Tower* tower, Enemy* target) {
    // 根据发射塔的类型查找对应的子弹贴图
    // 这里简化处理，假设所有塔都用同一种子弹或在tower prototype里定义
    QPixmap pixmap(":/bullets/default_bullet.png"); // 应从JSON读取

    auto* bullet = new Bullet(tower->damage, 10.0, target, pixmap); // 速度硬编码，可改为从JSON读取
    bullet->setPos(tower->pos());

    m_scene->addItem(bullet);
    m_bullets.append(bullet);
    connect(bullet, &Bullet::hitTarget, this, &GameManager::onBulletHitTarget);
}

void GameManager::onEnemyReachedEnd(Enemy* enemy) {
    m_player->decreaseStability(enemy->getDamage());
    m_entitiesToClean.append(enemy);
    m_enemies.removeAll(enemy);
}

void GameManager::onEnemyDied(Enemy* enemy) {
    // 可根据敌人类型给予不同资源
    m_player->addResource(10);
    m_entitiesToClean.append(enemy);
    m_enemies.removeAll(enemy);
}

void GameManager::onBulletHitTarget(Bullet* bullet) {
    Enemy* target = bullet->getTarget();
    if (target && m_enemies.contains(target)) { // 确保目标仍然有效
        target->takeDamage(bullet->getDamage());
    }
    m_entitiesToClean.append(bullet);
    m_bullets.removeAll(bullet);
}

void GameManager::onObstacleDestroyed(Obstacle* obstacle, int resourceValue) {
    m_player->addResource(resourceValue);
    m_entitiesToClean.append(obstacle);
    m_obstacles.removeAll(obstacle);
}

void GameManager::cleanupEntities() {
    if (m_entitiesToClean.isEmpty()) return;

    for (QGraphicsItem* item : m_entitiesToClean) {
        m_scene->removeItem(item);
        delete item;
    }
    m_entitiesToClean.clear();
}

void GameManager::updateTowerTargets() {
    for (Tower* tower : m_towers) {
        if (tower->currentTarget && tower->targetIsInRange()) {
            continue; // 当前目标有效，无需更换
        }

        Enemy* closestEnemy = nullptr;
        double minDistance = tower->range + 1.0;

        for (Enemy* enemy : m_enemies) {
            double distance = QLineF(tower->pos(), enemy->pos()).length();
            if (distance < minDistance) {
                minDistance = distance;
                closestEnemy = enemy;
            }
        }
        tower->setTarget(closestEnemy);
    }
}

void GameManager::checkWinLossConditions() {
    if (m_player->getStability() <= 0) {
        m_gameIsOver = true;
        m_gameTimer->stop();
        QMessageBox::information(nullptr, "Game Over", "You Lost!");
        // 此处可以发射一个游戏失败的信号
    }

    if (m_waveManager->isFinished() && m_enemies.isEmpty()) {
        m_gameIsOver = true;
        m_gameTimer->stop();
        QMessageBox::information(nullptr, "Congratulations", "You Won!");
        // 此处可以发射一个游戏胜利的信号
    }
}
