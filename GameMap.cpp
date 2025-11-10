#include "GameMap.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointF>
#include <QDir>
#include <QFileInfo>
#include <QStringDecoder>
#include <limits>
#include <cmath>
#include <algorithm>

namespace
{
struct StageAssetRow
{
    const char *type;
    const char *stage1;
    const char *stage2;
    const char *stage3;
};

template <size_t N>
QString lookupStageAsset(const StageAssetRow (&table)[N], const QString &type, int stageIndex)
{
    if (stageIndex < 1 || stageIndex > 3 || type.isEmpty())
    {
        return {};
    }

    for (const StageAssetRow &row : table)
    {
        if (QString::compare(type, QString::fromUtf8(row.type), Qt::CaseInsensitive) == 0)
        {
            const char *candidate = nullptr;
            switch (stageIndex)
            {
            case 1:
                candidate = row.stage1;
                break;
            case 2:
                candidate = row.stage2;
                break;
            case 3:
                candidate = row.stage3;
                break;
            default:
                break;
            }

            if (candidate && *candidate)
            {
                return QString::fromUtf8(candidate);
            }
            break;
        }
    }

    return {};
}

const StageAssetRow kTowerSpriteTable[] = {
    {"KnowledgeTree", u8"美术素材（透明）/防御塔/第一关/知识古树.png", "", ""},
    {"InspirationBulb", u8"美术素材（透明）/防御塔/第一关/灵感灯泡.png", "", ""},
    {"LiveCoffee", u8"美术素材（透明）/防御塔/第一关/续命咖啡.png", "", ""},
    {"FishingCatPillow", u8"美术素材（透明）/防御塔/第一关/摸鱼猫抱枕.png", "", ""},
    {"WarmMemories", "", u8"美术素材（透明）/防御塔/第二关/温暖的记忆.png", ""},
    {"NightRadio", "", u8"美术素材（透明）/防御塔/第二关/深夜电台.png", ""},
    {"PettingCatTime", "", u8"美术素材（透明）/防御塔/第二关/撸猫时间.png", ""},
    {"Companionship", "", u8"美术素材（透明）/防御塔/第二关/朋友陪伴.png", ""}
};

const StageAssetRow kTowerBulletTable[] = {
    {"KnowledgeTree", u8"美术素材（透明）/防御塔子弹/第一关/知识古树子弹.png", "", ""},
    {"InspirationBulb", u8"美术素材（透明）/防御塔子弹/第一关/灵感灯泡子弹.png", "", ""},
    {"LiveCoffee", u8"美术素材（透明）/防御塔子弹/第一关/续命咖啡（光环特效）.png", "", ""},
    {"NightRadio", "", u8"美术素材（透明）/防御塔子弹/第二关/深夜电台子弹.png", ""},
    {"Companionship", "", u8"美术素材（透明）/防御塔子弹/第二关/朋友陪伴光环.png", ""}
};

const StageAssetRow kEnemySpriteTable[] = {
    {"bug", u8"美术素材（透明）/敌人/第一关/bug怪物.png", "", ""},
    {"ddl", u8"美术素材（透明）/敌人/第一关/ddl怪物.png", "", ""},
    {"gpa", u8"美术素材（透明）/敌人/第一关/gpa怪物.png", "", ""},
    {"pre", u8"美术素材（透明）/敌人/第一关/pre怪物.png", "", ""},
    {"thesis", u8"美术素材（透明）/敌人/第一关/论文boss.png", "", ""},
    {"coldwords", "", u8"美术素材（透明）/敌人/第二关/冰冷的言语.png", ""},
    {"loneliness", "", u8"美术素材（透明）/敌人/第二关/孤独怪物.png", ""},
    {"lonelyness", "", u8"美术素材（透明）/敌人/第二关/孤独怪物.png", ""},
    {"regret", "", u8"美术素材（透明）/敌人/第二关/回忆怪物.png", ""},
    {"recall", "", u8"美术素材（透明）/敌人/第二关/回忆怪物.png", ""},
    {"tears", "", u8"美术素材（透明）/敌人/第二关/泪水怪物.png", ""},
    {"past", "", u8"美术素材（透明）/敌人/第二关/昔日幻影boss.png", ""},
    {"thephantomofthepast", "", u8"美术素材（透明）/敌人/第二关/昔日幻影boss.png", ""},
    {"nightmare", "", "", u8"美术素材（透明）/敌人/第三关/梦魇boss.png"}
};

const StageAssetRow kObstacleSpriteTable[] = {
    {"BookStack", u8"美术素材（透明）/地图方块/第一关/书本障碍方块.png", "", ""},
    {"Book", u8"美术素材（透明）/地图方块/第一关/书本障碍方块.png", "", ""},
    {"Scratchpaper", u8"美术素材（透明）/地图方块/第一关/草稿障碍方块.png", "", ""},
    {"Cable", u8"美术素材（透明）/地图方块/第一关/数据线障碍方块.png", "", ""},
    {"Stationery", u8"美术素材（透明）/地图方块/第一关/文具障碍方块.png", "", ""},
    {"MemoryBox", "", u8"美术素材（透明）/地图方块/第二关/尘封记忆障碍物.png", ""},
    {"WitheredFlowers", "", u8"美术素材（透明）/地图方块/第二关/凋落的花障碍物.png", ""},
    {"BrokenRing", "", u8"美术素材（透明）/地图方块/第二关/破碎戒指障碍物.png", ""},
    {"BrokenFreindship", "", u8"美术素材（透明）/地图方块/第二关/破碎友谊障碍物.png", ""},
    {"BrokenFriendship", "", u8"美术素材（透明）/地图方块/第二关/破碎友谊障碍物.png", ""}
};

}

