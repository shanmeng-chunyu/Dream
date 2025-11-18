#include "WaveManager.h"
#include <QRandomGenerator>
#include <algorithm>

WaveManager::WaveManager(GameMap* map, QObject* parent)
    : QObject(parent), currentWaveIndex(-1), gameMap(map), screenSize(800, 600) ,m_spawnCooldownTicks(-1),m_interWaveCooldownTicks(0),m_waveIntervalTicks(0){
}

void WaveManager::loadWaves(const QJsonArray& wavesData) {
    waves.clear();
    for (const auto& waveValue : wavesData) {
        Wave wave;
        QJsonArray enemiesData = waveValue.toObject()["enemies"].toArray();
        for (const auto& enemyValue : enemiesData) {
            QJsonObject enemyObj = enemyValue.toObject();
            EnemyWaveData enemyData;
            enemyData.type = enemyObj["type"].toString();
            enemyData.count = enemyObj["count"].toInt();
            enemyData.interval = enemyObj["interval"].toDouble();
            wave.enemies.append(enemyData);
        }
        waves.append(wave);
    }
}

void WaveManager::startNextWave() {
    if (currentWaveIndex + 1 >= waves.size()) {
        return;
    }

    currentWaveIndex++;
    m_spawnBag.clear(); // 1. 清空“袋子”

    // 2. 获取当前波次的所有“敌人组”
    QList<EnemyWaveData> enemyGroups = waves[currentWaveIndex].enemies;

    if (enemyGroups.isEmpty()) {
        // 这是一个空波次，直接跳过
        m_spawnCooldownTicks = -1;
        if (currentWaveIndex >= waves.size() - 1) {
            emit allWavesCompleted();
        } else {
            m_interWaveCooldownTicks = intervalToTicks(5.0); // 5 秒
        }
        return;
    }

    // 3. (关键) “装袋子”：把所有敌人都加进来
    for (const EnemyWaveData& group : enemyGroups) {
        for (int i = 0; i < group.count; ++i) {
            m_spawnBag.append(group.type);
        }
    }

    // 4. (关键) “洗牌”：随机打乱“袋子”
    std::shuffle(m_spawnBag.begin(), m_spawnBag.end(), *QRandomGenerator::global());

    // 5. 确定刷怪间隔：我们使用这个波次中“第一组”敌人的间隔
    //    (这是一个约定：第一个定义的敌人组的 interval 将是本波次的刷怪速度)
    m_waveIntervalTicks = intervalToTicks(enemyGroups.first().interval);

    // 6. 设置第一个怪的刷新倒计时
    m_spawnCooldownTicks = m_waveIntervalTicks;
    m_interWaveCooldownTicks = 0;
}

void WaveManager::spawnEnemyAndResetCooldown() {

    // 1. (新) 检查“袋子”是否为空
    if (m_spawnBag.isEmpty()) {
        // (旧逻辑，现在不需要了)
        return;
    }

    // 2. (新) 从“袋子”里抽一个
    QString enemyType = m_spawnBag.takeFirst();

    // 3. (旧) 获取路径并生成敌人
    std::vector<QPointF> absolutePath;
    const auto& relativePath = gameMap->getPath();
    for(const QPointF& relPt : relativePath) {
        absolutePath.emplace_back(relPt.x() * screenSize.width(), relPt.y() * screenSize.height());
    }
    emit spawnEnemy(enemyType, absolutePath);

    // 4. (新) 检查“袋子”是否被抽空了
    if (m_spawnBag.isEmpty()) {
        // “袋子”空了，这个波次*生成*完毕
        m_spawnCooldownTicks = -1; // 停止生成
        if (currentWaveIndex >= waves.size() - 1) {
            // 并且这是*最后*一波
            emit allWavesCompleted();
        } else {
            // 并且这不是最后一波，开始 5 秒的波次间歇
            m_interWaveCooldownTicks = intervalToTicks(5.0); // 5 秒
        }
    } else {
        // “袋子”没空，设置下一个敌人的生成CD
        m_spawnCooldownTicks = m_waveIntervalTicks;
    }
}


bool WaveManager::isFinished() const {
    // (新) 检查最后一个波次且“袋子”已空
    return currentWaveIndex >= waves.size() - 1 && m_spawnBag.isEmpty();
}

void WaveManager::setScreenSize(const QSizeF& size) {
    screenSize = size;
}

void WaveManager::update() {
    if (m_interWaveCooldownTicks > 0) {
        // 1. 优先处理波次间歇
        m_interWaveCooldownTicks--;
        if (m_interWaveCooldownTicks == 0) {
            // 2. 间歇结束，开始下一波
            startNextWave();
        }
    } else if (m_spawnCooldownTicks == 0) {
        // 3. 间歇为0 且 CD为0，生成一个敌人
        spawnEnemyAndResetCooldown();
    } else if (m_spawnCooldownTicks > 0) {
        // 4. 间歇为0 且 CD > 0，倒计时
        m_spawnCooldownTicks--;
    }
}

int WaveManager::intervalToTicks(double intervalInSeconds) {
    const int gameTickMs = 16;
    int ticks = static_cast<int>((intervalInSeconds * 1000.0) / gameTickMs);
    return std::max(1,ticks);
}