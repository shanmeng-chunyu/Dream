#ifndef DREAM_MAINWINDOW_H
#define DREAM_MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVector>
#include <QDebug>
#include <QPointer>
#include <QStringList>

#include "GameMap.h"
#include "widget_post_game.h"

class QGraphicsPixmapItem;
class QGraphicsItem;
class QResizeEvent;
class QKeyEvent;
class TowerBaseItem;

struct TowerBaseVisual
{
    QPointF relativePosition;
    TowerBaseItem *graphicsItem = nullptr;
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

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onTowerBaseClicked(int baseIndex, const QPointF &scenePos);
    void handleGameFinished(bool win, int stability, int killCount);

private:
    void initializeScene();
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
    QPointer<widget_post_game> m_postGameWidget;
    QStringList m_levelSources;
    int m_currentLevelIndex;
};

#endif
