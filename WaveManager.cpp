#include "WaveManager.h"
#include "GameManager.h"
#include "Enemy.h"

WaveManager::WaveManager(QObject *parent)
    : QObject(parent), m_currentWave(0), m_enemiesToSpawn(0) {
    m_spawnTimer = new QTimer(this);
    connect(m_spawnTimer, &QTimer::timeout, this, &WaveManager::spawnEnemy);

    // ʾ����������
    m_waves.push_back({0, 10, 1000});
    m_waves.push_back({0, 15, 800});
}

void WaveManager::startNextWave() {
    if (m_currentWave >= m_waves.size()) {
        // ���в��������
        return;
    }

    WaveInfo &wave = m_waves[m_currentWave];
    m_enemiesToSpawn = wave.enemyCount;
    m_spawnTimer->start(wave.spawnInterval);

    m_currentWave++;
}

void WaveManager::loadWavesFromFile(const QString &filePath) {
    // ��JSON��XML�ļ����ز�����Ϣ
}

void WaveManager::spawnEnemy() {
    if (m_enemiesToSpawn <= 0) {
        m_spawnTimer->stop();
        return;
    }

    // ��: ����һ�����˲���ӵ���Ϸ��
    // ·��Ӧ�ôӹؿ������ж�ȡ
    std::vector<QPointF> path;
    path.push_back(QPointF(0, 100));
    path.push_back(QPointF(700, 100));
    path.push_back(QPointF(700, 400));

    Enemy *enemy = new Enemy(path);
    GameManager::instance()->addEnemy(enemy);

    m_enemiesToSpawn--;
}
