#include "GameMap.h"

GameMap::GameMap() {
    // ���캯�����ڷǳ��򵥣���Ϊ���г�Ա������std::vector�����ᱻ�Զ���ʼ��Ϊ�ա�
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
