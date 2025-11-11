#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <vector>
#include <QPointF>
#include <QString>
#include <QStringList>
#include <QJsonDocument>

struct ObstacleData
{
    QString type;
    QString pixmapPath;
    QPointF relativePosition;
    int health;
    int resourceValue;
};

struct TowerPrototypeInfo
{
    QString type;
    QString name;
    QString pixmapPath;
    QString bulletPixmapPath;
    int cost;
};

class GameMap
{
public:
    GameMap();

    bool loadFromFile(const QString &filePath);

    void setPath(const std::vector<QPointF> &path);
    const std::vector<QPointF> &getPath() const;

    void setTowerPositions(const std::vector<QPointF> &positions);
    const std::vector<QPointF> &getTowerPositions() const;

    void setObstacles(const std::vector<ObstacleData> &obstacles);
    const std::vector<ObstacleData> &getObstacles() const;

    const QString &getBackgroundPixmap() const;
    double getPathWidthRatio() const;
    const std::vector<TowerPrototypeInfo> &getAvailableTowers() const;
    const QString &getLevelName() const;
    const QString &getLevelFilePath() const;
    const QString &getPathTexturePixmap() const;
    const QString &getTowerBasePixmap() const;
    int getStageIndex() const;
    double getGridSpacingX() const;
    double getGridSpacingY() const;

    static int guessStageFromName(const QString &name);
    static QString fallbackTowerPixmap(const QString &type, int stageIndex, bool bullet = false);
    static QString fallbackEnemyPixmap(const QString &type, int stageIndex);
    static QString fallbackObstaclePixmap(const QString &type, int stageIndex);
    static QJsonDocument parseJsonWithEncodingFallback(const QString &filePath, const QByteArray &data, QJsonParseError *error = nullptr);
    static QString locateProjectRootPath(const QString &startDir);
    static QString fallbackBackgroundPixmap(int stageIndex);
    static QString fallbackPathTexture(int stageIndex);
    static QString fallbackTowerBaseFrame();
    static double fallbackPathWidthRatio(int stageIndex);

private:
    QString normalizeAssetPath(const QString &rawPath, const QStringList &fallbackCandidates = {}) const;
    void updateGridSpacing();
    double extractSpacing(const std::vector<QPointF> &points, bool horizontal) const;
    static QString stageSpecificBackgroundPath(int stageIndex);
    static QString stageSpecificPathTexture(int stageIndex);
    static QString defaultTowerBaseFrame();
    static int detectStageFromName(const QString &name);
    static double computeHorizontalCenterOffset(const std::vector<QPointF> &points);
    static void applyHorizontalOffset(std::vector<QPointF> &points, double offset);
    static void applyHorizontalOffset(std::vector<ObstacleData> &obstacles, double offset);

    std::vector<QPointF> enemyPath;
    std::vector<QPointF> towerPositions;
    std::vector<ObstacleData> obstacleData;
    std::vector<TowerPrototypeInfo> towerPrototypes;
    QString backgroundPixmapPath;
    QString levelName;
    QString levelFilePath;
    QString levelDirectory;
    QString pathTexturePixmapPath;
    QString towerBasePixmapPath;
    QString projectRootPath;
    double pathWidthRatio;
    bool pathWidthExplicit;
    int stageIndex;
    double gridSpacingX;
    double gridSpacingY;
};

#endif
