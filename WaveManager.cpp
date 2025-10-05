#include "WaveManager.h"
#include "GameManager.h"
#include "Enemy.h"

WaveManager::WaveManager(QObject *parent)
    : QObject(parent), m_currentWave(0), m_enemiesToSpawn(0) {
    m_spawnTimer = new QTimer(this);
    connect(m_spawnTimer, &QTimer::timeout, this, &WaveManager::spawnEnemy);

    // 示例波次数据
    m_waves.push_back({0, 10, 1000});
    m_waves.push_back({0, 15, 800});
}

void WaveManager::startNextWave() {
    if (m_currentWave >= m_waves.size()) {
        // 所有波次已完成
        return;
    }

    WaveInfo &wave = m_waves[m_currentWave];
    m_enemiesToSpawn = wave.enemyCount;
    m_spawnTimer->start(wave.spawnInterval);

    m_currentWave++;
}

void WaveManager::loadWavesFromFile(const QString &filePath) {
    // 从JSON或XML文件加载波次信息
}

void WaveManager::spawnEnemy() {
    if (m_enemiesToSpawn <= 0) {
        m_spawnTimer->stop();
        return;
    }

    // 简化: 创建一个敌人并添加到游戏中
    // 路径应该从关卡数据中读取
    std::vector<QPointF> path;
    path.push_back(QPointF(0, 100));
    path.push_back(QPointF(700, 100));
    path.push_back(QPointF(700, 400));

    Enemy *enemy = new Enemy(path);
    GameManager::instance()->addEnemy(enemy);

    m_enemiesToSpawn--;
}
