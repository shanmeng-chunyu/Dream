#include "GameManager.h"

#include <InspirationBulb.h>

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

#include "widget_post_game.h"

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
    loadPrototypes();

    // 根据地图数据创建障碍物
    for (const auto& obsData : m_gameMap->getObstacles()) {
        QPixmap pixmap(obsData.pixmapPath);

        // 1. 定义障碍物的固定像素大小
        const QSize obstaclePixelSize(152, 152);
        QPixmap scaledPixmap = pixmap.scaled(obstaclePixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        auto* obstacle = new Obstacle(obsData.health, obsData.resourceValue, scaledPixmap);

        // 2. 将json中的坐标视为“中心点”
        QPointF absCenterPos(obsData.relativePosition.x() * m_screenSize.width(),
                             obsData.relativePosition.y() * m_screenSize.height());

        // 3. (统一逻辑) 根据中心点计算左上角位置
        QPointF absTopLeftPos(absCenterPos.x() - obstaclePixelSize.width() / 2.0,
                              absCenterPos.y() - obstaclePixelSize.height() / 2.0);

        // 4. 使用计算出的左上角位置
        obstacle->setPos(absTopLeftPos);

        m_scene->addItem(obstacle);
        m_obstacles.append(obstacle);
        connect(obstacle, &Obstacle::destroyed, this, &GameManager::onObstacleDestroyed);
    }
}

void GameManager::loadPrototypes() {
    m_enemyPrototypes.clear();
    m_towerPrototypes.clear();

    // --- 加载敌人主数据 ---
    // (注意：这些路径是 .qrc 文件中定义的路径)
    QFile enemyFile(":/data/enemy_data.json");
    if (enemyFile.open(QIODevice::ReadOnly)) {
        QJsonDocument enemyDoc = QJsonDocument::fromJson(enemyFile.readAll());
        // (根据 enemy_data.json 的结构)
        QJsonArray enemyArray = enemyDoc.object()["master_enemies"].toArray();
        for (const QJsonValue& val : enemyArray) {
            QJsonObject obj = val.toObject();
            m_enemyPrototypes[obj["type"].toString()] = obj;
        }
        enemyFile.close();
    }

    // --- 加载防御塔主数据 ---
    QFile towerFile(":/data/tower_data.json");
    if (towerFile.open(QIODevice::ReadOnly)) {
        QJsonDocument towerDoc = QJsonDocument::fromJson(towerFile.readAll());
        // (根据 tower_data.json 的结构)
        QJsonArray towerArray = towerDoc.object()["master_towers"].toArray();
        for (const QJsonValue& val : towerArray) {
            QJsonObject obj = val.toObject();
            m_towerPrototypes[obj["type"].toString()] = obj;
        }
        towerFile.close();
    }
}

void GameManager::startGame() {
    m_gameIsOver = false;
    m_gameTimer->start(16); // ~60 FPS
    m_waveManager->startNextWave();
}

