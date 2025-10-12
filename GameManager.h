#pragma once

#include <QObject>
#include <memory>
#include <vector>

// --- ǰ������ ---
class Tower;
class Enemy;
class Projectile;
class WaveManager;
class Player;
class GameMap;
class Obstacle; // <-- ����ǰ������

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
    // ��������: �洢�ϰ���ʵ��������
    // ˵��: �����ϰ��ﲻ��update()ѭ���и��£�����GameManager����������Ȩ
    //      ����ȷ�����ǵ��������ڱ���ȷ��������Ϸ����ʱͳһ�ͷ���Դ��
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    // =================================================================

    bool isPaused;
};
