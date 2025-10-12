#pragma once

#include <QObject>
#include <memory>
#include <vector>

// --- 前向声明 ---
class Tower;
class Enemy;
class Projectile;
class WaveManager;
class Player;
class GameMap;
class Obstacle; // <-- 新增前向声明

class GameManager : public QObject {
    Q_OBJECT

public:
    static GameManager* getInstance();
    void startGame(int levelId);
    void stopGame();
    void pauseGame();
    void resumeGame();

    public slots:
        void update();

private:
    explicit GameManager(QObject* parent = nullptr);
    ~GameManager();
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;
    void cleanup();

    static GameManager* instance;

    std::unique_ptr<Player> player;
    std::unique_ptr<GameMap> map;
    std::unique_ptr<WaveManager> waveManager;

    std::vector<std::unique_ptr<Tower>> towers;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Projectile>> projectiles;

    // =================================================================
    // 新增内容: 存储障碍物实例的容器
    // 说明: 尽管障碍物不在update()循环中更新，但由GameManager持有其所有权
    //      可以确保它们的生命周期被正确管理，在游戏结束时统一释放资源。
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    // =================================================================

    bool isPaused;
};
