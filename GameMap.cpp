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

    // --- 【已修改】 使用 QRC 路径 ---
    const StageAssetRow kTowerSpriteTable[] = {
        {"KnowledgeTree",     u8":/towers/resources/towers/level1/KnowledgeTree.png",    "", u8":/towers/resources/towers/level1/KnowledgeTree.png"},
        {"InspirationBulb",   u8":/towers/resources/towers/level1/InspirationBulb.png",  "", u8":/towers/resources/towers/level1/InspirationBulb.png"},
        {"LiveCoffee",        u8":/towers/resources/towers/level1/LiveCoffee.png",       "", u8":/towers/resources/towers/level1/LiveCoffee.png"},
        {"FishingCatPillow",  u8":/towers/resources/towers/level1/FishingCatPillow.png", "", u8":/towers/resources/towers/level1/FishingCatPillow.png"},
        {"WarmMemories",      "", u8":/towers/resources/towers/level2/WarmMemories.png",     u8":/towers/resources/towers/level2/WarmMemories.png"},
        {"NightRadio",        "", u8":/towers/resources/towers/level2/NightRadio.png",       u8":/towers/resources/towers/level2/NightRadio.png"},
        {"PettingCatTime",    "", u8":/towers/resources/towers/level2/PettingCatTime.png",  u8":/towers/resources/towers/level2/PettingCatTime.png"},
        {"Companionship",     "", u8":/towers/resources/towers/level2/Companionship.png",    u8":/towers/resources/towers/level2/Companionship.png"}};

    // --- 【已修改】 使用 QRC 路径 ---
    const StageAssetRow kTowerBulletTable[] = {
        {"KnowledgeTree",     u8":/bullet/resources/bullet/level1/KnowledgeTree.png",    "", u8":/bullet/resources/bullet/level1/KnowledgeTree.png"},
        {"InspirationBulb",   u8":/bullet/resources/bullet/level1/InspirationBulb.png",  "", u8":/bullet/resources/bullet/level1/InspirationBulb.png"},
        {"LiveCoffee",        u8":/bullet/resources/bullet/level1/LiveCoffee.png",       "", u8":/bullet/resources/bullet/level1/LiveCoffee.png"}, // 咖啡的光环
        {"NightRadio",        "", u8":/bullet/resources/bullet/level2/NightRadio.png",       u8":/bullet/resources/bullet/level2/NightRadio.png"},
        {"Companionship",     "", u8":/bullet/resources/bullet/level2/Companionship.png",    u8":/bullet/resources/bullet/level2/Companionship.png"}}; // 朋友的光环

    // --- 【已修改】 使用 QRC 路径 ---
    const StageAssetRow kEnemySpriteTable[] = {
        {"bug", u8":/enemies/resources/enemies/level1/bug.png", "", ""},
        {"ddl", u8":/enemies/resources/enemies/level1/ddl.png", "", ""},
        {"gpa", u8":/enemies/resources/enemies/level1/gpa.png", "", ""},
        {"pre", u8":/enemies/resources/enemies/level1/pre.png", "", ""},
        {"thesis", u8":/enemies/resources/enemies/level1/thesis.png", "", ""},
        {"coldwords", "", u8":/enemies/resources/enemies/level2/coldwords.png", ""},
        {"loneliness", "", u8":/enemies/resources/enemies/level2/lonelyness.png", ""},
        {"lonelyness", "", u8":/enemies/resources/enemies/level2/lonelyness.png", ""}, // 别名
        {"regret", "", u8":/enemies/resources/enemies/level2/recall.png", ""},
        {"recall", "", u8":/enemies/resources/enemies/level2/recall.png", ""}, // 别名
        {"tears", "", u8":/enemies/resources/enemies/level2/tears.png", ""},
        {"past", "", u8":/enemies/resources/enemies/level2/thephantomofthepast.png", ""},
        {"thephantomofthepast", "", u8":/enemies/resources/enemies/level2/thephantomofthepast.png", ""}, // 别名
        {"nightmare", "", "", u8":/enemies/resources/enemies/level3/nightmare.png"}};

    // --- 【已修改】 使用 QRC 路径 ---
    const StageAssetRow kObstacleSpriteTable[] = {
        {"BookStack", u8":/map_items/resources/map_items/first/book.png", "", ""},
        {"Book", u8":/map_items/resources/map_items/first/book.png", "", ""},
        {"Scratchpaper", u8":/map_items/resources/map_items/first/scratchpaper.png", "", ""},
        {"Cable", u8":/map_items/resources/map_items/first/cable.png", "", ""},
        {"Stationery", u8":/map_items/resources/map_items/first/stationery.png", "", ""},
        {"MemoryBox", "", u8":/map_items/resources/map_items/second/memory_box.png", ""},
        {"WitheredFlowers", "", u8":/map_items/resources/map_items/second/withered_flowers.png", ""},
        {"BrokenRing", "", u8":/map_items/resources/map_items/second/broken_ring.png", ""},
        {"BrokenFreindship", "", u8":/map_items/resources/map_items/second/broken_friendship.png", ""},
        {"BrokenFriendship", "", u8":/map_items/resources/map_items/second/broken_friendship.png", ""}};

} // (匿名命名空间结束)

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
        // 【注意】：我们保留这个函数，但由于所有路径都改成了QRC，它现在不那么重要了
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

    if (stageIndex >= 2)
    {
        const double offset = computeHorizontalCenterOffset(path);
        if (std::abs(offset) > 1e-6)
        {
            applyHorizontalOffset(path, offset);
            applyHorizontalOffset(towerPos, offset);
            applyHorizontalOffset(obstacles, offset);
            setPath(path);
            setTowerPositions(towerPos);
        }
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

// ... (setPath, getPath, setTowerPositions, getTowerPositions, 等函数保持不变) ...
// (这里是 GameMap.cpp 中未显示的所有 getter/setter，它们不需要修改)

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

bool GameMap::isPathWidthExplicit() const
{
    return pathWidthExplicit;
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

// ... (parseJsonWithEncodingFallback 函数保持不变) ...
// (这个函数对于加载 JSON 本身很重要，不需要修改)

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

        // --- 【关键】 ---
        // 我们的逻辑永远优先检查 Qt 资源 (:/)
        if (candidate.startsWith(":/"))
        {
            QFile resourceFile(candidate);
            if (resourceFile.exists())
            {
                return candidate;
            }
            continue; // QRC 路径无效，尝试下一个
        }

        // --- (以下是旧的外部文件逻辑，保留作为备用) ---
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

// --- 【已修改】 使用 QRC 路径 ---
QString GameMap::stageSpecificBackgroundPath(int stageIndex)
{
    switch (stageIndex)
    {
    case 1:
        return QStringLiteral(":/background/resources/background/first_background.png");
    case 2:
        return QStringLiteral(":/background/resources/background/second_background.png");
    case 3:
        return QStringLiteral(":/background/resources/background/third_background.png");
    default:
        return {};
    }
}

// --- 【已修改】 使用 QRC 路径 ---
QString GameMap::stageSpecificPathTexture(int stageIndex)
{
    switch (stageIndex)
    {
    case 1:
        return QStringLiteral(":/map_items/resources/map_items/first/path_block.png");
    case 2:
        return QStringLiteral(":/map_items/resources/map_items/second/path_block.png");
    case 3:
        return QStringLiteral(":/map_items/resources/map_items/third/path_block.png");
    default:
        return {};
    }
}

// --- 【已修改】 路径在 QRC 中不存在，返回空 ---
QString GameMap::defaultTowerBaseFrame()
{
    // 这个资源 (防御塔选择框.png) 并没有在 resources.qrc 中定义。
    // 并且 MainWindow.cpp 似乎也没有实际使用它。
    // 返回空字符串是安全的，可以断开对外部文件的依赖。
    return QStringLiteral("");
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
        return 0.12548828125;
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

// ... (updateGridSpacing, extractSpacing, locateProjectRootPath, computeHorizontalCenterOffset, applyHorizontalOffset 等) ...
// (这些是纯逻辑函数，保持 100% 不变)

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
        // 【注意】：我们仍然保留这个检测，以防万一
        if (dir.exists(QStringLiteral("resources.qrc")))
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

double GameMap::computeHorizontalCenterOffset(const std::vector<QPointF> &points)
{
    if (points.empty())
    {
        return 0.0;
    }

    double minX = 1.0;
    double maxX = 0.0;
    for (const QPointF &pt : points)
    {
        minX = std::min(minX, pt.x());
        maxX = std::max(maxX, pt.x());
    }

    if (!(maxX > minX))
    {
        return 0.0;
    }

    const double currentCenter = minX + (maxX - minX) * 0.5;
    double offset = 0.5 - currentCenter;
    const double minOffset = -minX;
    const double maxOffset = 1.0 - maxX;
    offset = std::clamp(offset, minOffset, maxOffset);

    if (std::abs(offset) < 1e-5)
    {
        return 0.0;
    }
    return offset;
}

void GameMap::applyHorizontalOffset(std::vector<QPointF> &points, double offset)
{
    if (points.empty() || std::abs(offset) < 1e-6)
    {
        return;
    }
    for (QPointF &pt : points)
    {
        const double shifted = std::clamp(pt.x() + offset, 0.0, 1.0);
        pt.setX(shifted);
    }
}

void GameMap::applyHorizontalOffset(std::vector<ObstacleData> &obstacles, double offset)
{
    if (obstacles.empty() || std::abs(offset) < 1e-6)
    {
        return;
    }
    for (ObstacleData &obs : obstacles)
    {
        const double shifted = std::clamp(obs.relativePosition.x() + offset, 0.0, 1.0);
        obs.relativePosition.setX(shifted);
    }
}