GameMap::GameMap()
    : pathWidthRatio(0.08),
      pathWidthExplicit(false),
      stageIndex(1),
      gridSpacingX(0.08),
      gridSpacingY(0.08)
{
}

bool GameMap::loadFromFile(const QString &filePath)
{

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open level file:" << filePath;
        return false;
    }

    QJsonParseError parseError;
    const QByteArray originalData = file.readAll();
    const QJsonDocument doc = GameMap::parseJsonWithEncodingFallback(filePath, originalData, &parseError);
    if (!doc.isObject())
    {
        qWarning() << "Invalid level json:" << filePath << parseError.errorString();
        return false;
    }

    QFileInfo levelInfo(filePath);
    levelFilePath = levelInfo.absoluteFilePath();
    levelDirectory = levelInfo.absolutePath();

    QJsonObject root = doc.object();
    levelName = root.value("level_name").toString(filePath);
    stageIndex = detectStageFromName(levelName);
    if (stageIndex <= 0)
    {
        stageIndex = 1;
    }

    const QString embeddedRoot = root.value(QStringLiteral("_project_root")).toString();
    if (!embeddedRoot.isEmpty())
    {
        projectRootPath = embeddedRoot;
    }
    if (projectRootPath.isEmpty())
    {
        projectRootPath = locateProjectRootPath(levelDirectory);
    }

    const QJsonObject mapObj = root.value("map").toObject();
    backgroundPixmapPath = normalizeAssetPath(mapObj.value("background").toString(),
                                              {stageSpecificBackgroundPath(stageIndex)});
    if (mapObj.contains("path_width"))
    {
        pathWidthRatio = mapObj.value("path_width").toDouble(pathWidthRatio);
        pathWidthExplicit = true;
    }
    else
    {
        pathWidthExplicit = false;
        pathWidthRatio = fallbackPathWidthRatio(stageIndex);
    }
    const QString pathTextureCandidate = mapObj.value("path_texture").toString();
    pathTexturePixmapPath = normalizeAssetPath(pathTextureCandidate, {stageSpecificPathTexture(stageIndex)});
    const QString towerBaseCandidate = mapObj.value("tower_base").toString();
    towerBasePixmapPath = normalizeAssetPath(towerBaseCandidate, {defaultTowerBaseFrame()});

    std::vector<QPointF> path;
    const QJsonArray pathArray = mapObj.value("path").toArray();
    path.reserve(pathArray.size());
    for (const QJsonValue &pointValue : pathArray)
    {
        const QJsonObject pointObj = pointValue.toObject();
        path.emplace_back(pointObj.value("x").toDouble(), pointObj.value("y").toDouble());
    }
    setPath(path);

    std::vector<QPointF> towerPos;
    const QJsonArray towerArray = mapObj.value("tower_positions").toArray();
    towerPos.reserve(towerArray.size());
    for (const QJsonValue &pointValue : towerArray)
    {
        const QJsonObject pointObj = pointValue.toObject();
        towerPos.emplace_back(pointObj.value("x").toDouble(), pointObj.value("y").toDouble());
    }
    setTowerPositions(towerPos);

    std::vector<ObstacleData> obstacles;
    const QJsonArray obstacleArray = mapObj.value("obstacles").toArray();
    obstacles.reserve(obstacleArray.size());

    for (int idx = 0; idx < obstacleArray.size(); ++idx)
    {
        const QJsonValue &obsValue = obstacleArray[idx];
        const QJsonObject obsObj = obsValue.toObject();
        const QJsonObject posObj = obsObj.value("position").toObject();

        ObstacleData data;
        data.type = obsObj.value("type").toString();
        const QString obstacleSpriteFallback = fallbackObstaclePixmap(data.type, stageIndex);
        data.pixmapPath = normalizeAssetPath(obsObj.value("pixmap").toString(), {obstacleSpriteFallback});


        double posX = posObj.value("x").toDouble();
        double posY = posObj.value("y").toDouble();
        data.relativePosition = QPointF(posX, posY);

        data.health = obsObj.value("health").toInt();
        data.resourceValue = obsObj.value("resource").toInt();

        obstacles.push_back(data);
    }

    setObstacles(obstacles);

    towerPrototypes.clear();
    const QJsonArray towerProtoArray = root.value("available_towers").toArray();
    towerPrototypes.reserve(towerProtoArray.size());
    for (const QJsonValue &towerValue : towerProtoArray)
    {
        const QJsonObject towerObj = towerValue.toObject();
        TowerPrototypeInfo proto;
        proto.type = towerObj.value("type").toString();
        proto.name = towerObj.value("name").toString(proto.type);
        const QString towerSpriteFallback = fallbackTowerPixmap(proto.type, stageIndex, false);
        const QString bulletSpriteFallback = fallbackTowerPixmap(proto.type, stageIndex, true);
        proto.pixmapPath = normalizeAssetPath(towerObj.value("pixmap").toString(), {towerSpriteFallback});
        proto.bulletPixmapPath = normalizeAssetPath(towerObj.value("bullet_pixmap").toString(), {bulletSpriteFallback});
        proto.cost = towerObj.value("cost").toInt();
        if (!proto.type.isEmpty())
        {
            towerPrototypes.push_back(proto);
        }
    }

    return true;
}

