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
#include <QHash>

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
                enemy->setBaseSpeed(baseSpd * mul);
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

    Enemy* enemy = spawnByTypeWithPath(type, absolutePath, 1.0);

    if (enemy && !absolutePath.empty()) {
        enemy->setPos(absolutePath[0]);
    }

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
    if (!m_enemies.contains(enemy)) {
        return;
    }
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
    if (!m_enemies.contains(enemy)) {
        return;
    }
    QString type = enemy->getType();
    QJsonObject proto = m_enemyPrototypes[type];

    // 1. 增加资源
    m_player->addResource(proto["drops"].toInt());

    // 2. 【修复】通知 WaveManager 敌人被击杀，用于统计总击杀数
    m_waveManager->onEnemykilled();

    // 3. 清理所有指向该敌人的塔
    for (Tower* tower : m_towers) {
        if (tower->getCurrentTarget() == enemy) {
            tower->setTarget(nullptr);
        }
    }

    // 4. 【新增】分裂逻辑
    if (type == "bug") {
        // 4a. 获取 "bug" 死亡时的路径信息
        const auto& currentPath = enemy->getAbsolutePath();
        int currentIndex = enemy->getCurrentPathIndex();
        QPointF deathPos = enemy->pos(); // "bug" 死亡的确切坐标

        // 4b. 为 "bugmini" 构建剩余路径
        //     新路径 = [死亡坐标, 目标航点, 目标航点+1, ...]
        std::vector<QPointF> remainingPath;
        remainingPath.push_back(deathPos); // 路径的第一个点是死亡坐标

        // 4c. 找到 "bug" 的下一个目标航点
        int nextWaypointIndex = currentIndex + 1;
        if (nextWaypointIndex < currentPath.size()) {
            // 将所有剩余的航点添加到新路径中
            for (size_t i = nextWaypointIndex; i < currentPath.size(); ++i) {
                remainingPath.push_back(currentPath[i]);
            }
        }

        // 4d. 【核心】调用 spawnByTypeWithPath 生成两个 "bugmini"
        //     我们给小 bug 一个 0.75 的缩放
        Enemy* child1 = spawnByTypeWithPath("bugmini", remainingPath, 0.75);
        Enemy* child2 = spawnByTypeWithPath("bugmini", remainingPath, 0.75);

        // 4e. 将第二只 "bugmini" 稍微偏移，防止它们完全重叠
        if (child2) {
            child2->setPos(child2->pos() + QPointF(5, -5)); // 5像素的小偏移
        }
    }
    // --- 分裂逻辑结束 ---

    // 5. (额外修复) 清理与该敌人相关的 Boss 状态
    //    防止在敌人被删除后，QHash 中留下悬垂指针
    if (m_healCd.contains(enemy)) {
        m_healCd.remove(enemy);
    }
    if (m_raged.contains(enemy)) {
        m_raged.remove(enemy);
    }

    // 6. 将原始的 "bug" 敌人添加到清理队列
    m_entitiesToClean.append(enemy);
    m_enemies.removeAll(enemy);

}