void GameManager::setScreenSize(const QSizeF& size) {
    //1.获取旧尺寸，并防止无效计算
    QSizeF oldSize = m_screenSize;
    if (size == oldSize || oldSize.isEmpty() || oldSize.width() == 0 || oldSize.height() == 0) {
        m_screenSize = size;
        m_waveManager->setScreenSize(size);
        return;
    }

    // 2. 更新管理器中的尺寸
    m_screenSize = size;
    m_waveManager->setScreenSize(size);

    // 3. 计算 X 和 Y 方向的缩放因子
    qreal scaleX = size.width() / oldSize.width();
    qreal scaleY = size.height() / oldSize.height();

    // 4. 定义一个辅助函数，用于缩放 QGraphicsItem 的位置
    auto rescaleItemPos = [=](QGraphicsItem* item) {
        if (item) {
            item->setPos(item->pos().x() * scaleX, item->pos().y() * scaleY);
        }
    };

    // 5. 遍历和更新所有实体

    // 更新敌人
    for (Enemy* enemy : m_enemies) {
        rescaleItemPos(enemy);

        // 关键：必须同时更新敌人未走完的路径点
        std::vector<QPointF> newPath;
        const auto& oldPath = enemy->getAbsolutePath(); // (我们将在 Enemy.h 中添加这个函数)
        newPath.reserve(oldPath.size());

        for(const QPointF& pt : oldPath) {
            newPath.emplace_back(pt.x() * scaleX, pt.y() * scaleY);
        }
        // setAbsolutePath 已经存在
        enemy->setAbsolutePath(newPath);
    }

    // 更新防御塔
    for (Tower* tower : m_towers) {
        rescaleItemPos(tower);

        // 关键：必须同时更新塔的攻击范围
        // getRange() 已经存在
        double oldRange = tower->getRange();
        // 我们的 buildTower 是按宽度缩放范围的，所以这里也按 X 缩放
        double newRange = oldRange * scaleX;
        tower->setRange(newRange); // (我们将在 Tower.h 中添加这个函数)
    }

    // 更新子弹
    for (Bullet* bullet : m_bullets) {
        rescaleItemPos(bullet);
    }

    // 更新障碍物
    for (Obstacle* obstacle : m_obstacles) {
        rescaleItemPos(obstacle);
    }
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
    int cost = proto["cost"].toInt();

    // 1. 检查资源
    if (!m_player->spendResource(cost)) {
        return;
    }

    // 2. 计算塔的绝对像素位置
    QPointF absPos(relativePosition.x() * m_screenSize.width(),
                    relativePosition.y() * m_screenSize.height());

    double relativeRange = proto["range"].toDouble();
    double gridRelativeWidth = m_gameMap->getPathWidthRatio();
    double gridPixelWidth = gridRelativeWidth * m_screenSize.width();
    double pixelRange = relativeRange * gridPixelWidth;

    Tower* tower = nullptr;

    if (type == "InspirationBulb") {
        tower = new InspirationBulb(pixelRange);
    }else if (type == "KnowledgeTree") {

    }

    tower->setPos(absPos);
    m_scene->addItem(tower);
    m_towers.append(tower);
    connect(tower,&Tower::newBullet,this,&GameManager::onNewBullet);
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
    QJsonObject proto = m_enemyPrototypes[enemy->getType()];
    m_player->addResource(proto["drops"].toInt());
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
        emit gameFinished(false,m_player->getStability(), m_waveManager->getTotalEnemiesKilled());
        // 此处可以发射一个游戏失败的信号
    }

    if (m_waveManager->isFinished() && m_enemies.isEmpty()) {
        m_gameIsOver = true;
        m_gameTimer->stop();
        emit gameFinished(true,m_player->getStability(), m_waveManager->getTotalEnemiesKilled());
        // 此处可以发射一个游戏胜利的信号
    }
}

void GameManager::onTowerUpgradeRequested(const QPointF& relativePosition) {
    // 查找对应位置的塔并升级
    for (Tower* tower : m_towers) {
        QPointF towerRelPos(tower->pos().x() / m_screenSize.width(),
                            tower->pos().y() / m_screenSize.height());
        if (qFuzzyCompare(towerRelPos, relativePosition)) {
            QJsonObject proto = m_towerPrototypes[tower->getType()];
            if (m_player->spendResource(proto["upgrade_cost"].toInt())) {
                tower->upgrade();
            }
            break;
        }
    }
}

void GameManager::onTowerSellRequested(const QPointF& relativePosition) {
    // 查找对应位置的塔并出售
    for (int i = 0; i < m_towers.size(); ++i) {
        Tower* tower = m_towers[i];
        QPointF towerRelPos(tower->pos().x() / m_screenSize.width(),
                            tower->pos().y() / m_screenSize.height());
        if (qFuzzyCompare(towerRelPos, relativePosition)) {
            m_entitiesToClean.append(tower);
            m_towers.removeAt(i);
            break;
        }
    }
}

void GameManager::pauseGame() {
    m_gameTimer->stop();
}

void GameManager::resumeGame() {
    if (!m_gameIsOver) {
        m_gameTimer->start(16);
    }
}