void GameMap::setPath(const std::vector<QPointF> &path)
{
    enemyPath = path;
    updateGridSpacing();
}

const std::vector<QPointF> &GameMap::getPath() const
{
    return enemyPath;
}

void GameMap::setTowerPositions(const std::vector<QPointF> &positions)
{
    towerPositions = positions;
    updateGridSpacing();
}

const std::vector<QPointF> &GameMap::getTowerPositions() const
{
    return towerPositions;
}

void GameMap::setObstacles(const std::vector<ObstacleData> &obstacles)
{
    obstacleData = obstacles;
}

const std::vector<ObstacleData> &GameMap::getObstacles() const
{
    return obstacleData;
}

const QString &GameMap::getBackgroundPixmap() const
{
    return backgroundPixmapPath;
}

double GameMap::getPathWidthRatio() const
{
    return pathWidthRatio;
}

const std::vector<TowerPrototypeInfo> &GameMap::getAvailableTowers() const
{
    return towerPrototypes;
}

const QString &GameMap::getLevelName() const
{
    return levelName;
}

const QString &GameMap::getLevelFilePath() const
{
    return levelFilePath;
}

const QString &GameMap::getPathTexturePixmap() const
{
    return pathTexturePixmapPath;
}

const QString &GameMap::getTowerBasePixmap() const
{
    return towerBasePixmapPath;
}

