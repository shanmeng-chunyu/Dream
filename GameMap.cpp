#include "GameMap.h"

GameMap::GameMap() {
    // 构造函数现在非常简单，因为所有成员变量（std::vector）都会被自动初始化为空。
}

void GameMap::setPath(const std::vector<QPointF>& path) {
    enemyPath = path;
}

const std::vector<QPointF>& GameMap::getPath() const {
    return enemyPath;
}

void GameMap::setTowerPositions(const std::vector<QPointF>& positions) {
    towerPositions = positions;
}

const std::vector<QPointF>& GameMap::getTowerPositions() const {
    return towerPositions;
}

void GameMap::setObstacles(const std::vector<ObstacleData>& obstacles) {
    obstacleData = obstacles;
}

const std::vector<ObstacleData>& GameMap::getObstacles() const {
    return obstacleData;
}
