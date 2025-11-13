#include "GameManager.h"

#include <FishingCatPillow.h>
#include <InspirationBulb.h>
#include <KnowledgeTree.h>

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

#include "FriendCompanion.h"
#include "LiveCoffee.h"
#include "NightRadio.h"
#include "PettingCatTime.h"
#include "WarmMemory.h"
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
    // QFile file(levelPath);
    // if (!file.open(QIODevice::ReadOnly)) return;
    //
    // QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    // QJsonObject rootObj = doc.object();
    // file.close();
    //
    // // 使用LevelLoader加载核心数据
    // LevelLoader::loadLevel(levelPath, *m_gameMap, *m_waveManager, *m_player);
    if (!m_gameMap->loadFromFile(levelPath)) {
        qWarning() << "GameManager failed to load map:" << levelPath;
        return;
    }
    QFile file(levelPath);
    if (!file.open(QIODevice::ReadOnly)) return;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject rootObj = doc.object();
    file.close();

    QJsonObject playerObj = rootObj["player"].toObject();
    m_player->setInitialState(playerObj["initial_stability"].toInt(), playerObj["initial_resource"].toInt());

    QJsonArray wavesArray = rootObj["waves"].toArray();
    m_waveManager->loadWaves(wavesArray);

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

        obstacle->setPos(absCenterPos);
        obstacle->setOffset(-obstaclePixelSize.width() / 2.0,
                    -obstaclePixelSize.height() / 2.0);

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

    m_waveManager->update();

    /* ========= 敌人更新（移动 + 狂暴 + 回血） ========= */
    for (Enemy* enemy : m_enemies) {
        enemy->move();

        /* ----------- Boss 狂暴（第一关） ----------- */
        if (enemy->getType() == "thesis" && !m_raged.contains(enemy)) {
            const QJsonObject proto = m_enemyPrototypes["thesis"];
            const double thr = proto.value("rageThreshold").toDouble(0.4);
            const int maxHp = proto.value("health").toInt();

            if (maxHp > 0 && (double)enemy->getHealth() / maxHp < thr) {
                const double baseSpd = proto.value("speed").toDouble();
                const double mul = proto.value("rageSpeedMul").toDouble(1.35);
                enemy->setSpeed(baseSpd * mul);
                m_raged.insert(enemy);
            }
        }

        /* ----------- 泪水怪回血（第二关） ----------- */
        if (enemy->getType() == "tears") {
            const QJsonObject proto = m_enemyPrototypes["tears"];
            const int maxHp = proto.value("health").toInt();
            if (maxHp > 0) {
                const double thr      = proto.value("healBelowHp").toDouble(0.5);
                const int heal        = proto.value("healPerTick").toInt(10);
                const double radius   = proto.value("healRadius").toDouble(200.0);
                const double interval = proto.value("healInterval").toDouble(2.0);

                if ((double)enemy->getHealth() / maxHp < thr) {
                    int& cd = m_healCd[enemy];
                    if (cd <= 0) {
                        QPointF c = enemy->pos();
                        for (Enemy* ally : m_enemies) {
                            if (ally == enemy) continue;
                            if (QLineF(c, ally->pos()).length() <= radius)
                                ally->heal(heal);
                        }
                        cd = int(interval * 60.0); // 默认以 60fps 当帧率
                    } else {
                        --cd;
                    }
                }
            }
        }
    }

    /* ========= 子弹移动 ========= */
    for (Bullet* bullet : m_bullets) bullet->move();

    updateTowerTargets();
    for (Tower* tower : m_towers) {
        tower->findAndAttackTarget();
    }

    cleanupEntities();
    checkWinLossConditions();
}


void GameManager::onSpawnEnemy(const QString& type, const std::vector<QPointF>& absolutePath) {
    if (!m_enemyPrototypes.contains(type)) return;

    const QSize enemyPixelSize(126, 126);
    QJsonObject proto = m_enemyPrototypes[type];
    QPixmap pixmap(proto["pixmap"].toString());
    QPixmap scaledPixmap = pixmap.scaled(enemyPixelSize,Qt::KeepAspectRatio);

    auto* enemy = new Enemy(
        proto["health"].toInt(),
        proto["speed"].toDouble(),
        proto["damage"].toInt(),
        absolutePath,
        proto["type"].toString(),
        scaledPixmap
    );

    enemy->setOffset(-enemyPixelSize.width() / 2.0, -enemyPixelSize.height()*0.8);
    if (!absolutePath.empty()) {
        enemy->setPos(absolutePath[0]);
    }

    m_scene->addItem(enemy);
    m_enemies.append(enemy);

    connect(enemy, &Enemy::reachedEnd, this, &GameManager::onEnemyReachedEnd);
    connect(enemy, &Enemy::died, this, &GameManager::onEnemyDied);

    //if (type == "nightmare") { destroyAllTowers(true); }

}

