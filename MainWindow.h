#ifndef DREAM_MAINWINDOW_H
#define DREAM_MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVector>
#include <QDebug>
#include <QPointer>
#include <QPixmap>
#include <QPainter>
#include <QStringList>
#include <QJsonObject>
#include <QRectF>
#include <QShowEvent>

#include "GameMap.h"
#include "widget_post_game.h"

class QGraphicsPixmapItem;
class QGraphicsItem;
class QResizeEvent;
class QKeyEvent;
class TowerBaseItem;
class QPushButton;
class QLabel;
class QProgressBar;
class QTimer;
class Player;
class WaveManager;
class widget_ingame;
class widget_pause_menu;

struct TowerBaseVisual
{
    QPointF relativePosition;
    TowerBaseItem *graphicsItem = nullptr;
    QRectF cellRect;
    bool blockedByObstacle = false;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
    bool startLevelFromSource(const QString &candidatePath, bool showError = true);

signals:
    void towerUpgradeRequested(const QPointF &relativePosition);
    void towerSellRequested(const QPointF &relativePosition);
    void levelSelectionRequested();
    void mainMenuRequested();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void onTowerBaseClicked(int baseIndex, const QPointF &scenePos);
    void handleGameFinished(bool win, int stability, int killCount);
    void onHudPauseRequested();
    void onHudResumeRequested();
    void onHudSpeedUpRequested();
    void onHudSpeedNormalRequested();
    void onPlayerResourceChanged(int value);
    void onPlayerStabilityChanged(int value);
    void onWaveEnemySpawned();
    void onAllWavesCompleted();
    void onObstacleAreaCleared(const QRectF &relativeRect);
    void onHudMenuClicked();      // 响应 HUD 菜单按钮点击
    void onReturnToMainMenu();    // 响应暂停菜单的“返回主菜单”
    void onPauseMenuClosed();     // 响应暂停菜单关闭事件（用于清理）

private:
    void initializeScene();
    void generateBlurredBackground();
    QString resolveLevelPath(const QString &candidate) const;
    bool loadVisualLevel(const QString &levelPath);
    void populateSceneFromMap();
    void drawBackgroundLayer();
    void drawPathLayer();
    void drawObstacles();
    void createTowerBaseItems();
    void updateTowerBaseGeometry();
    void updateSingleBaseState(int index);
    void refreshAllBaseStates();
    void recalculateBaseRadius();
    void applySceneRectFromMap();
    QSizeF mapSceneSizeHint() const;
    bool isScenePointBlocked(const QPointF &scenePos) const;
    QPointF toAbsolutePosition(const QPointF &relative) const;
    QPoint sceneToGlobalPoint(const QPointF &scenePos) const;
    void showBuildMenu(int baseIndex, const QPoint &globalPos);
    void showUpgradeMenu(int baseIndex, const QPoint &globalPos);
    bool baseHasTower(int baseIndex) const;
    void synchronizeLogicScreenSize() const;
    void fitViewToScene();
    QSizeF defaultSceneSize() const;
    QString searchForFileUpwards(const QString &startDir, const QString &fileName, int depthLimit = 5) const;
    QString prepareRuntimeLevelFile(const QString &levelPath);
    QString absoluteAssetPath(const QString &path, const QString &projectRoot) const;
    QString locateProjectRootPath(const QString &levelPath) const;
    void showPostGameWidget(bool win, int stability, int killCount);
    void dismissPostGameWidget();
    bool loadLevelByIndex(int index, bool showError = true);
    void cycleLevel(int delta);
    void updateLevelSwitchStatus(int index);
    void ensureHudWidget();
    void destroyHudWidget();
    void positionHudWidget();
    void connectHudToSystems();
    void disconnectWaveSignals();
    void prepareWaveTrackingFromJson(const QJsonObject &levelRoot);
    void resetWaveTracking();
    void updateHudProgress();
    void locateGameLoopTimer();
    Player *resolvePlayer() const;
    WaveManager *resolveWaveManager() const;
    void applyGameSpeed(bool fastMode);
    QVector<double> buildAxisCoordinates(double offset, double spacing) const;
    double estimateGridOffset(bool horizontal, double spacing) const;
    bool rectIntersectsAny(const QVector<QRectF> &rects, const QRectF &candidate, qreal tolerance = 0.0) const;
    void updateBaseAvailability();
    void spawnBaseItem(int index);
    QRectF relativeRectToSceneRect(const QRectF &relativeRect) const;
    qreal cellIntersectionTolerance() const;
    void cacheHudSubControls();
    void updateHudStability(int value);

    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    GameMap m_visualMap;
    QString m_currentLevelPath;
    QGraphicsPixmapItem *m_backgroundItem;
    QVector<QGraphicsItem *> m_pathItems;
    QVector<QRectF> m_pathTileRects;
    QVector<TowerBaseVisual> m_towerBases;
    QVector<QRectF> m_obstacleRects;
    qreal m_baseRadius;
    QSizeF m_sceneDesignSize;
    QSizeF m_cellSize;
    QPointer<widget_post_game> m_postGameWidget;
    widget_pause_menu *m_pauseMenuWidget;
    QStringList m_levelSources;
    int m_currentLevelIndex;
    widget_ingame *m_hudWidget;
    QPointer<QLabel> m_hudProgressLabel;
    QPointer<QProgressBar> m_hudProgressBar;
    QPointer<QLabel> m_hudStabilityLabel;
    QPointer<QProgressBar> m_hudStabilityBar;
    QSizeF m_hudDesignSize;
    int m_initialStability;
    QVector<int> m_waveEnemyTotals;
    int m_totalEnemyCount;
    int m_spawnedEnemyCount;
    int m_currentWaveCounter;
    int m_spawnedInCurrentWave;
    QPointer<Player> m_cachedPlayer;
    QPointer<WaveManager> m_cachedWaveManager;
    QPointer<QTimer> m_gameLoopTimer;
    QMetaObject::Connection m_waveSpawnConnection;
    QMetaObject::Connection m_waveCompletionConnection;
    bool m_fastModeActive;
    QPixmap m_rawBg;
    QPixmap m_blurredBg;
    int m_blurRadius;
};

#endif
