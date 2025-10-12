// ... ����include ...
#include "GameManager.h"
#include "GameMap.h"
#include "Player.h"
#include "WaveManager.h"
#include "Tower.h"
#include "Enemy.h"
#include "Obstacle.h" // <-- ��������¼ӵ�ͷ�ļ�

// ... getInstance(), ����, �����Ⱥ������� ...

void GameManager::startGame(int levelId) {
    cleanup(); // ��ʼ����Ϸǰ������ɳ���

    // ��ʼ���������
    player = std::make_unique<Player>();
    map = std::make_unique<GameMap>();

    if (map->loadMap(levelId)) {
        // ... ����WaveManager, ������ҳ�ʼ״̬�ȴ��� ...

        // =================================================================
        // ��������: ���ݵ�ͼ���ݴ����ϰ���ʵ��
        // ������: P1 / P2
        // ˵��: �����������ݺͱ��ֵĹؼ�һ����
        //      1. ��map�����ȡ�����ϰ��������(ObstacleData)��
        //      2. ������Щ���ݡ�
        //      3. Ϊÿһ�����ݴ���һ��Obstacle��ͼ��ʵ����
        //      4. ��ʵ����ָ�����GameManager��obstacles�����н���ͳһ����
        //      5. (��Ҫ) ��ʵ����ӵ�QGraphicsScene�н�����Ⱦ (�ⲽ��P2�ڳ����������)��
        const auto& obstacleData = map->getObstacles();
        for (const auto& data : obstacleData) {
            auto obstacle = std::make_unique<Obstacle>(data.typeId);
            obstacle->setPos(data.position);
            // scene->addItem(obstacle.get()); // P2ͬѧ��Ҫ�����ʵ����ӵ�������
            obstacles.push_back(std::move(obstacle));
        }
        // =================================================================

    } else {
        // �����ͼ����ʧ�ܵ����
    }
}

void GameManager::cleanup() {
    // ... ���� towers, enemies, projectiles ...
    towers.clear();
    enemies.clear();
    projectiles.clear();

    // =================================================================
    // ��������: �����ϰ���
    // ˵��: ����Ϸ���������¿�ʼʱ, obstacles�����ᱻ��ա�
    //      ��������ʹ����std::unique_ptr, �������ʱ���Զ�ɾ������
    //      �������Obstacle���󣬰�ȫ���ͷ��ڴ档
    obstacles.clear();
    // =================================================================

    // ... reset player, map, waveManager ...
}

void GameManager::update() {
    // ��ѭ��...
    // ע�⣺�ϰ����Ǿ�̬��, ���Բ���Ҫ��updateѭ���е������ǵ��κη�����
    // ��Ҳ�����ǽ�������Ҫÿ֡���µ�GameObject�ֿ���Ƶ�ԭ�򣬿����������ܡ�
}

// ... ����������ʵ�� ...
