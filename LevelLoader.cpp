#include "LevelLoader.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPointF>
#include <vector>

bool LevelLoader::loadLevel(const QString& filePath, GameMap& map, WaveManager& waveManager, Player& player) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject rootObj = doc.object();

    // Load map data
    QJsonObject mapObj = rootObj["map"].toObject();
    std::vector<QPointF> path;
    QJsonArray pathArray = mapObj["path"].toArray();
    for (const auto& pointValue : pathArray) {
        QJsonObject pointObj = pointValue.toObject();
        path.emplace_back(pointObj["x"].toDouble(), pointObj["y"].toDouble());
    }

    std::vector<QPointF> towerPositions;
    QJsonArray towerPosArray = mapObj["tower_positions"].toArray();
    for (const auto& pointValue : towerPosArray) {
        QJsonObject pointObj = pointValue.toObject();
        towerPositions.emplace_back(pointObj["x"].toDouble(), pointObj["y"].toDouble());
    }

    std::vector<ObstacleData> obstacles;
    QJsonArray obstaclesArray = mapObj["obstacles"].toArray();
    for (const auto& obsValue : obstaclesArray) {
        QJsonObject obsObj = obsValue.toObject();
        obstacles.emplace_back();
        ObstacleData& obsData = obstacles.back();
        obsData.type = obsObj["type"].toString();
        obsData.pixmapPath = obsObj["pixmap"].toString();

        QJsonObject posObj = obsObj["position"].toObject(); // 先拿出position对象
        obsData.relativePosition = QPointF(posObj["x"].toDouble(),
                                           posObj["y"].toDouble());

        obsData.health = obsObj["health"].toInt();
        obsData.resourceValue = obsObj["resource"].toInt();
    }

    // ... load obstacles in a similar way if needed by GameMap ...
    map.setPath(path);
    map.setTowerPositions(towerPositions);
    map.setObstacles(obstacles);

    // Load player data
    QJsonObject playerObj = rootObj["player"].toObject();
    player.setInitialState(playerObj["initial_stability"].toInt(), playerObj["initial_resource"].toInt());

    // Load wave data
    QJsonArray wavesArray = rootObj["waves"].toArray();
    waveManager.loadWaves(wavesArray);

    return true;
}
