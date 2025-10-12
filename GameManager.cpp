// ... 其他include ...
#include "GameManager.h"
#include "GameMap.h"
#include "Player.h"
#include "WaveManager.h"
#include "Tower.h"
#include "Enemy.h"
#include "Obstacle.h" // <-- 必须包含新加的头文件

// ... getInstance(), 构造, 析构等函数不变 ...

void GameManager::startGame(int levelId) {
    cleanup(); // 开始新游戏前先清理旧场景

    // 初始化核心组件
    player = std::make_unique<Player>();
    map = std::make_unique<GameMap>();

    if (map->loadMap(levelId)) {
        // ... 加载WaveManager, 设置玩家初始状态等代码 ...

        // =================================================================
        // 新增内容: 根据地图数据创建障碍物实例
        // 负责人: P1 / P2
        // 说明: 这是连接数据和表现的关键一步。
        //      1. 从map对象获取所有障碍物的数据(ObstacleData)。
        //      2. 遍历这些数据。
        //      3. 为每一条数据创建一个Obstacle的图形实例。
        //      4. 将实例的指针存入GameManager的obstacles容器中进行统一管理。
        //      5. (重要) 将实例添加到QGraphicsScene中进行渲染 (这步由P2在场景类中完成)。
        const auto& obstacleData = map->getObstacles();
        for (const auto& data : obstacleData) {
            auto obstacle = std::make_unique<Obstacle>(data.typeId);
            obstacle->setPos(data.position);
            // scene->addItem(obstacle.get()); // P2同学需要将这个实例添加到场景中
            obstacles.push_back(std::move(obstacle));
        }
        // =================================================================

    } else {
        // 处理地图加载失败的情况
    }
}

void GameManager::cleanup() {
    // ... 清理 towers, enemies, projectiles ...
    towers.clear();
    enemies.clear();
    projectiles.clear();

    // =================================================================
    // 新增内容: 清理障碍物
    // 说明: 当游戏结束或重新开始时, obstacles容器会被清空。
    //      由于我们使用了std::unique_ptr, 容器清空时会自动删除所有
    //      它管理的Obstacle对象，安全地释放内存。
    obstacles.clear();
    // =================================================================

    // ... reset player, map, waveManager ...
}

void GameManager::update() {
    // 主循环...
    // 注意：障碍物是静态的, 所以不需要在update循环中调用它们的任何方法。
    // 这也是我们将它与需要每帧更新的GameObject分开设计的原因，可以提升性能。
}

// ... 其他函数的实现 ...
