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
#include "ConfigHelper.h"

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
#include <QSoundEffect>
#include <QUrl>

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

    preloadAllSounds();

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
        qreal relWidth = (qreal)obstaclePixelSize.width() / m_screenSize.width();
        qreal relHeight = (qreal)obstaclePixelSize.height() / m_screenSize.height();
        QPointF relCenter = obsData.relativePosition;
        QRectF relativeRect(relCenter.x() - relWidth / 2.0,
                            relCenter.y() - relHeight / 2.0,
                            relWidth,
                            relHeight);

        auto* obstacle = new Obstacle(obsData.health, obsData.resourceValue, scaledPixmap, relativeRect);

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
    QFile enemyFile(getConfigFile("enemy_data.json"));
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
    QFile towerFile(getConfigFile("tower_data.json"));
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
    const QList<Enemy*> enemiesSnapshot = m_enemies;
    for (Enemy* enemy : enemiesSnapshot) {
        enemy->move();

        /* ----------- Boss 狂暴（第一关） ----------- */
        if (enemy->getType() == "thesis" && !m_raged.contains(enemy)) {
            const QJsonObject proto = m_enemyPrototypes["thesis"];
            const double thr = proto.value("rageThreshold").toDouble(0.4);
            const int maxHp = proto.value("health").toInt();

            if (maxHp > 0 && (double)enemy->getHealth() / maxHp < thr) {
                const double baseSpd = proto.value("speed").toDouble();
                const double mul = proto.value("rageSpeedMul").toDouble(1.6);
                enemy->setBaseSpeed(baseSpd * mul);
                m_raged.insert(enemy);
            }
        }

        /* ----------- 泪水怪回血（第二关） ----------- */
        if (enemy->getType() == "tears") {
            const QJsonObject proto = m_enemyPrototypes["tears"];
            const int maxHp = proto.value("health").toInt();
            if (maxHp > 0) {
                const double thr      = proto.value("healBelowHp").toDouble(0.7);
                const int heal        = proto.value("healPerTick").toInt(100);
                const double radius   = proto.value("healRadius").toDouble(400.0);
                const double interval = proto.value("healInterval").toDouble(1.0);

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
        // ---------- 昔日幻影：半血以下减速 + 自愈（第二关 Boss） ----------
        if (enemy->getType() == "thephantomofthepast") {

            const QJsonObject proto = m_enemyPrototypes["thephantomofthepast"];
            const int maxHp        = proto.value("health").toInt();
            const double thr       = proto.value("selfRegenBelowHp").toDouble(0.5);
            const int heal         = proto.value("selfRegenPerTick").toInt(40);
            const double interval  = proto.value("selfRegenInterval").toDouble(1.5);
            const double slowMul   = proto.value("slowMul").toDouble(0.2);  // 半血减速倍率

            if (maxHp > 0 && (double)enemy->getHealth() / maxHp < thr) {

                // ★★★ 移速衰减版本（不停止） ★★★
                enemy->setBaseSpeed(proto.value("speed").toDouble() * slowMul);

                // 冷却（与泪水怪共用）
                int& cd = m_healCd[enemy];
                if (cd <= 0) {
                    enemy->heal(heal);
                    cd = qMax(1, (int)std::round(interval * 60.0));
                } else {
                    --cd;
                }
            }
        }
    }

    /* ========= 子弹移动 ========= */
    const QList<Bullet*> bulletsSnapshot = m_bullets;
    for (Bullet* bullet : bulletsSnapshot) {
        // 必须检查子弹是否还“活着”（可能被其他逻辑删除了，虽然子弹通常是一次性的）
        if (!m_bullets.contains(bullet)) continue;
        bullet->move();
        // move() 可能会触发 onBulletHitTarget -> removeAll()
        // 但因为我们遍历的是 snapshot，所以不会崩。
    }

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

    // 检查生成的敌人类型是否为 "nightmare"
    if (type == "nightmare") {
        // 如果是，立即调用 destroyAllTowers
        playSound(m_nightmareSpawnSound);
        destroyAllTowers(true);
    }

}

void GameManager::buildTower(const QString& type, const QPointF& relativePosition) {
    if (!m_towerPrototypes.contains(type)) return;

    QJsonObject proto = m_towerPrototypes[type];
    int cost = proto["cost"].toInt();
    QString pixmap_path = proto["gif_path"].toString();
    if (pixmap_path == "") {
        pixmap_path = proto["pixmap"].toString();
    }

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
    const QSize towerPixelSize(76, 76);

    if (type == "InspirationBulb") {
        tower = new InspirationBulb(pixelRange,pixmap_path,towerPixelSize);
    }else if (type == "KnowledgeTree") {
        tower = new KnowledgeTree(pixelRange,pixmap_path,towerPixelSize);
    }else if (type == "FishingCatPillow") {
        tower = new FishingCatPillow(pixelRange,pixmap_path,towerPixelSize);
        FishingCatPillow* pillow = qobject_cast<FishingCatPillow*>(tower);
        connect(pillow,&FishingCatPillow::applyControl,this,&GameManager::onApplyEnemyControl);
    }else if (type == "LiveCoffee") {
        tower = new LiveCoffee(pixelRange,pixmap_path,towerPixelSize);
    }else if (type == "WarmMemories") {
        QString wm_charge_path;
        wm_charge_path = ":/towers/resources/towers/level2/WarmMemories_charge.gif";
        tower = new WarmMemory(pixelRange,pixmap_path,wm_charge_path,towerPixelSize);
        WarmMemory* memory = qobject_cast<WarmMemory*>(tower);
        connect(memory, &WarmMemory::applyControl, this, &GameManager::onApplyEnemyControl);
    }else if (type == "NightRadio") {
        tower = new NightRadio(pixelRange,pixmap_path,towerPixelSize);
    }else if (type == "PettingCatTime") {
        tower = new PettingCatTime(pixelRange,pixmap_path,towerPixelSize);
        PettingCatTime* catTime = qobject_cast<PettingCatTime*>(tower);
        connect(catTime, &PettingCatTime::applyControl, this, &GameManager::onApplyEnemyControl);
    }else if (type == "Companionship") {
        tower = new FriendCompanion(pixelRange,pixmap_path,towerPixelSize);
    }


    if (tower) {
        // 从 JSON 中读取 "damage_upgrade"
        int upgradeDmg = proto["damage_upgrade"].toInt();
        // 注入到塔对象中
        tower->setDamageUpgradeValue(upgradeDmg);
    }
    // 设置偏移量，将 (0,0) 点移动到中心
    const QPointF towerTopLeftPos(absPos.x() - towerPixelSize.width() / 2.0,
                                  absPos.y() - towerPixelSize.height() / 2.0);
    tower->setPos(towerTopLeftPos);
    playSound(m_buildSound);
    m_scene->addItem(tower);
    m_towers.append(tower);
    if (tower->getAuraItem())
    {
        // 1. 计算塔的中心点
        const QPointF towerCenter(towerTopLeftPos.x() + towerPixelSize.width() / 2.0,
                                  towerTopLeftPos.y() + towerPixelSize.height() / 2.0);
        // 2. 计算光环的左上角 (光环是 300x300)
        const QPointF auraTopLeft(towerCenter.x() - 300.0 / 2.0,
                                    towerCenter.y() - 300.0 / 2.0);
        tower->getAuraItem()->setPos(auraTopLeft); // 定位光环
        m_scene->addItem(tower->getAuraItem());    // 添加光环
    }
    connect(tower,&Tower::newBullet,this,&GameManager::onNewBullet);
}


void GameManager::onNewBullet(Tower* tower, QGraphicsPixmapItem* target) {
    // 根据发射塔的类型查找对应的子弹贴图
    // 1. 定义子弹的固定像素大小 (按照你的要求)
    const QSize bulletPixelSize(76, 76);
    const QSize towerPixelSize(76, 76);

    // 2. 获取子弹贴图原型
    QString type = tower->getType();
    QJsonObject proto = m_towerPrototypes[type];
    bool isUpgraded = tower->upgraded;
    QString bulletPixmapPath;
    // 3. 根据升级状态选择贴图路径
    if (isUpgraded && proto.contains("bullet_pixmap_upgrade")) {
        // 如果已升级，并且JSON中有升级版贴图
        bulletPixmapPath = proto["bullet_pixmap_upgrade"].toString();
    } else {
        // 否则，使用默认贴图
        bulletPixmapPath = proto["bullet_pixmap"].toString(); //
    }

    // 4. 使用我们选择的路径加载贴图
    QPixmap originalPixmap(bulletPixmapPath);

    // 3. 缩放贴图
    QPixmap scaledPixmap = originalPixmap.scaled(bulletPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // (健壮性检查：如果贴图路径错误，创建一个红色的方块)
    if (scaledPixmap.isNull()) {
        scaledPixmap = QPixmap(bulletPixelSize);
        scaledPixmap.fill(Qt::red);
    }
    Bullet::DamageType damageType = Bullet::SingleTarget; // 默认
    double aoeRadius = 0.0;

    // 从 tower_data.json 读取 "KnowledgeTree" 的特殊配置
    if (type == "KnowledgeTree") {
        damageType = Bullet::AreaOfEffect;
        // 你应该在 tower_data.json 中添加 "aoe_radius": 100.0 这样的字段
        // 这里我们先硬编码一个值
        aoeRadius = 100.0;
    }
    // 从 tower_data.json 读取 "NightRadio" 的特殊配置
    else if (type == "NightRadio") {
        damageType = Bullet::Piercing;
        // 穿透伤害不需要半径
        playSound(m_radioatkSound);
    }else {
        playSound(m_bulbatkSound);
    }
    // (其他塔，如 InspirationBulb，会使用默认的 SingleTarget)
    // 创建子弹实例 (使用缩放后的贴图)
    // 4. [修改] 计算塔的中心点 (子弹发射点)
    QPointF towerTopLeft = tower->pos();
    QPointF spawnCenterPos(towerTopLeft.x() + towerPixelSize.width() / 2.0,
                           towerTopLeft.y() + towerPixelSize.height() / 2.0);

    auto* bullet = new Bullet(tower->getDamage(), 10.0, target,
                              damageType, spawnCenterPos, aoeRadius,
                              scaledPixmap);

    // 6. 计算子弹的 top-left 位置 (保持不变)
    QPointF bulletTopLeftPos(spawnCenterPos.x() - bulletPixelSize.width() / 2.0,
                             spawnCenterPos.y() - bulletPixelSize.height() / 2.0);
    bullet->setPos(bulletTopLeftPos);

    m_scene->addItem(bullet);
    m_bullets.append(bullet);
    connect(bullet, &Bullet::hitTarget, this, &GameManager::onBulletHitTarget);
    if (bullet->getDamageType() == Bullet::Piercing) {
        connect(bullet, &Bullet::hitEnemy, this, &GameManager::onBulletHitEnemy);
        connect(bullet, &Bullet::hitObstacle, this, &GameManager::onBulletHitObstacle);
    }
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
        std::vector<QPointF> remainingPath;
        remainingPath.push_back(deathPos); // 路径的第一个点是死亡坐标

        int nextWaypointIndex = currentIndex + 1;
        if (nextWaypointIndex < currentPath.size()) {
            // 将所有剩余的航点添加到新路径中
            for (size_t i = nextWaypointIndex; i < currentPath.size(); ++i) {
                remainingPath.push_back(currentPath[i]);
            }
        }

        // 4c. 【核心】生成两个 "bugmini"
        // (它们俩此时都会在 deathPos)
        Enemy* child1 = spawnByTypeWithPath("bugmini", remainingPath, 0.75);
        Enemy* child2 = spawnByTypeWithPath("bugmini", remainingPath, 0.75);

        // 4d. 【替换】计算沿路径前后的分裂位置
        if (child1 && child2) {
            QPointF pathVector; // 存储路径的 (dx, dy) 方向向量

            // 1. 尝试获取 "前进" 方向 (朝向下一个航点)
            if (nextWaypointIndex < currentPath.size()) {
                QPointF nextWaypoint = currentPath[nextWaypointIndex];
                QLineF pathLine(deathPos, nextWaypoint);
                if (pathLine.length() > 0.01) { // 避免除零
                    QLineF unitPathLine = pathLine.unitVector();
                    pathVector = unitPathLine.p2() - unitPathLine.p1();
                }
            }

            // 2. 如果没有前进方向 (例如死在终点)，尝试获取 "后退" 方向 (来自上一个航点)
            if (pathVector.isNull() && currentIndex > 0) {
                QPointF prevWaypoint = currentPath[currentIndex]; // 注意是 currentIndex，不是-1
                QLineF pathLine(prevWaypoint, deathPos);
                if (pathLine.length() > 0.01) {
                    QLineF unitPathLine = pathLine.unitVector();
                    pathVector = unitPathLine.p2() - unitPathLine.p1();
                }
            }

            // 3. 如果仍然没有方向 (罕见)，默认水平分裂
            if (pathVector.isNull()) {
                pathVector = QPointF(1.0, 0.0);
            }

            // 定义弹开的距离 (20 像素)
            qreal offsetAmount = 25.0;

            // 4e. 【替换】将两个 bugmini 设置到弹开后的位置
            // child1 放在前面
            child1->setPos(deathPos + pathVector * offsetAmount);
            // child2 放在后面
            child2->setPos(deathPos - pathVector * offsetAmount);
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
    m_enemies.removeAll(enemy);

}

void GameManager::onBulletHitTarget(Bullet* bullet) {
    if (!m_bullets.contains(bullet)) {
        return;
    }
    // 1. 获取子弹的所有信息
    int damage = bullet->getDamage();
    Bullet::DamageType type = bullet->getDamageType();
    QGraphicsPixmapItem* mainTargetItem = bullet->getTarget(); // 子弹瞄准的目标
    // 2. 根据类型执行不同的伤害逻辑
    switch (type) {

        case Bullet::SingleTarget: {
            // --- 逻辑 1: 单体伤害 (原逻辑) ---
            if (mainTargetItem) {
                Enemy *enemyTarget = dynamic_cast<Enemy*>(mainTargetItem);
                if (enemyTarget && m_enemies.contains(enemyTarget)) {
                    enemyTarget->takeDamage(damage);
                } else {
                    Obstacle* obstacleTarget = dynamic_cast<Obstacle*>(mainTargetItem);
                    if (obstacleTarget && m_obstacles.contains(obstacleTarget)) {
                        obstacleTarget->takeDamage(damage);
                    }
                }
            }
            break; // 结束
        }

        case Bullet::AreaOfEffect: {
            // --- 逻辑 2: 范围伤害 (KnowledgeTree) ---
            double aoeRadius = bullet->getAoeRadius();
            QPointF impactCenter; // 爆炸中心

            // 检查目标是否还存在
            if (mainTargetItem && m_enemies.contains(dynamic_cast<Enemy*>(mainTargetItem))) {
                // 目标存活，在目标当前位置爆炸
                impactCenter = mainTargetItem->pos();
            } else {
                // 目标已死亡，在子弹的终点 (m_lastKnownPos) 爆炸
                impactCenter = bullet->pos() + bullet->transformOriginPoint();
            }

            const QList<Enemy*> enemiesSnapshot = m_enemies;
            // 遍历所有敌人，检查是否在爆炸半径内
            for (Enemy* enemy : enemiesSnapshot) {
                if (!m_enemies.contains(enemy)) continue;
                double distance = QLineF(impactCenter, enemy->pos()).length();
                if (distance <= aoeRadius) {
                    enemy->takeDamage(damage);
                }
            }
            const QList<Obstacle*> obstaclesSnapshot = m_obstacles;

            for (Obstacle* obstacle : obstaclesSnapshot) {
                if (!m_obstacles.contains(obstacle)) continue;

                double distance = QLineF(impactCenter, obstacle->pos()).length();
                if (distance <= aoeRadius) {
                    obstacle->takeDamage(damage);
                }
            }
            playSound(m_hitSoundAOE);
            break; // 结束
        }

        case Bullet::Piercing: {
            // 子弹到达终点时，穿透弹什么也不做。
            // 伤害已经在 onBulletHitEnemy 中处理了。
            // 我们只需要让它在 switch 之后被正常清理。
            break;
        }
    } // 结束 switch

    // (清理逻辑保持不变)
    m_entitiesToClean.append(bullet);
    m_bullets.removeAll(bullet);
}

void GameManager::onObstacleDestroyed(Obstacle* obstacle, int resourceValue) {
    if (!m_obstacles.contains(obstacle)) {
        return;
    }
    m_player->addResource(resourceValue);
    emit obstacleCleared(obstacle->getRelativeRect());
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
        Enemy* closestTaunter = nullptr;
        double minTauntDistance = tower->getRange() + 1.0;

        // 【新增】获取塔的绝对中心点坐标
        // sceneBoundingRect() 会自动处理塔的位置和大小，返回准确的包围盒
        QPointF towerCenter = tower->sceneBoundingRect().center();

        // 第一次遍历：只寻找 "pre" 类型的敌人
        for (Enemy* enemy : m_enemies) {
            if (enemy->getType() == "pre") {
                // 【修改】使用 towerCenter 而不是 tower->pos()
                double distance = QLineF(towerCenter, enemy->pos()).length();

                if (distance <= tower->getRange()) {
                    if (distance < minTauntDistance) {
                        minTauntDistance = distance;
                        closestTaunter = enemy;
                    }
                }
            }
        }

        if (closestTaunter) {
            tower->setTarget(closestTaunter);
            continue;
        }

        // --- 优先级 1: 永远优先寻找敌人 ---
        Enemy* closestEnemy = nullptr;
        double minEnemyDistance = tower->getRange() + 1.0;

        for (Enemy* enemy : m_enemies) {
            // 【修改】使用 towerCenter 而不是 tower->pos()
            double distance = QLineF(towerCenter, enemy->pos()).length();

            if (distance <= tower->getRange()) {
                if (distance < minEnemyDistance) {
                    minEnemyDistance = distance;
                    closestEnemy = enemy;
                }
            }
        }

        if (closestEnemy) {
            tower->setTarget(closestEnemy);
            continue;
        }

        // --- 优先级 2: 检查障碍物 ---
        Obstacle* currentObstacle = dynamic_cast<Obstacle*>(tower->getCurrentTarget());
        if (currentObstacle &&
            m_obstacles.contains(currentObstacle) &&
            tower->targetIsInRange()) {
            // 注意：tower->targetIsInRange() 内部可能也用了 pos()，也需要检查 Tower.cpp
            continue;
        }

        Obstacle* closestObstacle = nullptr;
        double minObstacleDistance = tower->getRange() + 1.0;

        for (Obstacle* obstacle : m_obstacles) {
            // 【修改】使用 towerCenter 而不是 tower->pos()
            // 注意：障碍物的 pos() 通常已经是中心点（在 GameManager 中设置过偏移），但为了保险也可以用 center()
            // 这里假设 obstacle->pos() 是正确的（通常是中心）
            double distance = QLineF(towerCenter, obstacle->pos()).length();

            if (distance <= tower->getRange()) {
                if (distance < minObstacleDistance) {
                    minObstacleDistance = distance;
                    closestObstacle = obstacle;
                }
            }
        }

        if (closestObstacle) {
            tower->setTarget(closestObstacle);
        } else {
            tower->setTarget(nullptr);
        }
    }
}

void GameManager::checkWinLossConditions() {
    if (m_player->getStability() <= 0) {
        m_gameIsOver = true;
        m_gameTimer->stop();
        playSound(m_defeatSound);
        emit gameFinished(false,m_player->getStability(), m_waveManager->getTotalEnemiesKilled());
        return;
        // 此处可以发射一个游戏失败的信号
    }

    if (m_waveManager->isFinished() && m_enemies.isEmpty()) {
        m_gameIsOver = true;
        m_gameTimer->stop();
        playSound(m_victorySound);
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
                playSound(m_upgradeSound);
                if (tower->getAuraItem())
                {
                    // (我们假设塔的大小和位置不变)
                    const QPointF towerTopLeftAbs = tower->pos();
                    const QSize towerPixelSize(76, 76);
                    const QPointF towerCenter(towerTopLeftAbs.x() + towerPixelSize.width() / 2.0,
                                              towerTopLeftAbs.y() + towerPixelSize.height() / 2.0);
                    const QPointF auraTopLeft(towerCenter.x() - 300.0 / 2.0,
                                                towerCenter.y() - 300.0 / 2.0);
                    // 重新设置光环的位置 (这会更新 upgrade() 中修改过的光环贴图)
                    tower->getAuraItem()->setPos(auraTopLeft);
                }
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
    // 2. 计算退款
    int totalCost = towerToSell->getCost();
    if (towerToSell->IsUpgraded()) {
        totalCost += towerToSell->getUpgradeCost();
    }

    // 定义退款率为 70%
    const double REFUND_RATE = 0.7;
    int refundAmount = static_cast<int>(totalCost * REFUND_RATE);

    // 3. 返还资源
    m_player->addResource(refundAmount);
    playSound(m_sellSound);
    if (towerToSell->getAuraItem()) {
        m_scene->removeItem(towerToSell->getAuraItem());
        // 将光环也加入清理队列
        m_entitiesToClean.append(towerToSell->getAuraItem());
    }
    m_entitiesToClean.append(towerToSell);
    m_towers.removeAll(towerToSell);
    cleanupEntities();
}

void GameManager::pauseGame() {
    m_gameTimer->stop();
    for (Enemy* enemy : m_enemies) {
        if (enemy) {
            enemy->pauseAnimation();
        }
    }
    for (Tower* tower : m_towers) {
        if (tower) {
            tower->pauseAnimation(); //
        }
    }
}

void GameManager::resumeGame() {
    if (!m_gameIsOver) {
        m_gameTimer->start(16);
        for (Enemy* enemy : m_enemies) {
            if (enemy) {
                enemy->resumeAnimation();
            }
        }
        for (Tower* tower : m_towers) {
            if (tower) {
                tower->resumeAnimation(); //
            }
        }
    }
}

void GameManager::onApplyEnemyControl(QGraphicsPixmapItem* enemy,double duration) {
    Enemy* enemyTarget = static_cast<Enemy*>(enemy);
    if (!enemyTarget || !m_enemies.contains(enemy)) {
        return;
    }
    // 1. 【新增】获取发射信号的塔
    //    我们使用 QObject::sender() 来找出是哪座塔触发了这个信号
    Tower* sendingTower = qobject_cast<Tower*>(sender());

    // 2. 【新增】如果找到了塔，就去获取它在 tower_data.json 中定义的“子弹贴图”
    if (sendingTower) {
        QString towerType = sendingTower->getType(); //
        QJsonObject proto = m_towerPrototypes[towerType]; //

        // 我们利用 tower_data.json 中已有的 "bullet_pixmap" 字段

        QString effectPath;
        if (towerType == "PettingCatTime")
        {
            // 它使用“摸鱼猫猫枕”的图标
            effectPath = m_towerPrototypes["FishingCatPillow"].value("bullet_pixmap").toString();
        }else {
            bool isUpgraded = sendingTower->upgraded;
            if (isUpgraded) {
                effectPath = proto.value("bullet_pixmap_upgrade").toString();
            }else {
                effectPath = proto.value("bullet_pixmap").toString();
            }
        }


        if (!effectPath.isEmpty()) {
            QPixmap effectPixmap(effectPath);
            if (!effectPixmap.isNull()) {
                // 4. 【新增】调用敌人的新函数来显示特效
                //    (特效时长与眩晕时长一致)
                enemyTarget->applyVisualEffect(effectPixmap, duration);
            }
        }
    }
    QString towerType = sendingTower ? sendingTower->getType() : "";
    if (towerType == "FishingCatPillow" || towerType == "WarmMemories")
    {
        enemyTarget->stopFor(duration);
    }
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
    const QString gifPath = proto.value("gif_path").toString();
    const QString pixPath = proto.value("pixmap").toString();
    QString RealPath;
    if (gifPath != "") {
        RealPath = gifPath;
    }else {
        RealPath = pixPath;
    }
    // 【新增】1. 定义标准尺寸
    QSize enemyPixelSize(126, 126);
    if (type == "thesis" || type == "past"  || type == "nightmare") {
        enemyPixelSize = QSize(200,200);
    }



    // 【修改】3. 使用缩放后的贴图创建 Enemy
    Enemy* e = new Enemy(hp, spd, dmg, absPath, type, RealPath, enemyPixelSize);

    // 【新增】4. 设置偏移量，使其中心点在路径上
    e->setOffset(-enemyPixelSize.width() / 2.0, -enemyPixelSize.height() * 0.8);

    if (scale != 1.0) e->setScale(scale);

    m_scene->addItem(e);
    m_enemies.append(e);
    connect(e, &Enemy::reachedEnd, this, &GameManager::onEnemyReachedEnd);
    connect(e, &Enemy::died,       this, &GameManager::onEnemyDied);
    connect(e, &Enemy::deathAnimationFinished, this, &GameManager::onEnemyDeathAnimationFinished);
    return e;
}

void GameManager::onBulletHitEnemy(Bullet* bullet, Enemy* enemy)
{
    // 检查子弹和敌人是否都还“存活”
    if (!bullet || !enemy || !m_bullets.contains(bullet) || !m_enemies.contains(enemy)) {
        return;
    }

    // 对穿透的敌人造成伤害
    enemy->takeDamage(bullet->getDamage());
}

void GameManager::onBulletHitObstacle(Bullet* bullet, Obstacle* obstacle)
{
    // 检查子弹和障碍物是否都还“存活”
    if (!bullet || !obstacle || !m_bullets.contains(bullet) || !m_obstacles.contains(obstacle)) {
        return;
    }

    // 对穿透的障碍物造成伤害
    obstacle->takeDamage(bullet->getDamage());

}

void GameManager::destroyAllTowers(bool withEffects)
{
    // (可选：你可以在这里播放一个全屏的爆炸音效或视觉特效)

    // 1. 遍历当前所有的塔
    for (Tower* tower : m_towers) {
        // 2. 将它们全部加入到 m_entitiesToClean 队列
        //    这能确保它们在当前帧的末尾被安全地从场景中移除和删除
        if (tower->getAuraItem()) {
            m_entitiesToClean.append(tower->getAuraItem());
        }
        m_entitiesToClean.append(tower);
    }

    // 3. 立即清空 m_towers 列表，这样它们就不能再进行任何索敌或攻击
    m_towers.clear();
}

void GameManager::onEnemyDeathAnimationFinished(Enemy* enemy)
{
    // 动画播放完毕，现在可以安全地将敌人添加到清理队列
    if (enemy && !m_entitiesToClean.contains(enemy)) {
        m_entitiesToClean.append(enemy);
    }
}

void GameManager::clearGameScene()
{
    m_gameTimer->stop();
    m_gameIsOver = true; // 阻止任何正在进行的 updateGame 逻辑

    // 1. 收集所有场景中的实体
    for (Tower* tower : m_towers) {
        if (tower->getAuraItem()) {
            m_entitiesToClean.append(tower->getAuraItem());
        }
        m_entitiesToClean.append(tower);
    }
    m_towers.clear();

    for (Enemy* enemy : m_enemies) {
        // (注意：如果敌人正在播放死亡动画，我们也要立即清理)
        m_entitiesToClean.append(enemy);
    }
    m_enemies.clear();

    for (Bullet* bullet : m_bullets) {
        m_entitiesToClean.append(bullet);
    }
    m_bullets.clear();

    for (Obstacle* obstacle : m_obstacles) {
        m_entitiesToClean.append(obstacle);
    }
    m_obstacles.clear();

    // 2. 清理所有状态追踪器
    m_raged.clear();
    m_healCd.clear();
    m_activeDeathAnimations.clear();

    // 3. 立即执行清理
    cleanupEntities();
    Q_ASSERT(m_entitiesToClean.isEmpty()); // 确保清理完成

    // 4. 重置 WaveManager
    if (m_waveManager) {
        disconnect(m_waveManager, nullptr, this, nullptr);
        delete m_waveManager;
    }
    m_waveManager = new WaveManager(m_gameMap, this);
    connect(m_waveManager, &WaveManager::spawnEnemy, this, &GameManager::onSpawnEnemy);

    // 5. (可选) 重置 Player 状态，但这通常在 loadLevel 时完成
    // m_player->setInitialState(0, 0);
}

QSoundEffect* GameManager::createSound(const QString& fileName, double volume) {
    QString targetPath;

    // 1. 优先级 A: 检查 .exe 旁边的 sounds 文件夹 (发布/Release模式)
    QString releasePath = QCoreApplication::applicationDirPath() + "/sounds/" + fileName;

    // 2. 优先级 B: 检查源码目录 (开发/Debug模式)
    // (利用 CMakeLists.txt 中定义的 PROJECT_ROOT)
#ifdef PROJECT_ROOT
    QString devPath = QString(PROJECT_ROOT) + "/sounds/" + fileName;
#else
    QString devPath = "";
#endif

    if (QFile::exists(releasePath)) {
        targetPath = releasePath;
    } else if (!devPath.isEmpty() && QFile::exists(devPath)) {
        targetPath = devPath;
    } else {
        qWarning() << "[Audio Error] File not found:" << fileName
                   << "\nSearched:" << releasePath << "\nAnd:" << devPath;
        return nullptr; // 文件未找到，返回空指针
    }

    // 3. 创建并初始化音效
    QSoundEffect* sound = new QSoundEffect(this);
    sound->setSource(QUrl::fromLocalFile(targetPath));
    sound->setVolume(volume);

    // 验证加载是否成功（非必须，但有助于调试）
    // if (sound->status() == QSoundEffect::Error) {
    //     qWarning() << "Failed to decode audio:" << targetPath;
    // }

    return sound;
}

void GameManager::preloadAllSounds() {
    // 使用 createSound 一次性加载所有成员变量
    // 这样程序刚启动，这些对象就都在内存里准备好了

    m_buildSound = createSound("build_tower.wav", 0.5);
    m_sellSound  = createSound("sell_tower.wav", 0.5);

    m_bulbatkSound  = createSound("bulb_atk.wav", 0.5);
    m_radioatkSound = createSound("radio_atk.wav", 0.5);

    m_hitSoundAOE   = createSound("explosion.wav", 0.5);

    m_victorySound = createSound("gamewin.wav", 1.0);
    m_defeatSound  = createSound("gamelose.wav", 1.0);

    m_upgradeSound = createSound("upgrade.wav", 0.6);
    m_nightmareSpawnSound = createSound("nightmare.wav", 0.9);

    qDebug() << "All audio assets pre-loaded.";
}

void GameManager::playSound(QSoundEffect* sound) {
    // 如果 sound 是 nullptr（文件没找到），或者状态错误，就不播放，避免崩溃
    if (sound && sound->status() != QSoundEffect::Error) {
        // 如果需要支持密集播放，可以在这里加 sound->stop()，但在塔防里通常不需要
        sound->play();
    }
}