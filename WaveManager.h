#ifndef WAVEMANAGER_H
#define WAVEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <vector>
#include <QSizeF>
#include "GameMap.h"

struct EnemyWaveData {
    QString type;
    int count;
    double interval; // in seconds
};

struct Wave {
    QList<EnemyWaveData> enemies;
};

class WaveManager : public QObject {
    Q_OBJECT

public:
    explicit WaveManager(GameMap* map, QObject* parent = nullptr);
    void loadWaves(const QJsonArray& wavesData);
    void startNextWave();
    bool isFinished() const;
    void setScreenSize(const QSizeF& size);
    int getTotalEnemiesKilled() const { return totalEnemiesKilled; }

    signals:
        void spawnEnemy(const QString& type, const std::vector<QPointF>& absolutePath);
    void allWavesCompleted();

    private slots:
        void spawnEnemyFromQueue();

private:
    QList<Wave> waves;
    int currentWaveIndex;
    QList<EnemyWaveData> spawnQueue;
    QTimer* spawnTimer;
    GameMap* gameMap;
    QSizeF screenSize;
    int totalEnemiesKilled = 0;
};

#endif // WAVEMANAGER_H
