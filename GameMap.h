#pragma once

#include <QObject>
#include <QPixmap>
#include <QPointF>
#include <vector>

// =================================================================
// 新增内容: ObstacleData 结构体
// 说明: 用于在加载和编辑关卡时，存储单个障碍物的核心数据。
//      将数据（位置、类型）与它的图形表现（Obstacle类）分离，
//      是一种良好的解耦设计。
struct ObstacleData {
    QPointF position;
    int typeId;
};
// =================================================================

class GameMap : public QObject {
    Q_OBJECT

public:
    explicit GameMap(QObject* parent = nullptr);

    bool loadMap(int levelId);

    const std::vector<QPointF>& getPathPoints() const;
    const std::vector<QPointF>& getTowerBasePositions() const;

    // =================================================================
    // 新增内容: 获取障碍物数据的接口
    // 说明: 为GameManager和场景渲染模块提供所有障碍物的数据。
    const std::vector<ObstacleData>& getObstacles() const;
    // =================================================================

    const QPixmap& getBackground() const;

private:
    std::vector<QPointF> pathPoints;
    std::vector<QPointF> towerBasePositions;
    QPixmap mapBackground;

    // =================================================================
    // 新增内容: 存储障碍物数据的容器
    std::vector<ObstacleData> obstacles;
    // =================================================================
};
