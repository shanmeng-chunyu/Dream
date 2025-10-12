#include "GameMap.h"
// 实际项目中, 这里会包含P8编写的Json或Xml解析器头文件
// #include "LevelLoader.h"

GameMap::GameMap(QObject* parent) : QObject(parent) {}

// loadMap负责从数据源（通常是P8同学实现的LevelLoader）获取关卡信息
// 并填充到GameMap的成员变量中。
bool GameMap::loadMap(int levelId) {
    // 清空旧数据，为加载新关卡做准备
    pathPoints.clear();
    towerBasePositions.clear();
    obstacles.clear();

    // --- 实际加载逻辑 ---
    // 在真实项目中, 这里会调用P8的LevelLoader来解析JSON文件,
    // 并用解析出的数据填充下面的vector。
    // LevelData levelData = LevelLoader::load(levelId);
    // pathPoints = levelData.path;
    // towerBasePositions = levelData.bases;
    // obstacles = levelData.obstacles;
    // mapBackground.load(levelData.backgroundPath);

    // --- 以下为演示用例，模拟加载了一个包含障碍物的关卡 ---
    // 模拟加载第一关
    if (levelId == 1) {
        // 加载背景
        mapBackground.load(":/assets/images/backgrounds/第一关背景.png");

        // 加载路径点
        pathPoints = { {100, 100}, {500, 100}, {500, 400}, {800, 400} };

        // 加载塔基座
        towerBasePositions = { {300, 200}, {600, 250} };

        // =================================================================
        // 新增内容: 加载障碍物数据
        // 说明: 我们在这里手动添加了三个障碍物的数据，包括它们的位置和类型ID。
        //      在正式版本中，这些数据都将从关卡文件中动态读取。
        obstacles.push_back({QPointF(200, 50), 1}); // 在(200, 50)位置创建一个类型1的书本障碍物
        obstacles.push_back({QPointF(400, 300), 2}); // 在(400, 300)位置创建一个类型2的数据线障碍物
        obstacles.push_back({QPointF(700, 150), 3}); // 在(700, 150)位置创建一个类型3的文具障碍物
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
// 新增内容: getObstacles() 函数的实现
// 说明: 这个函数很简单, 就是返回内部存储的障碍物数据。
//      const&的返回类型确保了高效和安全，外部代码只能读取不能修改。
const std::vector<ObstacleData>& GameMap::getObstacles() const {
    return obstacles;
}
// =================================================================

const QPixmap& GameMap::getBackground() const {
    return mapBackground;
}
