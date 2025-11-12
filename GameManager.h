#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <QSizeF>
#include <QGraphicsItem>

// 前向声明，避免在头文件中引入过多依赖
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
    // 单例模式
    static GameManager* instance();

    // 初始化
    void init(QGraphicsScene* scene);
    // 加载关卡
    void loadLevel(const QString& levelPath);
    // 开始游戏
    void startGame();
    // 响应窗口大小变化
    void setScreenSize(const QSizeF& size);

    // 建造防御塔
    void buildTower(const QString& type, const QPointF& relativePosition);

public slots:
    // 响应WaveManager的信号来生成敌人
    void onSpawnEnemy(const QString& type, const std::vector<QPointF>& absolutePath);
    // 响应Tower的信号来生成子弹
    void onNewBullet(Tower* tower, QGraphicsPixmapItem* target);
    // 响应Enemy信号
    void onEnemyReachedEnd(Enemy* enemy);
    void onEnemyDied(Enemy* enemy);
    // 响应Bullet信号
    void onBulletHitTarget(Bullet* bullet);
    // 响应Obstacle信号
    void onObstacleDestroyed(Obstacle* obstacle, int resourceValue);
    //相应用户请求升级或出售防御塔
    void onTowerUpgradeRequested(const QPointF& relativePosition);
    void onTowerSellRequested(const QPointF& relativePosition);
    void pauseGame();
    void resumeGame();

private slots:
    // 游戏主循环
    void updateGame();
    void onApplyEnemyControl(Enemy* enemy,double duration);
signals:
    void gameFinished(bool win,int finalStability, int enemiesKilled);

private:
    GameManager(QObject* parent = nullptr);
    ~GameManager() override;
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    void loadPrototypes();
    void cleanupEntities();
    void updateTowerTargets();
    void checkWinLossConditions();

    static GameManager* m_instance;

    QGraphicsScene* m_scene;
    QTimer* m_gameTimer;
    QSizeF m_screenSize;

    // 游戏模块
    Player* m_player;
    GameMap* m_gameMap;
    WaveManager* m_waveManager;

    // 实体管理
    QList<Enemy*> m_enemies;
    QList<Tower*> m_towers;
    QList<Bullet*> m_bullets;
    QList<Obstacle*> m_obstacles;

    // 待删除的实体列表，用于安全删除
    QList<QGraphicsItem*> m_entitiesToClean;

    // 从关卡文件加载的原型数据
    QMap<QString, QJsonObject> m_enemyPrototypes;
    QMap<QString, QJsonObject> m_towerPrototypes;

    bool m_gameIsOver;

    QSet<Enemy*> m_raged;
    QHash<Enemy*, int> m_healCd;

};

#endif // GAMEMANAGER_H
