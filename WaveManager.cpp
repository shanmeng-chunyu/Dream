#include "WaveManager.h"

WaveManager::WaveManager(GameMap* map, QObject* parent)
    : QObject(parent), currentWaveIndex(-1), gameMap(map), screenSize(800, 600) ,m_spawnCooldownTicks(-1),m_interWaveCooldownTicks(0){
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
    spawnQueue = waves[currentWaveIndex].enemies;
    if (!spawnQueue.isEmpty()) {
        m_spawnCooldownTicks = intervalToTicks(spawnQueue.first().interval);
    }

    m_interWaveCooldownTicks = 0;
}

void WaveManager::spawnEnemyAndResetCooldown() {
    EnemyWaveData& current = spawnQueue.first();

    // 将地图的相对路径转换为绝对路径
    std::vector<QPointF> absolutePath;
    const auto& relativePath = gameMap->getPath();
    for(const QPointF& relPt : relativePath) {
        absolutePath.emplace_back(relPt.x() * screenSize.width(), relPt.y() * screenSize.height());
    }

    emit spawnEnemy(current.type, absolutePath);

    current.count--;
    if (current.count <= 0) {
        spawnQueue.removeFirst();
        if (spawnQueue.isEmpty()) {
            // 这个波次*生成*完毕
            m_spawnCooldownTicks = -1; // 停止生成
            if (currentWaveIndex >= waves.size() - 1) {
                // 并且这是*最后*一波
                emit allWavesCompleted();
            } else {
                // 并且这不是最后一波，开始 5 秒的波次间歇
                m_interWaveCooldownTicks = intervalToTicks(5.0); // 5 秒
            }
        } else {
            // 队列没空，设置下一个敌人的生成CD
            m_spawnCooldownTicks = intervalToTicks(spawnQueue.first().interval);
        }
    }else {
        m_spawnCooldownTicks = intervalToTicks(current.interval);
    }
}


bool WaveManager::isFinished() const {
    return currentWaveIndex >= waves.size() - 1 && spawnQueue.isEmpty();
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