int GameMap::getStageIndex() const
{
    return stageIndex;
}

double GameMap::getGridSpacingX() const
{
    return gridSpacingX;
}

double GameMap::getGridSpacingY() const
{
    return gridSpacingY;
}

int GameMap::guessStageFromName(const QString &name)
{
    return detectStageFromName(name);
}

QString GameMap::fallbackTowerPixmap(const QString &type, int stageIndex, bool bullet)
{
    return bullet ? lookupStageAsset(kTowerBulletTable, type, stageIndex)
                  : lookupStageAsset(kTowerSpriteTable, type, stageIndex);
}

QString GameMap::fallbackEnemyPixmap(const QString &type, int stageIndex)
{
    return lookupStageAsset(kEnemySpriteTable, type, stageIndex);
}

QString GameMap::fallbackObstaclePixmap(const QString &type, int stageIndex)
{
    return lookupStageAsset(kObstacleSpriteTable, type, stageIndex);
}

QJsonDocument GameMap::parseJsonWithEncodingFallback(const QString &filePath, const QByteArray &data, QJsonParseError *error)
{
    QJsonParseError localError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &localError);
    if (localError.error == QJsonParseError::NoError)
    {
        if (error)
        {
            *error = localError;
        }
        return doc;
    }

    const QString errorDescription = localError.errorString().toLower();
    if (!errorDescription.contains(QStringLiteral("utf")))
    {
        if (error)
        {
            *error = localError;
        }
        return doc;
    }

    const QString localizedText = QString::fromLocal8Bit(data);
    if (!localizedText.isEmpty())
    {
        const QByteArray utf8Data = localizedText.toUtf8();
        QJsonParseError retryError;
        QJsonDocument retryDoc = QJsonDocument::fromJson(utf8Data, &retryError);
        if (retryError.error == QJsonParseError::NoError)
        {
            if (error)
            {
                *error = retryError;
            }
            return retryDoc;
        }
    }


    const char *const encodingNames[] = {"GB18030", "GBK", "GB2312"};

    for (const char *encodingName : encodingNames)
    {
        QStringDecoder decoder(encodingName);
        if (!decoder.isValid())
        {
            continue;
        }

        QString decoded = decoder.decode(data);
        if (decoder.hasError() || decoded.isEmpty())
        {
            continue;
        }

        const QByteArray utf8Data = decoded.toUtf8();
        QJsonParseError retryError;
        QJsonDocument retryDoc = QJsonDocument::fromJson(utf8Data, &retryError);
        if (retryError.error == QJsonParseError::NoError)
        {
            if (error)
            {
                *error = retryError;
            }
            return retryDoc;
        }
    }

    if (error)
    {
        *error = localError;
    }
    qWarning() << "Failed to decode level file with UTF-8 or system locale:" << filePath << localError.errorString();
    return doc;
}

QString GameMap::normalizeAssetPath(const QString &rawPath, const QStringList &fallbackCandidates) const
{
    QStringList candidates;
    if (!rawPath.isEmpty())
    {
        candidates << rawPath;
    }
    candidates << fallbackCandidates;

    for (const QString &candidate : candidates)
    {
        if (candidate.isEmpty())
        {
            continue;
        }

        if (candidate.startsWith(":/"))
        {
            QFile resourceFile(candidate);
            if (resourceFile.exists())
            {
                return candidate;
            }
            continue;
        }

        QFileInfo info(candidate);
        if (info.isRelative() && !levelDirectory.isEmpty())
        {
            QFileInfo relativeInfo(QDir(levelDirectory), candidate);
            if (relativeInfo.exists())
            {
                return relativeInfo.absoluteFilePath();
            }
        }

        if (info.isRelative() && !projectRootPath.isEmpty())
        {
            QFileInfo projectInfo(QDir(projectRootPath), candidate);
            if (projectInfo.exists())
            {
                return projectInfo.absoluteFilePath();
            }
        }

        if (info.exists())
        {
            return info.absoluteFilePath();
        }
    }

    return rawPath;
}

