#include "GameMap.h"
// ʵ����Ŀ��, ��������P8��д��Json��Xml������ͷ�ļ�
// #include "LevelLoader.h"

GameMap::GameMap(QObject* parent) : QObject(parent) {}

// loadMap���������Դ��ͨ����P8ͬѧʵ�ֵ�LevelLoader����ȡ�ؿ���Ϣ
// ����䵽GameMap�ĳ�Ա�����С�
bool GameMap::loadMap(int levelId) {
    // ��վ����ݣ�Ϊ�����¹ؿ���׼��
    pathPoints.clear();
    towerBasePositions.clear();
    obstacles.clear();

    // --- ʵ�ʼ����߼� ---
    // ����ʵ��Ŀ��, ��������P8��LevelLoader������JSON�ļ�,
    // ���ý�������������������vector��
    // LevelData levelData = LevelLoader::load(levelId);
    // pathPoints = levelData.path;
    // towerBasePositions = levelData.bases;
    // obstacles = levelData.obstacles;
    // mapBackground.load(levelData.backgroundPath);

    // --- ����Ϊ��ʾ������ģ�������һ�������ϰ���Ĺؿ� ---
    // ģ����ص�һ��
    if (levelId == 1) {
        // ���ر���
        mapBackground.load(":/assets/images/backgrounds/��һ�ر���.png");

        // ����·����
        pathPoints = { {100, 100}, {500, 100}, {500, 400}, {800, 400} };

        // ����������
        towerBasePositions = { {300, 200}, {600, 250} };

        // =================================================================
        // ��������: �����ϰ�������
        // ˵��: �����������ֶ�����������ϰ�������ݣ��������ǵ�λ�ú�����ID��
        //      ����ʽ�汾�У���Щ���ݶ����ӹؿ��ļ��ж�̬��ȡ��
        obstacles.push_back({QPointF(200, 50), 1}); // ��(200, 50)λ�ô���һ������1���鱾�ϰ���
        obstacles.push_back({QPointF(400, 300), 2}); // ��(400, 300)λ�ô���һ������2���������ϰ���
        obstacles.push_back({QPointF(700, 150), 3}); // ��(700, 150)λ�ô���һ������3���ľ��ϰ���
        // =================================================================

        return true;
    }

    return false;
}

const std::vector<QPointF>& GameMap::getPathPoints() const {
    return pathPoints;
}

const std::vector<QPointF>& GameMap::getTowerBasePositions() const {
    return towerBasePositions;
}

// =================================================================
// ��������: getObstacles() ������ʵ��
// ˵��: ��������ܼ�, ���Ƿ����ڲ��洢���ϰ������ݡ�
//      const&�ķ�������ȷ���˸�Ч�Ͱ�ȫ���ⲿ����ֻ�ܶ�ȡ�����޸ġ�
const std::vector<ObstacleData>& GameMap::getObstacles() const {
    return obstacles;
}
// =================================================================

const QPixmap& GameMap::getBackground() const {
    return mapBackground;
}
