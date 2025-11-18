#ifndef WAVEMANAGER_H
#define WAVEMANAGER_H

#include <QObject>
#include <QList>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <vector>
#include <QSizeF>
#include <QRandomGenerator>
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
    void onEnemykilled(){totalEnemiesKilled++;}
    //更新函数
    void update();

    signals:
        void spawnEnemy(const QString& type, const std::vector<QPointF>& absolutePath);
    void allWavesCompleted();

private:
    void spawnEnemyAndResetCooldown();
    int intervalToTicks(double intervalInSeconds);
    QList<Wave> waves;
    int currentWaveIndex;
    QList<QString> m_spawnBag;
    int m_spawnCooldownTicks;//计时器，-1空闲，0准备就绪，>0正在倒计时
    int m_interWaveCooldownTicks;//波次间计时器
    int m_waveIntervalTicks;
    GameMap* gameMap;
    QSizeF screenSize;
    int totalEnemiesKilled = 0;
};

#endif // WAVEMANAGER_H