void GameManager::buildTower(const QString& type, const QPointF& relativePosition) {
    if (!m_towerPrototypes.contains(type)) return;

    QJsonObject proto = m_towerPrototypes[type];
    int cost = proto["cost"].toInt();
    QString pixmap_path = proto["pixmap"].toString();

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
        tower = new KnowledgeTree(pixelRange);
    }else if (type == "FishingCatPillow") {
        tower = new FishingCatPillow(pixelRange);
        FishingCatPillow* pillow = qobject_cast<FishingCatPillow*>(tower);
        connect(pillow,&FishingCatPillow::applyControl,this,&GameManager::onApplyEnemyControl);
    }else if (type == "LiveCoffee") {
        tower = new LiveCoffee(pixelRange);
    }else if (type == "WarmMemories") {
        tower = new WarmMemory(pixelRange);
    }else if (type == "NightRadio") {
        tower = new NightRadio(pixelRange);
    }else if (type == "PettingCatTime") {
        tower = new PettingCatTime(pixelRange);
    }else if (type == "Companionship") {
        tower = new FriendCompanion(pixelRange);
    }

    const QSize towerPixelSize(76, 76);

    QPixmap originalPixmap = QPixmap(pixmap_path); // 获取构造函数设置的pixmap
    // 缩放 pixmap
    QPixmap scaledPixmap = originalPixmap.scaled(towerPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    // 替换掉原来的 pixmap
    tower->setPixmap(scaledPixmap);
    // 设置偏移量，将 (0,0) 点移动到中心
    const QPointF towerTopLeftPos(absPos.x() - towerPixelSize.width() / 2.0,
                                  absPos.y() - towerPixelSize.height() / 2.0);
    tower->setPos(towerTopLeftPos);
    m_scene->addItem(tower);
    m_towers.append(tower);
    connect(tower,&Tower::newBullet,this,&GameManager::onNewBullet);
}


