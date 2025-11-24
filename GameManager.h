#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QString>
#include <QJsonObject>
#include <QSizeF>
#include <QGraphicsItem>
#include <QHash>

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
class QSoundEffect;

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
    void clearGameScene();

    const QList<Tower*>& getTowers() const { return m_towers; }

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
    const QList<Enemy*>& getEnemies() const { return m_enemies; }
    void onEnemyDeathAnimationFinished(Enemy* enemy); // 死亡动画播放完毕后触发的槽

private slots:
    // 游戏主循环
    void updateGame();
    void onApplyEnemyControl(QGraphicsPixmapItem* enemy,double duration);
    void onBulletHitEnemy(Bullet* bullet, Enemy* enemy);
    void onBulletHitObstacle(Bullet* bullet, Obstacle* obstacle);
signals:
    void gameFinished(bool win,int finalStability, int enemiesKilled);
    void obstacleCleared(const QRectF &relativeRect);

private:
    GameManager(QObject* parent = nullptr);
    ~GameManager() override;
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    void loadPrototypes();
    void cleanupEntities();
    void updateTowerTargets();
    void checkWinLossConditions();
    QSoundEffect* createSound(const QString& fileName, double volume);
    void preloadAllSounds();
    void playSound(QSoundEffect* sound);

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
    QSoundEffect* m_hitSoundAOE = nullptr;
    QSoundEffect* m_victorySound = nullptr;
    QSoundEffect* m_defeatSound = nullptr;
    QSoundEffect* m_upgradeSound = nullptr;
    QSoundEffect* m_nightmareSpawnSound = nullptr;
    QSoundEffect* m_buildSound = nullptr;
    QSoundEffect* m_sellSound = nullptr;
    QSoundEffect* m_bulbatkSound = nullptr;
    QSoundEffect* m_radioatkSound = nullptr;

    // 待删除的实体列表，用于安全删除
    QList<QGraphicsItem*> m_entitiesToClean;

    // 从关卡文件加载的原型数据
    QMap<QString, QJsonObject> m_enemyPrototypes;
    QMap<QString, QJsonObject> m_towerPrototypes;

    bool m_gameIsOver;
    Enemy* spawnByTypeWithPath(const QString& type, const std::vector<QPointF>& absPath,double scale = 1.0);


    void destroyAllTowers(bool withEffects = true);

    QSet<Enemy*> m_raged;
    QHash<Enemy*, int> m_healCd;
    QSet<QGraphicsItem*> m_activeDeathAnimations; // 跟踪正在播放的死亡动画，以便清理
};

#endif // GAMEMANAGER_H
