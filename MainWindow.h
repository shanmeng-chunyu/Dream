#ifndef DREAM_MAINWINDOW_H
#define DREAM_MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVector>

#include "GameMap.h"

class QGraphicsPixmapItem;
class QGraphicsItem;
class QResizeEvent;
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

signals:
    void towerUpgradeRequested(const QPointF &relativePosition);
    void towerSellRequested(const QPointF &relativePosition);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onTowerBaseClicked(int baseIndex, const QPointF &scenePos);

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

    QGraphicsScene *m_scene;
    QGraphicsView *m_view;
    GameMap m_visualMap;
    QString m_currentLevelPath;
    QGraphicsPixmapItem *m_backgroundItem;
    QVector<QGraphicsItem *> m_pathItems;
    QVector<QRectF> m_pathTileRects;
    QVector<TowerBaseVisual> m_towerBases;
    QVector<QGraphicsPixmapItem *> m_obstacleItems;
    QVector<QRectF> m_obstacleRects;
    qreal m_baseRadius;
    QSizeF m_sceneDesignSize;
};

#endif
