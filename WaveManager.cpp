#include "WaveManager.h"

WaveManager::WaveManager(GameMap* map, QObject* parent)
    : QObject(parent), currentWaveIndex(-1), gameMap(map), screenSize(800, 600) {
    spawnTimer = new QTimer(this);
    connect(spawnTimer, &QTimer::timeout, this, &WaveManager::spawnEnemyFromQueue);
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
        spawnTimer->start(spawnQueue.first().interval * 1000);
    }
}

void WaveManager::spawnEnemyFromQueue() {
    if (spawnQueue.isEmpty()) {
        spawnTimer->stop();
        if (currentWaveIndex >= waves.size() - 1) {
             emit allWavesCompleted();
        }
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
            spawnTimer->stop();
             if (currentWaveIndex >= waves.size() - 1) {
                emit allWavesCompleted();
            }
        } else {
            spawnTimer->setInterval(spawnQueue.first().interval * 1000);
        }
    }
}


bool WaveManager::isFinished() const {
    return currentWaveIndex >= waves.size() - 1 && spawnQueue.isEmpty();
}

void WaveManager::setScreenSize(const QSizeF& size) {
    screenSize = size;
}