void GameManager::onNewBullet(Tower* tower, QGraphicsPixmapItem* target) {
    // 根据发射塔的类型查找对应的子弹贴图
    // 1. 定义子弹的固定像素大小 (按照你的要求)
    const QSize bulletPixelSize(76, 76);

    // 2. 获取子弹贴图原型
    QString type = tower->getType();
    QJsonObject proto = m_towerPrototypes[type];
    QPixmap originalPixmap(proto["bullet_pixmap"].toString());

    // 3. 缩放贴图
    QPixmap scaledPixmap = originalPixmap.scaled(bulletPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // (健壮性检查：如果贴图路径错误，创建一个红色的方块)
    if (scaledPixmap.isNull()) {
        scaledPixmap = QPixmap(bulletPixelSize);
        scaledPixmap.fill(Qt::red);
    }

    // 4. 创建子弹实例 (使用缩放后的贴图)
    // (速度 10.0 仍然是硬编码，你可以后续从 proto 中读取)
    auto* bullet = new Bullet(tower->getDamage(), 10.0, target, scaledPixmap);

    // 5. 计算生成位置（关键：中心对齐）

    // 5a. 获取塔的 top-left 位置 (这是 tower->pos() 的含义)
    QPointF towerTopLeft = tower->pos();

    // 5b. 获取塔的尺寸 (必须与 buildTower 中定义的76x76一致)
    const QSize towerPixelSize(76, 76);

    // 5c. 计算塔的中心点 (即子弹的生成点)
    QPointF spawnCenterPos(towerTopLeft.x() + towerPixelSize.width() / 2.0,
                           towerTopLeft.y() + towerPixelSize.height() / 2.0);

    // 5d. 根据子弹的中心点和40x40的尺寸，计算子弹的 top-left 位置
    QPointF bulletTopLeftPos(spawnCenterPos.x() - bulletPixelSize.width() / 2.0,
                             spawnCenterPos.y() - bulletPixelSize.height() / 2.0);

    bullet->setPos(bulletTopLeftPos);

    m_scene->addItem(bullet);
    m_bullets.append(bullet);
    connect(bullet, &Bullet::hitTarget, this, &GameManager::onBulletHitTarget);
}

void GameManager::onEnemyReachedEnd(Enemy* enemy) {
    m_player->decreaseStability(enemy->getDamage());
    for (Tower* tower : m_towers) {
        if (tower->getCurrentTarget() == enemy) {
            tower->setTarget(nullptr);
        }
    }
    m_entitiesToClean.append(enemy);
    m_enemies.removeAll(enemy);
}

void GameManager::onEnemyDied(Enemy* enemy) {
    // 可根据敌人类型给予不同资源
    QJsonObject proto = m_enemyPrototypes[enemy->getType()];
    m_player->addResource(proto["drops"].toInt());
    for (Tower* tower : m_towers) {
        if (tower->getCurrentTarget() == enemy) {
            tower->setTarget(nullptr);
        }
    }
    m_entitiesToClean.append(enemy);
    m_enemies.removeAll(enemy);

}

void GameManager::onBulletHitTarget(Bullet* bullet) {
    QGraphicsPixmapItem* target = bullet->getTarget();
    //确定子弹目标
    if (target) {
        Enemy *enemyTarget = dynamic_cast<Enemy*>(target);
        if (enemyTarget && m_enemies.contains(enemyTarget)) {
            enemyTarget->takeDamage(bullet->getDamage());
        }else {
            Obstacle* obstacleTarget = dynamic_cast<Obstacle*>(target);
            if (obstacleTarget && m_obstacles.contains(obstacleTarget)) {
                obstacleTarget->takeDamage(bullet->getDamage());
            }
        }
    }
    m_entitiesToClean.append(bullet);
    m_bullets.removeAll(bullet);
}

void GameManager::onObstacleDestroyed(Obstacle* obstacle, int resourceValue) {
    m_player->addResource(resourceValue);

    //检查是否有塔的目标为该障碍物
    for (Tower* tower : m_towers) {
        if (tower->getCurrentTarget() == obstacle) {
            tower->setTarget(nullptr);
        }
    }
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

        // 1. 检查当前目标是否仍然有效且在范围内
        if (tower->currentTarget && tower->targetIsInRange()) {
            // 检查目标是否还“存活”
            Enemy* enemyTarget = dynamic_cast<Enemy*>(tower->currentTarget);
            if (enemyTarget && m_enemies.contains(enemyTarget)) {
                continue; // 目标是敌人，有效，继续攻击
            }
            Obstacle* obstacleTarget = dynamic_cast<Obstacle*>(tower->currentTarget);
            if (obstacleTarget && m_obstacles.contains(obstacleTarget)) {
                continue; // 目标是障碍物，有效，继续攻击
            }
        }

        // 2. 目标无效或出范围，寻找新目标
        //    【优先级 1: 寻找最近的敌人】
        Enemy* closestEnemy = nullptr;
        double minEnemyDistance = tower->range + 1.0;

        for (Enemy* enemy : m_enemies) {
            double distance = QLineF(tower->pos(), enemy->pos()).length();
            if (distance < minEnemyDistance) {
                minEnemyDistance = distance;
                closestEnemy = enemy;
            }
        }

        if (closestEnemy) {
            tower->setTarget(closestEnemy);
            continue; // 找到敌人，此塔更新完毕
        }

        // 3. 【优先级 2: 寻找最近的障碍物】
        //    (只有在没有敌人在范围内时，才会执行到这里)
        Obstacle* closestObstacle = nullptr;
        double minObstacleDistance = tower->range + 1.0;

        for (Obstacle* obstacle : m_obstacles) {
            double distance = QLineF(tower->pos(), obstacle->pos()).length();
            if (distance < minObstacleDistance) {
                minObstacleDistance = distance;
                closestObstacle = obstacle;
            }
        }

        if (closestObstacle) {
            tower->setTarget(closestObstacle);
        } else {
            // 4. 【优先级 3: 没有目标】
            tower->setTarget(nullptr);
        }
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

void GameManager::onApplyEnemyControl(QGraphicsPixmapItem* enemy,double duration) {
    Enemy* enemyTarget = static_cast<Enemy*>(enemy);
    if (!enemyTarget || !m_enemies.contains(enemy)) {
        return;
    }

    enemyTarget->stopFor(duration);
}
Enemy* GameManager::spawnByTypeWithPath(const QString& type,
                                        const std::vector<QPointF>& absPath,
                                        double scale)
{
    if (!m_enemyPrototypes.contains(type)) return nullptr;
    const QJsonObject proto = m_enemyPrototypes[type];

    const int hp = proto.value("health").toInt();
    const double spd = proto.value("speed").toDouble();
    const int dmg = proto.value("damage").toInt();
    const QString pix = proto.value("pixmap").toString();

    QPixmap pm(pix);
    Enemy* e = new Enemy(hp, spd, dmg, absPath, type, pm);
    if (scale != 1.0) e->setScale(scale);

    m_scene->addItem(e);
    m_enemies.append(e);
    connect(e, &Enemy::reachedEnd, this, &GameManager::onEnemyReachedEnd);
    connect(e, &Enemy::died,       this, &GameManager::onEnemyDied);
    return e;
}
