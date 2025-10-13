#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <vector>
#include <QPointF>
#include <QString>

// ���ڴ洢��JSON���ص��ϰ����ʼ����
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

    // ���úͻ�ȡ���˵��н�·����������꣩
    void setPath(const std::vector<QPointF>& path);
    const std::vector<QPointF>& getPath() const;

    // ���úͻ�ȡ�������ɷ��õ�λ�ã�������꣩
    void setTowerPositions(const std::vector<QPointF>& positions);
    const std::vector<QPointF>& getTowerPositions() const;

    // ���úͻ�ȡ�ϰ���ĳ�ʼ����
    void setObstacles(const std::vector<ObstacleData>& obstacles);
    const std::vector<ObstacleData>& getObstacles() const;

private:
    std::vector<QPointF> enemyPath;
    std::vector<QPointF> towerPositions;
    std::vector<ObstacleData> obstacleData;
};

#endif // GAMEMAP_H