void GameManager::onBulletHitTarget(Bullet* bullet) {
    if (!m_bullets.contains(bullet)) {
        return;
    }
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
    if (!m_obstacles.contains(obstacle)) {
        return;
    }
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

        // --- 优先级 1: 永远优先寻找敌人 ---
        // (我们每一帧都执行这个搜索)
        Enemy* closestEnemy = nullptr;
        double minEnemyDistance = tower->getRange() + 1.0;

        for (Enemy* enemy : m_enemies) {
            double distance = QLineF(tower->pos(), enemy->pos()).length();

            // 检查敌人是否在塔的攻击范围内
            if (distance <= tower->getRange()) {
                // 如果是，再检查它是否是"最近"的敌人
                if (distance < minEnemyDistance) {
                    minEnemyDistance = distance;
                    closestEnemy = enemy;
                }
            }
        }

        // --- 决策 1: 是否找到了敌人？ ---
        if (closestEnemy) {
            // 是。敌人拥有最高优先级。
            // 无论当前目标是什么（哪怕是障碍物），立即切换目标为敌人。
            tower->setTarget(closestEnemy);
            continue; // 此塔的索敌逻辑在本帧完成
        }

        // --- 优先级 2: 没有敌人在范围内。检查障碍物 ---
        // (只有在 100% 确定没有敌人在范围内时，才执行这里的逻辑)

        // 2a. 检查是否已经在攻击一个有效的障碍物
        Obstacle* currentObstacle = dynamic_cast<Obstacle*>(tower->currentTarget);
        if (currentObstacle &&
            m_obstacles.contains(currentObstacle) &&
            tower->targetIsInRange()) {

            // 是。保持当前目标，继续攻击障碍物。
            continue; // 此塔的索敌逻辑在本帧完成
        }

        // 2b. 如果没有在攻击有效障碍物，则寻找一个新的障碍物
        Obstacle* closestObstacle = nullptr;
        double minObstacleDistance = tower->getRange() + 1.0;

        for (Obstacle* obstacle : m_obstacles) {
            double distance = QLineF(tower->pos(), obstacle->pos()).length();
            if (distance <= tower->getRange()) {
                if (distance < minObstacleDistance) {
                    minObstacleDistance = distance;
                    closestObstacle = obstacle;
                }
            }
        }

        // --- 决策 2: 是否找到了新的障碍物？ ---
        if (closestObstacle) {
            // 是。设置新目标为障碍物。
            tower->setTarget(closestObstacle);
        } else {
            // 否。范围内没有敌人，也没有障碍物。
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
    const QSize towerPixelSize(76, 76);
    for (Tower* tower : m_towers) {
        // 1. 获取塔的 "左上角" 绝对坐标
        QPointF towerTopLeftAbs = tower->pos();

        // 2. 计算塔的 "中心" 绝对坐标
        QPointF towerCenterAbs(towerTopLeftAbs.x() + towerPixelSize.width() / 2.0,
                               towerTopLeftAbs.y() + towerPixelSize.height() / 2.0);

        // 3. 将 "中心" 绝对坐标 转换回 相对坐标
        QPointF towerRelPos(towerCenterAbs.x() / m_screenSize.width(),
                            towerCenterAbs.y() / m_screenSize.height());
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
    const QSize towerPixelSize(76, 76);
    Tower* towerToSell = nullptr;

    for (Tower* tower : m_towers) {
        // 1a. 获取塔的 "左上角" 绝对坐标
        QPointF towerTopLeftAbs = tower->pos();

        // 1b. 计算塔的 "中心" 绝对坐标
        QPointF towerCenterAbs(towerTopLeftAbs.x() + towerPixelSize.width() / 2.0,
                               towerTopLeftAbs.y() + towerPixelSize.height() / 2.0);

        // 1c. 将 "中心" 绝对坐标 转换回 相对坐标
        QPointF towerRelPos(towerCenterAbs.x() / m_screenSize.width(),
                            towerCenterAbs.y() / m_screenSize.height());

        // 1d. 比较相对坐标
        if (qFuzzyCompare(towerRelPos, relativePosition)) {
            towerToSell = tower;
            break;
        }
    }

    if (!towerToSell) {
        // 没有找到塔
        return;
    }
    // // 2. 计算退款
    // int totalCost = towerToSell->getCost();
    // if (towerToSell->IsUpgraded()) {
    //     totalCost += towerToSell->getUpgradeCost();
    // }
    //
    // // 定义退款率为 70%
    // const double REFUND_RATE = 0.7;
    // int refundAmount = static_cast<int>(totalCost * REFUND_RATE);
    //
    // // 3. 返还资源
    // m_player->addResource(refundAmount);
    m_entitiesToClean.append(towerToSell);
    m_towers.removeAll(towerToSell);
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

    // 【新增】1. 定义标准尺寸
    const QSize enemyPixelSize(126, 126);

    // 【修改】2. 加载并缩放贴图
    QPixmap originalPixmap(pix);
    QPixmap scaledPixmap = originalPixmap.scaled(enemyPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // 【修改】3. 使用缩放后的贴图创建 Enemy
    Enemy* e = new Enemy(hp, spd, dmg, absPath, type, scaledPixmap);

    // 【新增】4. 设置偏移量，使其中心点在路径上
    e->setOffset(-enemyPixelSize.width() / 2.0, -enemyPixelSize.height() * 0.8);

    if (scale != 1.0) e->setScale(scale);

    m_scene->addItem(e);
    m_enemies.append(e);
    connect(e, &Enemy::reachedEnd, this, &GameManager::onEnemyReachedEnd);
    connect(e, &Enemy::died,       this, &GameManager::onEnemyDied);
    return e;
}