QString GameMap::fallbackBackgroundPixmap(int stageIndex)
{
    return stageSpecificBackgroundPath(stageIndex);
}

QString GameMap::fallbackPathTexture(int stageIndex)
{
    return stageSpecificPathTexture(stageIndex);
}

QString GameMap::fallbackTowerBaseFrame()
{
    return defaultTowerBaseFrame();
}

QString GameMap::stageSpecificBackgroundPath(int stageIndex)
{
    switch (stageIndex)
    {
    case 1:
        return QStringLiteral("美术素材（透明）/画面背景/第一关背景.png");
    case 2:
        return QStringLiteral("美术素材（透明）/画面背景/第二关背景.png");
    case 3:
        return QStringLiteral("美术素材（透明）/画面背景/第三关背景.png");
    default:
        return {};
    }
}

QString GameMap::stageSpecificPathTexture(int stageIndex)
{
    switch (stageIndex)
    {
    case 1:
        return QStringLiteral("美术素材（透明）/地图方块/第一关/第一关道路方块.png");
    case 2:
        return QStringLiteral("美术素材（透明）/地图方块/第二关/第二关道路方块.png");
    case 3:
        return QStringLiteral("美术素材（透明）/地图方块/第三关/第三关道路方块.png");
    default:
        return {};
    }
}

QString GameMap::defaultTowerBaseFrame()
{
    return QStringLiteral("美术素材（透明）/框/防御塔选择框.png");
}

double GameMap::fallbackPathWidthRatio(int stageIndex)
{
    switch (stageIndex)
    {
    case 1:
        return 0.085;
    case 2:
        return 0.08;
    case 3:
        return 0.08;
    default:
        return 0.06;
    }
}

int GameMap::detectStageFromName(const QString &name)
{
    if (name.contains(QStringLiteral("第一关")))
    {
        return 1;
    }
    if (name.contains(QStringLiteral("第二关")))
    {
        return 2;
    }
    if (name.contains(QStringLiteral("第三关")))
    {
        return 3;
    }
    return 0;
}

void GameMap::updateGridSpacing()
{
    auto combineSpacing = [&](bool horizontal) -> double
    {
        double spacing = extractSpacing(enemyPath, horizontal);
        if (spacing <= 0.0)
        {
            spacing = extractSpacing(towerPositions, horizontal);
        }
        if (spacing <= 0.0)
        {
            spacing = 0.08;
        }
        return spacing;
    };

    gridSpacingX = combineSpacing(true);
    gridSpacingY = combineSpacing(false);
}

double GameMap::extractSpacing(const std::vector<QPointF> &points, bool horizontal) const
{
    if (points.size() < 2)
    {
        return 0.0;
    }

    std::vector<double> coordinates;
    coordinates.reserve(points.size());
    for (const QPointF &pt : points)
    {
        coordinates.push_back(horizontal ? pt.x() : pt.y());
    }
    std::sort(coordinates.begin(), coordinates.end());

    double best = std::numeric_limits<double>::max();
    const double tolerance = 1e-4;
    double previous = coordinates.front();
    for (size_t i = 1; i < coordinates.size(); ++i)
    {
        const double current = coordinates[i];
        const double delta = current - previous;
        if (delta > tolerance && delta < best)
        {
            best = delta;
        }
        previous = current;
    }

    if (best == std::numeric_limits<double>::max())
    {
        return 0.0;
    }
    return best;
}

QString GameMap::locateProjectRootPath(const QString &startDir)
{
    if (startDir.isEmpty())
    {
        return {};
    }

    QDir dir(startDir);
    for (int depth = 0; depth < 8 && dir.exists(); ++depth)
    {
        if (dir.exists(QStringLiteral("美术素材（透明）")))
        {
            return dir.absolutePath();
        }
        if (!dir.cdUp())
        {
            break;
        }
    }
    return startDir;
}
