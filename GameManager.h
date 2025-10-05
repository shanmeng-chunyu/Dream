#ifndef DREAM_GAMEMANAGER_H
#define DREAM_GAMEMANAGER_H

#include <QObject>
#include <QGraphicsScene>
#include <QTimer>
#include "WaveManager.h"
#include "Player.h"
#include "Tower.h"
#include "Enemy.h"
#include "Bullet.h"

class GameManager : public QObject {
    Q_OBJECT

    public:
    static GameManager *instance();

    void initialize(QGraphicsScene *scene);

    void addTower(Tower *tower);

    void removeTower(Tower *tower);

    void addEnemy(Enemy *enemy);

    void removeEnemy(Enemy *enemy);

    void addBullet(QGraphicsItem *bullet);

    void removeBullet(QGraphicsItem *bullet);

    Player *getPlayer() const;

private:
    explicit GameManager(QObject *parent = nullptr);

    ~GameManager() override;

    // ��ֹ����
    GameManager(const GameManager &) = delete;

    GameManager &operator=(const GameManager &) = delete;

    void gameLoop();

    static GameManager *m_instance;
    QGraphicsScene *m_scene;
    WaveManager *m_waveManager;
    Player *m_player;
    QTimer *m_gameTimer;
    // ��������������˵��б�
    // QList<Tower*> m_towers;
    // QList<Enemy*> m_enemies;
};

#endif //DREAM_GAMEMANAGER_H
