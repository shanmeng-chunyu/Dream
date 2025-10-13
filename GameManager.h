#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <QSizeF>
#include <QGraphicsItem>

// ǰ��������������ͷ�ļ��������������
class QGraphicsScene;
class QTimer;
class Enemy;
class Tower;
class Bullet;
class Obstacle;
class Player;
class WaveManager;
class GameMap;

class GameManager : public QObject {
    Q_OBJECT

public:
    // ����ģʽ
    static GameManager* instance();

    // ��ʼ��
    void init(QGraphicsScene* scene);
    // ���عؿ�
    void loadLevel(const QString& levelPath);
    // ��ʼ��Ϸ
    void startGame();
    // ��Ӧ���ڴ�С�仯
    void setScreenSize(const QSizeF& size);

    // ���������
    void buildTower(const QString& type, const QPointF& relativePosition);

public slots:
    // ��ӦWaveManager���ź������ɵ���
    void onSpawnEnemy(const QString& type, const std::vector<QPointF>& absolutePath);
    // ��ӦTower���ź��������ӵ�
    void onNewBullet(Tower* tower, Enemy* target);
    // ��ӦEnemy�ź�
    void onEnemyReachedEnd(Enemy* enemy);
    void onEnemyDied(Enemy* enemy);
    // ��ӦBullet�ź�
    void onBulletHitTarget(Bullet* bullet);
    // ��ӦObstacle�ź�
    void onObstacleDestroyed(Obstacle* obstacle, int resourceValue);

private slots:
    // ��Ϸ��ѭ��
    void updateGame();

private:
    GameManager(QObject* parent = nullptr);
    ~GameManager() override;
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    void loadPrototypes(const QJsonObject& rootObj);
    void cleanupEntities();
    void updateTowerTargets();
    void checkWinLossConditions();

    static GameManager* m_instance;

    QGraphicsScene* m_scene;
    QTimer* m_gameTimer;
    QSizeF m_screenSize;

    // ��Ϸģ��
    Player* m_player;
    WaveManager* m_waveManager;
    GameMap* m_gameMap;

    // ʵ�����
    QList<Enemy*> m_enemies;
    QList<Tower*> m_towers;
    QList<Bullet*> m_bullets;
    QList<Obstacle*> m_obstacles;

    // ��ɾ����ʵ���б����ڰ�ȫɾ��
    QList<QGraphicsItem*> m_entitiesToClean;

    // �ӹؿ��ļ����ص�ԭ������
    QMap<QString, QJsonObject> m_enemyPrototypes;
    QMap<QString, QJsonObject> m_towerPrototypes;

    bool m_gameIsOver;
};

#endif // GAMEMANAGER_H
