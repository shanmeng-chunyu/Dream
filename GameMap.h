#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <vector>
#include <QPointF>
#include <QString>

// 用于存储从JSON加载的障碍物初始数据
struct ObstacleData {
    QString type;
    QString pixmapPath;
    QPointF relativePosition;
    int health;
    int resourceValue;
};

class GameMap {
public:
    GameMap();

    // 设置和获取敌人的行进路径（相对坐标）
    void setPath(const std::vector<QPointF>& path);
    const std::vector<QPointF>& getPath() const;

    // 设置和获取防御塔可放置的位置（相对坐标）
    void setTowerPositions(const std::vector<QPointF>& positions);
    const std::vector<QPointF>& getTowerPositions() const;

    // 设置和获取障碍物的初始数据
    void setObstacles(const std::vector<ObstacleData>& obstacles);
    const std::vector<ObstacleData>& getObstacles() const;

private:
    std::vector<QPointF> enemyPath;
    std::vector<QPointF> towerPositions;
    std::vector<ObstacleData> obstacleData;
};

#endif // GAMEMAP_H
