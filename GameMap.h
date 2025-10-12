#pragma once

#include <QObject>
#include <QPixmap>
#include <QPointF>
#include <vector>

// =================================================================
// ��������: ObstacleData �ṹ��
// ˵��: �����ڼ��غͱ༭�ؿ�ʱ���洢�����ϰ���ĺ������ݡ�
//      �����ݣ�λ�á����ͣ�������ͼ�α��֣�Obstacle�ࣩ���룬
//      ��һ�����õĽ�����ơ�
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
    // ��������: ��ȡ�ϰ������ݵĽӿ�
    // ˵��: ΪGameManager�ͳ�����Ⱦģ���ṩ�����ϰ�������ݡ�
    const std::vector<ObstacleData>& getObstacles() const;
    // =================================================================

    const QPixmap& getBackground() const;

private:
    std::vector<QPointF> pathPoints;
    std::vector<QPointF> towerBasePositions;
    QPixmap mapBackground;

    // =================================================================
    // ��������: �洢�ϰ������ݵ�����
    std::vector<ObstacleData> obstacles;
    // =================================================================
};
