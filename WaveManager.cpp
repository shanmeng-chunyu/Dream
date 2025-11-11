#include "WaveManager.h"

WaveManager::WaveManager(GameMap* map, QObject* parent)
    : QObject(parent), currentWaveIndex(-1), gameMap(map), screenSize(800, 600) ,m_spawnCooldownTicks(-1){
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
        emit allWavesCompleted();
        return;
    }

    currentWaveIndex++;
    spawnQueue = waves[currentWaveIndex].enemies;
    if (!spawnQueue.isEmpty()) {
        m_spawnCooldownTicks = intervalToTicks(spawnQueue.first().interval);
    }
}

void WaveManager::spawnEnemyAndResetCooldown() {
    if (spawnQueue.isEmpty()) {
        if (currentWaveIndex >= waves.size() - 1) {
             emit allWavesCompleted();
        }
        m_spawnCooldownTicks = -1;
        return;
    }

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
             if (currentWaveIndex >= waves.size() - 1) {
                emit allWavesCompleted();
            }
            m_spawnCooldownTicks = -1;
        } else {
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
    if (m_spawnCooldownTicks == 0) {
        spawnEnemyAndResetCooldown();
    }else if (m_spawnCooldownTicks > 0) {
        m_spawnCooldownTicks--;
    }
}

int WaveManager::intervalToTicks(double intervalInSeconds) {
    const int gameTickMs = 16;
    int ticks = static_cast<int>((intervalInSeconds * 1000.0) / gameTickMs);
    return std::max(1,ticks);
}