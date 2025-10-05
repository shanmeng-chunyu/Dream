#ifndef DREAM_WAVEMANAGER_H
#define DREAM_WAVEMANAGER_H

#include <QObject>
#include <vector>
#include <QTimer>

struct WaveInfo {
    int enemyType;
    int enemyCount;
    int spawnInterval; // in milliseconds
};

class WaveManager : public QObject {
    Q_OBJECT

    public:
    explicit WaveManager(QObject *parent = nullptr);

    void startNextWave();

    void loadWavesFromFile(const QString &filePath);

    private slots:

        void spawnEnemy();

private:
    int m_currentWave;
    std::vector<WaveInfo> m_waves;
    QTimer *m_spawnTimer;
    int m_enemiesToSpawn;
};

#endif //DREAM_WAVEMANAGER_H
