#include "MainWindow.h"
#include "GameManager.h"
#include "Tower.h"

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QBrush>
#include <QMenu>
#include <QAction>
#include <QResizeEvent>
#include <QFileInfo>
#include <QCoreApplication>
#include <QDir>
#include <QMessageBox>
#include <QTimer>
#include <QPainter>
#include <QStringList>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include <QSet>
#include <algorithm>
#include <functional>
#include <cmath>

namespace
{
    struct TowerBalanceRow
    {
        const char *type;
        const char *displayName;
        int cost;
        double damage;
        double range;
        double fireRate;
    };

    struct EnemyBalanceRow
    {
        const char *type;
        const char *displayName;
        int health;
        double speed;
        int drops;
        int damage;
    };

    const TowerBalanceRow kTowerBalanceTable[] = {
        {"InspirationBulb", u8"灵感灯泡", 100, 40.0, 2.5, 0.8},
        {"KnowledgeTree", u8"知识古树", 200, 160.0, 3.5, 2.0},
        {"FishingCatPillow", u8"摸鱼猫抱枕", 120, 0.0, 2.5, 1.0},
        {"LiveCoffee", u8"续命咖啡", 80, 0.0, 2.5, 1.0},
        {"WarmMemories", u8"温暖的记忆", 150, 70.0, 2.8, 1.1},
        {"NightRadio", u8"深夜电台", 220, 130.0, 3.3, 1.6},
        {"PettingCatTime", u8"撸猫时间", 140, 0.0, 2.6, 1.0},
        {"Companionship", u8"朋友陪伴", 160, 0.0, 3.0, 1.2}};

    const EnemyBalanceRow kEnemyBalanceTable[] = {
        {"bug", u8"BUG", 150, 1.0, 10, 1},
        {"ddl", u8"DDL", 80, 1.5, 8, 1},
        {"gpa", u8"GPA", 300, 1.0, 15, 2},
        {"pre", u8"PRE", 500, 1.0, 30, 5},
        {"thesis", u8"Thesis", 8000, 0.8, 200, 20},
        {"coldwords", u8"冰冷的言语", 220, 1.1, 18, 2},
        {"loneliness", u8"孤独怪物", 320, 1.0, 20, 3},
        {"regret", u8"回忆怪物", 450, 0.9, 25, 4},
        {"tears", u8"泪水怪物", 380, 1.2, 22, 3},
        {"past", u8"昔日幻影", 1800, 0.8, 120, 10},
        {"nightmare", u8"梦魇", 12000, 0.7, 300, 25}};

    const TowerBalanceRow *lookupTowerBalance(const QString &type)
    {
        for (const TowerBalanceRow &row : kTowerBalanceTable)
        {
            if (QString::compare(type, QString::fromUtf8(row.type), Qt::CaseInsensitive) == 0)
            {
                return &row;
            }
        }
        return nullptr;
    }

    const EnemyBalanceRow *lookupEnemyBalance(const QString &type)
    {
        for (const EnemyBalanceRow &row : kEnemyBalanceTable)
        {
            if (QString::compare(type, QString::fromUtf8(row.type), Qt::CaseInsensitive) == 0)
            {
                return &row;
            }
        }
        return nullptr;
    }

    QString canonicalEnemyTypeName(const QString &type)
    {
        const QString lower = type.trimmed().toLower();
        if (lower == QStringLiteral("lonelyness"))
        {
            return QStringLiteral("loneliness");
        }
        if (lower == QStringLiteral("recall"))
        {
            return QStringLiteral("regret");
        }
        if (lower == QStringLiteral("thephantomofthepast"))
        {
            return QStringLiteral("past");
        }
        return type.trimmed();
    }
}

class TowerBaseItem : public QGraphicsEllipseItem
{
public:
    using ClickHandler = std::function<void(int, const QPointF &)>;

    TowerBaseItem(int index,
                  qreal radius,
                  const QPointF &center,
                  ClickHandler handler,
                  QGraphicsItem *parent = nullptr)
        : QGraphicsEllipseItem(parent),
          m_index(index),
          m_radius(radius),
          m_center(center),
          m_clickHandler(std::move(handler)),
          m_isOccupied(false)
    {
        setZValue(25);
        setAcceptHoverEvents(false);
        setAcceptedMouseButtons(Qt::LeftButton);
        setFlag(QGraphicsItem::ItemIsSelectable, false);
        setPen(Qt::NoPen);
        setBrush(Qt::NoBrush);
        setCenter(center);
    }

    void setCenter(const QPointF &center)
    {
        m_center = center;
        setRect(QRectF(center.x() - m_radius, center.y() - m_radius, m_radius * 2.0, m_radius * 2.0));
    }

    void setRadius(qreal radius)
    {
        m_radius = radius;
        setCenter(m_center);
    }

    void setOccupied(bool occupied)
    {
        m_isOccupied = occupied;
    }

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override
    {
        if (m_clickHandler)
        {
            m_clickHandler(m_index, event->scenePos());
        }
        event->accept();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        Q_UNUSED(painter);
        Q_UNUSED(option);
        Q_UNUSED(widget);
    }

private:
    int m_index;
    qreal m_radius;
    QPointF m_center;
    ClickHandler m_clickHandler;
    bool m_isOccupied;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_scene(new QGraphicsScene(this)),
      m_view(new QGraphicsView(m_scene, this)),
      m_backgroundItem(nullptr),
      m_baseRadius(26.0),
      m_sceneDesignSize(1024, 768)
{

    setWindowTitle("Dream Guardian");
    resize(1024, 768);
    setMinimumSize(800, 600);

    m_scene->setSceneRect(0, 0, m_sceneDesignSize.width(), m_sceneDesignSize.height());
    m_view->setSceneRect(m_scene->sceneRect());
    m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    m_view->setAlignment(Qt::AlignCenter);

    m_view->resetTransform();

    setCentralWidget(m_view);

    initializeScene();

    QTimer::singleShot(0, this, [this]()
                       {
                           fitViewToScene();
                           m_view->update(); });
}

MainWindow::~MainWindow() = default;

void MainWindow::initializeScene()
{
    GameManager *manager = GameManager::instance();
    manager->init(m_scene);
    const bool connected = connect(manager,
                                   SIGNAL(gameFinished(bool, int, int)),
                                   this,
                                   SLOT(handleGameFinished(bool, int, int)));
    if (!connected)
    {
        qWarning() << "GameManager::gameFinished 信号尚未可用，无法自动弹出结算界面。";
    }
    synchronizeLogicScreenSize();
    recalculateBaseRadius();

    const auto commandLineLevelCandidate = []() -> QString
    {
        const QStringList args = QCoreApplication::arguments();
        for (int i = 1; i < args.size(); ++i)
        {
            const QString &arg = args.at(i);
            if (arg.startsWith(QStringLiteral("--level="), Qt::CaseInsensitive))
            {
                return arg.section('=', 1);
            }
            if (arg.compare(QStringLiteral("--level"), Qt::CaseInsensitive) == 0 && i + 1 < args.size())
            {
                return args.at(i + 1);
            }
        }
        return {};
    }();

    const QString envLevelCandidate = QString::fromLocal8Bit(qgetenv("DREAM_LEVEL_PATH"));

    QStringList levelSearchOrder;
    QSet<QString> seenCandidates;
    auto appendCandidate = [&](const QString &candidate)
    {
        const QString trimmed = QDir::fromNativeSeparators(candidate.trimmed());
        if (trimmed.isEmpty() || seenCandidates.contains(trimmed))
        {
            return;
        }
        levelSearchOrder.append(trimmed);
        seenCandidates.insert(trimmed);
    };

    appendCandidate(commandLineLevelCandidate);
    appendCandidate(envLevelCandidate);
    appendCandidate(QStringLiteral("levels/level1.json"));
    appendCandidate(QStringLiteral("levels/level2.json"));
    appendCandidate(QStringLiteral("level.json"));
    appendCandidate(QStringLiteral("levels/stage1.json"));
    appendCandidate(QStringLiteral("levels/level3.json"));

    QString preparedLevelPath;
    for (const QString &candidate : levelSearchOrder)
    {
        const QString resolved = resolveLevelPath(candidate);
        if (resolved.isEmpty())
        {
            continue;
        }

        const QString preparedCandidate = prepareRuntimeLevelFile(resolved);
        if (preparedCandidate.isEmpty())
        {
            continue;
        }

        if (loadVisualLevel(preparedCandidate))
        {
            preparedLevelPath = preparedCandidate;
            break;
        }

        qWarning() << "Level candidate rejected during loading:" << resolved;
    }

    if (preparedLevelPath.isEmpty())
    {
        QMessageBox::critical(this, tr("Level Missing"), tr("Cannot find a usable level.json in the workspace."));
        return;
    }

    manager->loadLevel(preparedLevelPath);
    manager->startGame();
}

QString MainWindow::resolveLevelPath(const QString &candidate) const
{
    if (candidate.isEmpty())
    {
        return {};
    }

    QFileInfo info(candidate);
    if (info.exists())
    {
        return info.absoluteFilePath();
    }

    const QStringList baseDirs = {
        QCoreApplication::applicationDirPath(),
        QDir::currentPath()};

    for (const QString &base : baseDirs)
    {
        QFileInfo joined(QDir(base), candidate);
        if (joined.exists())
        {
            return joined.absoluteFilePath();
        }
    }

    for (const QString &base : baseDirs)
    {
        const QString found = searchForFileUpwards(base, candidate, 6);
        if (!found.isEmpty())
        {
            return found;
        }
    }

    return {};
}

QString MainWindow::searchForFileUpwards(const QString &startDir, const QString &fileName, int depthLimit) const
{
    if (startDir.isEmpty() || fileName.isEmpty() || depthLimit < 0)
    {
        return {};
    }

    QDir dir(startDir);
    for (int depth = 0; depth <= depthLimit && dir.exists(); ++depth)
    {
        QFileInfo candidate(dir.filePath(fileName));
        if (candidate.exists())
        {
            return candidate.absoluteFilePath();
        }
        if (!dir.cdUp())
        {
            break;
        }
    }
    return {};
}

QString MainWindow::prepareRuntimeLevelFile(const QString &levelPath)
{
    if (levelPath.isEmpty())
    {
        return {};
    }

    QFile file(levelPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Unable to open level for preprocessing:" << levelPath;
        return {};
    }

    QJsonParseError parseError;
    const QByteArray originalData = file.readAll();
    const QJsonDocument doc = GameMap::parseJsonWithEncodingFallback(levelPath, originalData, &parseError);
    file.close();
    if (!doc.isObject())
    {
        qWarning() << "Invalid json during preprocessing:" << parseError.errorString();
        return levelPath;
    }

    QJsonObject root = doc.object();
    int stage = GameMap::guessStageFromName(root.value(QStringLiteral("level_name")).toString());
    if (stage <= 0)
    {
        stage = 1;
    }
    const QString loweredPath = QFileInfo(levelPath).fileName().toLower();
    if (loweredPath.contains(QStringLiteral("level3")))
    {
        stage = 3;
    }
    else if (loweredPath.contains(QStringLiteral("level2")))
    {
        stage = std::max(stage, 2);
    }
    else if (loweredPath.contains(QStringLiteral("level1")))
    {
        stage = std::max(stage, 1);
    }

    const QString projectRootPath = locateProjectRootPath(levelPath);
    auto updateField = [this, &projectRootPath](QJsonObject &obj, const QString &field, const QString &candidate) -> bool
    {
        const QString resolved = absoluteAssetPath(candidate, projectRootPath);
        if (!resolved.isEmpty() && QFile::exists(resolved))
        {
            obj[field] = resolved;
            return true;
        }
        return false;
    };

    root.insert(QStringLiteral("_project_root"), projectRootPath);

    auto ensureTowerPrototype = [&](const QJsonObject &input) -> QJsonObject
    {
        QJsonObject tower = input;
        const QString type = tower.value(QStringLiteral("type")).toString().trimmed();
        if (type.isEmpty())
        {
            return tower;
        }
        tower.insert(QStringLiteral("type"), type);
        if (tower.value(QStringLiteral("name")).toString().trimmed().isEmpty())
        {
            const TowerBalanceRow *balance = lookupTowerBalance(type);
            tower.insert(QStringLiteral("name"),
                         balance && balance->displayName ? QString::fromUtf8(balance->displayName) : type);
        }

        const TowerBalanceRow *stat = lookupTowerBalance(type);
        auto ensureNumber = [&](const char *field, double value)
        {
            if (!tower.contains(field) || tower.value(field).toDouble() <= 0.0)
            {
                tower.insert(QLatin1String(field), value);
            }
        };
        if (stat)
        {
            tower.insert(QStringLiteral("cost"), stat->cost);
            ensureNumber("damage", stat->damage);
            ensureNumber("range", stat->range);
            ensureNumber("fire_rate", stat->fireRate);
        }
        else
        {
            tower.insert(QStringLiteral("cost"), tower.value(QStringLiteral("cost")).toInt(120));
            ensureNumber("damage", 40.0);
            ensureNumber("range", 2.5);
            ensureNumber("fire_rate", 1.0);
        }

        updateField(tower, QStringLiteral("pixmap"), GameMap::fallbackTowerPixmap(type, stage, false));
        updateField(tower, QStringLiteral("bullet_pixmap"), GameMap::fallbackTowerPixmap(type, stage, true));
        return tower;
    };

    auto ensureEnemyPrototype = [&](const QJsonObject &input) -> QJsonObject
    {
        QJsonObject enemy = input;
        QString type = canonicalEnemyTypeName(enemy.value(QStringLiteral("type")).toString());
        if (type.isEmpty())
        {
            type = QStringLiteral("nightmare");
        }
        enemy.insert(QStringLiteral("type"), type);
        if (enemy.value(QStringLiteral("name")).toString().trimmed().isEmpty())
        {
            const EnemyBalanceRow *balance = lookupEnemyBalance(type);
            enemy.insert(QStringLiteral("name"),
                         balance && balance->displayName ? QString::fromUtf8(balance->displayName) : type);
        }

        const EnemyBalanceRow *stat = lookupEnemyBalance(type);
        auto ensureNumber = [&](const char *field, double value)
        {
            if (!enemy.contains(field) || enemy.value(field).toDouble() <= 0.0)
            {
                enemy.insert(QLatin1String(field), value);
            }
        };
        if (stat)
        {
            ensureNumber("health", stat->health);
            ensureNumber("speed", stat->speed);
            ensureNumber("drops", stat->drops);
            ensureNumber("damage", stat->damage);
        }
        else
        {
            ensureNumber("health", 200.0);
            ensureNumber("speed", 1.0);
            ensureNumber("drops", 10.0);
            ensureNumber("damage", 2.0);
        }

        updateField(enemy, QStringLiteral("pixmap"), GameMap::fallbackEnemyPixmap(type, stage));
        return enemy;
    };

    QJsonArray towerArray = root.value(QStringLiteral("available_towers")).toArray();
    QJsonArray normalizedTowers;
    for (const QJsonValue &value : towerArray)
    {
        if (value.isObject())
        {
            normalizedTowers.append(ensureTowerPrototype(value.toObject()));
        }
        else if (value.isString())
        {
            QJsonObject prototype;
            prototype.insert(QStringLiteral("type"), value.toString());
            normalizedTowers.append(ensureTowerPrototype(prototype));
        }
    }
    root.insert(QStringLiteral("available_towers"), normalizedTowers);

    QJsonArray enemyArray = root.value(QStringLiteral("available_enemies")).toArray();
    if (enemyArray.isEmpty())
    {
        enemyArray.append(QStringLiteral("nightmare"));
    }
    QJsonArray normalizedEnemies;
    QStringList normalizedEnemyTypes;
    for (const QJsonValue &value : enemyArray)
    {
        QJsonObject enemyObj;
        if (value.isObject())
        {
            enemyObj = ensureEnemyPrototype(value.toObject());
        }
        else
        {
            QJsonObject proto;
            proto.insert(QStringLiteral("type"), value.toString());
            enemyObj = ensureEnemyPrototype(proto);
        }
        normalizedEnemyTypes.append(enemyObj.value(QStringLiteral("type")).toString());
        normalizedEnemies.append(enemyObj);
    }
    root.insert(QStringLiteral("available_enemies"), normalizedEnemies);

    const QString defaultEnemyType = normalizedEnemyTypes.isEmpty()
                                         ? QStringLiteral("nightmare")
                                         : normalizedEnemyTypes.constFirst();

    QJsonArray waves = root.value(QStringLiteral("waves")).toArray();
    for (int waveIndex = 0; waveIndex < waves.size(); ++waveIndex)
    {
        QJsonObject waveObj = waves.at(waveIndex).toObject();
        QJsonArray spawnEntries = waveObj.value(QStringLiteral("enemies")).toArray();
        for (int spawnIndex = 0; spawnIndex < spawnEntries.size(); ++spawnIndex)
        {
            QJsonObject spawn = spawnEntries.at(spawnIndex).toObject();
            QString type = canonicalEnemyTypeName(spawn.value(QStringLiteral("type")).toString());
            if (type.isEmpty())
            {
                type = defaultEnemyType;
            }
            spawn.insert(QStringLiteral("type"), type);
            spawnEntries[spawnIndex] = spawn;
        }
        waveObj.insert(QStringLiteral("enemies"), spawnEntries);
        waves[waveIndex] = waveObj;
    }
    root.insert(QStringLiteral("waves"), waves);

    QJsonObject mapObj = root.value(QStringLiteral("map")).toObject();
    QJsonArray obstacles = mapObj.value(QStringLiteral("obstacles")).toArray();
    for (int i = 0; i < obstacles.size(); ++i)
    {
        QJsonObject obstacle = obstacles.at(i).toObject();
        if (updateField(obstacle, QStringLiteral("pixmap"), GameMap::fallbackObstaclePixmap(obstacle.value(QStringLiteral("type")).toString(), stage)))
        {
            obstacles[i] = obstacle;
        }
    }
    if (!obstacles.isEmpty())
    {
        mapObj.insert(QStringLiteral("obstacles"), obstacles);
    }

    auto ensureAbsoluteField = [&](const QString &field, const QString &fallbackCandidate)
    {
        const QString current = mapObj.value(field).toString();
        if (!current.isEmpty() && updateField(mapObj, field, current))
        {
            return;
        }
        if (!fallbackCandidate.isEmpty())
        {
            updateField(mapObj, field, fallbackCandidate);
        }
    };

    ensureAbsoluteField(QStringLiteral("background"), GameMap::fallbackBackgroundPixmap(stage));
    ensureAbsoluteField(QStringLiteral("path_texture"), GameMap::fallbackPathTexture(stage));
    ensureAbsoluteField(QStringLiteral("tower_base"), GameMap::fallbackTowerBaseFrame());

    root.insert(QStringLiteral("map"), mapObj);

    const QString cacheName = QStringLiteral("dream_level_%1.json").arg(QFileInfo(levelPath).completeBaseName());
    const QString targetPath = QDir(QDir::tempPath()).filePath(cacheName);
    QFile out(targetPath);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning() << "Failed to write prepared level file:" << targetPath;
        return levelPath;
    }
    out.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    out.close();
    return targetPath;
}

QString MainWindow::absoluteAssetPath(const QString &path, const QString &projectRoot) const
{
    if (path.isEmpty())
    {
        return {};
    }

    QFileInfo info(path);
    if (info.isAbsolute())
    {
        return info.absoluteFilePath();
    }

    if (projectRoot.isEmpty())
    {
        return path;
    }

    QFileInfo resolved(QDir(projectRoot), path);
    if (resolved.exists())
    {
        return resolved.absoluteFilePath();
    }

    return path;
}

QString MainWindow::locateProjectRootPath(const QString &levelPath) const
{
    QFileInfo info(levelPath);
    if (!info.exists())
    {
        return QDir::currentPath();
    }

    QDir dir = info.absoluteDir();
    for (int depth = 0; depth < 8; ++depth)
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

    return info.absoluteDir().absolutePath();
}

void MainWindow::handleGameFinished(bool win, int stability, int killCount)
{
    showPostGameWidget(win, stability, killCount);
}

void MainWindow::showPostGameWidget(bool win, int stability, int killCount)
{
    if (m_postGameWidget)
    {
        m_postGameWidget->close();
        m_postGameWidget->deleteLater();
    }
    m_postGameWidget = new widget_post_game(win, stability, killCount, this);
    m_postGameWidget->setAttribute(Qt::WA_DeleteOnClose);
    m_postGameWidget->show();
}

bool MainWindow::loadVisualLevel(const QString &levelPath)
{
    if (levelPath.isEmpty())
    {
        return false;
    }

    if (!m_visualMap.loadFromFile(levelPath))
    {
        qWarning() << "GameMap failed to load" << levelPath;
        return false;
    }

    m_currentLevelPath = levelPath;
    applySceneRectFromMap();
    populateSceneFromMap();
    refreshAllBaseStates();
    fitViewToScene();
    return true;
}

void MainWindow::populateSceneFromMap()
{
    drawBackgroundLayer();
    drawPathLayer();
    drawObstacles();
    createTowerBaseItems();
}

void MainWindow::drawBackgroundLayer()
{
    if (!m_backgroundItem)
    {
        m_backgroundItem = new QGraphicsPixmapItem();
        m_backgroundItem->setZValue(-100);
        m_scene->addItem(m_backgroundItem);
    }

    const QRectF rect = m_scene->sceneRect();
    QPixmap backgroundPixmap;

    const QString bgPath = m_visualMap.getBackgroundPixmap();
    if (!bgPath.isEmpty())
    {
        QPixmap pix(bgPath);
        if (!pix.isNull())
        {

            if (pix.size() == rect.size().toSize())
            {
                backgroundPixmap = pix;
            }
            else
            {
                backgroundPixmap = pix.scaled(rect.size().toSize(),
                                              Qt::KeepAspectRatio,
                                              Qt::SmoothTransformation);
            }
        }
        else
        {
            qWarning() << "❌ 无法加载背景图:" << bgPath;
            backgroundPixmap = QPixmap(rect.size().toSize());
            backgroundPixmap.fill(QColor(135, 206, 235));
        }
    }
    else
    {
        qWarning() << "⚠️  背景图路径为空，使用纯色背景";
        backgroundPixmap = QPixmap(rect.size().toSize());
        backgroundPixmap.fill(QColor(135, 206, 235));
    }

    m_backgroundItem->setPixmap(backgroundPixmap);

    qreal xOffset = (rect.width() - backgroundPixmap.width()) / 2.0;
    qreal yOffset = (rect.height() - backgroundPixmap.height()) / 2.0;
    m_backgroundItem->setPos(rect.left() + xOffset, rect.top() + yOffset);
}

namespace
{
    constexpr qreal kReferenceSceneWidth = 1024.0;
    constexpr qreal kReferenceSceneHeight = 768.0;
    constexpr qreal kReferenceTileSize = 118.5;
    constexpr qreal kReferenceObstacleSize = 152.0;
    constexpr qreal kObstacleToTileRatio = kReferenceObstacleSize / kReferenceTileSize;

    qreal referenceScaleFactor(const QSizeF &sceneSize)
    {
        if (sceneSize.isEmpty())
        {
            return 1.0;
        }
        const qreal sx = sceneSize.width() / kReferenceSceneWidth;
        const qreal sy = sceneSize.height() / kReferenceSceneHeight;
        return std::min(sx, sy);
    }
}

void MainWindow::drawPathLayer()
{
    for (QGraphicsItem *item : m_pathItems)
    {
        if (item)
        {
            m_scene->removeItem(item);
            delete item;
        }
    }
    m_pathItems.clear();
    m_pathTileRects.clear();

    const auto &pathData = m_visualMap.getPath();
    if (pathData.empty())
    {
        qWarning() << "⚠️ 路径数据为空";
        return;
    }

    const QSizeF sceneSize = m_scene->sceneRect().size();
    if (sceneSize.isEmpty())
    {
        return;
    }

    const qreal gridStepX = std::max<qreal>(1e-4, m_visualMap.getGridSpacingX());
    const qreal gridStepY = std::max<qreal>(1e-4, m_visualMap.getGridSpacingY());
    const qreal pixelStepX = sceneSize.width() * gridStepX;
    const qreal pixelStepY = sceneSize.height() * gridStepY;
    const qreal scaleFactor = referenceScaleFactor(sceneSize);
    const qreal desiredTile = kReferenceTileSize * scaleFactor;
    const qreal spacingReference = std::max<qreal>(4.0, std::min(pixelStepX, pixelStepY));
    const qreal lowerBound = spacingReference * 1.05;
    const qreal upperBound = spacingReference * 1.8;

    qreal tileSide = sceneSize.width() * std::max<qreal>(m_visualMap.getPathWidthRatio(), 1e-4);
    if (!(tileSide > 0.0))
    {
        tileSide = desiredTile;
    }

    if (tileSide < lowerBound)
    {
        tileSide = lowerBound;
    }
    else if (tileSide > upperBound)
    {
        tileSide = upperBound;
    }

    QPixmap texture(m_visualMap.getPathTexturePixmap());
    const bool hasTexture = !texture.isNull();
    if (hasTexture)
    {
        texture = texture.scaled(QSize(qRound(tileSide), qRound(tileSide)),
                                 Qt::IgnoreAspectRatio,
                                 Qt::SmoothTransformation);
    }

    auto quantize = [](double value) -> qint32
    {
        return static_cast<qint32>(std::round(value * 10000.0));
    };

    QSet<qulonglong> occupiedCells;
    auto cellKey = [&](const QPointF &relative) -> qulonglong
    {
        const qint32 qx = quantize(relative.x());
        const qint32 qy = quantize(relative.y());
        return (static_cast<qulonglong>(static_cast<quint32>(qx)) << 32) |
               static_cast<quint32>(qy);
    };

    auto appendTile = [&](const QPointF &relative)
    {
        const qulonglong key = cellKey(relative);
        if (occupiedCells.contains(key))
        {
            return;
        }
        occupiedCells.insert(key);

        const QPointF center = toAbsolutePosition(relative);
        QRectF rect(center.x() - tileSide / 2.0,
                    center.y() - tileSide / 2.0,
                    tileSide,
                    tileSide);

        QGraphicsItem *item = nullptr;
        if (hasTexture)
        {
            auto *pixItem = new QGraphicsPixmapItem(texture);
            pixItem->setPos(rect.topLeft());
            item = pixItem;
        }
        else
        {
            auto *rectItem = new QGraphicsRectItem(rect);
            rectItem->setBrush(QColor(210, 180, 255, 220));
            rectItem->setPen(QPen(QColor(150, 120, 200, 240), 2));
            item = rectItem;
        }

        item->setZValue(-10);
        m_scene->addItem(item);
        m_pathItems.append(item);
        m_pathTileRects.append(rect);
    };

    auto fillBetween = [&](const QPointF &from, const QPointF &to)
    {
        const QPointF delta = to - from;
        const qreal normX = gridStepX > 1e-5 ? std::abs(delta.x()) / gridStepX : 0.0;
        const qreal normY = gridStepY > 1e-5 ? std::abs(delta.y()) / gridStepY : 0.0;
        const qreal normalizedDistance = std::max(normX, normY);
        const int segments = std::max(1, static_cast<int>(std::round(normalizedDistance)));

        for (int i = 1; i <= segments; ++i)
        {
            const qreal t = static_cast<qreal>(i) / static_cast<qreal>(segments);
            appendTile(from + delta * t);
        }
    };

    QPointF previous = pathData.front();
    appendTile(previous);
    for (size_t i = 1; i < pathData.size(); ++i)
    {
        const QPointF current = pathData[i];
        fillBetween(previous, current);
        previous = current;
    }
}

void MainWindow::drawObstacles()
{
    m_obstacleRects.clear();

    const auto &obstacles = m_visualMap.getObstacles();
    if (obstacles.empty())
    {
        return;
    }

    const QSizeF sceneSize = m_scene->sceneRect().size();
    if (sceneSize.isEmpty())
    {
        return;
    }

    qreal tileVisualSize = kReferenceTileSize * referenceScaleFactor(sceneSize);
    if (!m_pathTileRects.isEmpty())
    {
        tileVisualSize = std::max<qreal>(m_pathTileRects.first().width(), 1.0);
    }

    const qreal targetObstacleSize = tileVisualSize * kObstacleToTileRatio;

    for (const auto &obs : obstacles)
    {
        QPixmap pix(obs.pixmapPath);
        if (pix.isNull())
        {
            qWarning() << "❌ 无法加载障碍物图片:" << obs.pixmapPath;
            continue;
        }

        QPixmap scaled = pix.scaled(QSize(qRound(targetObstacleSize), qRound(targetObstacleSize)),
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);

        const QPointF center = toAbsolutePosition(obs.relativePosition);
        QRectF rect(center.x() - scaled.width() / 2.0,
                    center.y() - scaled.height() / 2.0,
                    scaled.width(),
                    scaled.height());

        m_obstacleRects.append(rect);
    }
}

bool MainWindow::isScenePointBlocked(const QPointF &scenePos) const
{
    for (const QRectF &rect : m_pathTileRects)
    {
        if (rect.contains(scenePos))
        {
            return true;
        }
    }
    for (const QRectF &rect : m_obstacleRects)
    {
        if (rect.contains(scenePos))
        {
            return true;
        }
    }
    return false;
}

void MainWindow::createTowerBaseItems()
{
    for (TowerBaseVisual &base : m_towerBases)
    {
        if (base.graphicsItem)
        {
            m_scene->removeItem(base.graphicsItem);
            delete base.graphicsItem;
        }
    }
    m_towerBases.clear();

    const auto &bases = m_visualMap.getTowerPositions();

    m_towerBases.reserve(static_cast<int>(bases.size()));

    for (int i = 0; i < static_cast<int>(bases.size()); ++i)
    {
        const QPointF relative = bases[static_cast<size_t>(i)];
        const QPointF center = toAbsolutePosition(relative);
        if (isScenePointBlocked(center))
        {
            continue;
        }

        auto handler = [this](int index, const QPointF &pos)
        {
            onTowerBaseClicked(index, pos);
        };
        auto *baseItem = new TowerBaseItem(static_cast<int>(m_towerBases.size()), m_baseRadius, center, handler);
        m_scene->addItem(baseItem);

        TowerBaseVisual visual;
        visual.relativePosition = relative;
        visual.graphicsItem = baseItem;
        m_towerBases.append(visual);
    }
}

void MainWindow::updateTowerBaseGeometry()
{
    for (TowerBaseVisual &base : m_towerBases)
    {
        if (base.graphicsItem)
        {
            base.graphicsItem->setRadius(m_baseRadius);
            base.graphicsItem->setCenter(toAbsolutePosition(base.relativePosition));
        }
    }
}

void MainWindow::updateSingleBaseState(int index)
{
    if (index < 0 || index >= m_towerBases.size())
    {
        return;
    }
    TowerBaseVisual &base = m_towerBases[index];
    if (!base.graphicsItem)
    {
        return;
    }
    const bool occupied = baseHasTower(index);
    base.graphicsItem->setOccupied(occupied);
}

void MainWindow::refreshAllBaseStates()
{
    for (int i = 0; i < m_towerBases.size(); ++i)
    {
        updateSingleBaseState(i);
    }
}

void MainWindow::recalculateBaseRadius()
{
    if (!m_scene)
    {
        return;
    }
    const QSizeF size = m_scene->sceneRect().size();
    if (size.isEmpty())
    {
        m_baseRadius = 30.0;
        return;
    }
    const qreal pixelStepX = size.width() * std::max<qreal>(1e-4, m_visualMap.getGridSpacingX());
    const qreal pixelStepY = size.height() * std::max<qreal>(1e-4, m_visualMap.getGridSpacingY());
    const qreal tile = std::max(pixelStepX, pixelStepY);
    m_baseRadius = std::max<qreal>(24.0, tile * 0.4);
}

void MainWindow::applySceneRectFromMap()
{
    if (!m_scene)
    {
        return;
    }

    QSizeF hint = mapSceneSizeHint();

    if (hint.isEmpty())
    {
        hint = defaultSceneSize();
    }
    if (qFuzzyCompare(hint.width(), 0.0) || qFuzzyCompare(hint.height(), 0.0))
    {
        hint = defaultSceneSize();
    }

    m_sceneDesignSize = hint;
    m_scene->setSceneRect(0, 0, hint.width(), hint.height());
    m_view->setSceneRect(m_scene->sceneRect());

    recalculateBaseRadius();

    updateTowerBaseGeometry();
    synchronizeLogicScreenSize();
    fitViewToScene();
}

QSizeF MainWindow::mapSceneSizeHint() const
{
    const QString bgPath = m_visualMap.getBackgroundPixmap();
    if (!bgPath.isEmpty())
    {
        QPixmap pix(bgPath);
        if (!pix.isNull())
        {
            QSizeF size = pix.size();
            const qreal upperBound = 2200.0;
            const qreal lowerBound = 800.0;
            qreal maxDim = std::max(size.width(), size.height());
            if (maxDim > upperBound)
            {
                const qreal factor = upperBound / maxDim;
                size *= factor;
                maxDim = upperBound;
            }
            if (maxDim < lowerBound)
            {
                const qreal factor = lowerBound / maxDim;
                size *= factor;
            }
            return size;
        }
    }
    return {};
}

QPointF MainWindow::toAbsolutePosition(const QPointF &relative) const
{
    const QRectF rect = m_scene->sceneRect();
    return {rect.left() + relative.x() * rect.width(), rect.top() + relative.y() * rect.height()};
}

QPoint MainWindow::sceneToGlobalPoint(const QPointF &scenePos) const
{
    QPoint viewPoint = m_view->mapFromScene(scenePos);
    return m_view->viewport()->mapToGlobal(viewPoint);
}

void MainWindow::showBuildMenu(int baseIndex, const QPoint &globalPos)
{
    const auto &towerChoices = m_visualMap.getAvailableTowers();
    if (towerChoices.empty())
    {
        QMessageBox::information(this, tr("No Towers"), tr("No tower types are defined in this level."));
        return;
    }

    QMenu menu(this);
    for (const auto &proto : towerChoices)
    {
        QString label = proto.name.isEmpty() ? proto.type : proto.name;
        label.append(QStringLiteral(" (%1)").arg(QString::number(proto.cost)));
        QAction *action = menu.addAction(label);
        action->setData(proto.type);
    }

    QAction *selected = menu.exec(globalPos);
    if (!selected)
    {
        return;
    }

    const QString towerType = selected->data().toString();
    if (towerType.isEmpty())
    {
        return;
    }

    const QPointF relativePos = m_towerBases[baseIndex].relativePosition;
    GameManager::instance()->buildTower(towerType, relativePos);

    QTimer::singleShot(0, this, [this, baseIndex]()
                       { updateSingleBaseState(baseIndex); });
}

void MainWindow::showUpgradeMenu(int baseIndex, const QPoint &globalPos)
{
    QMenu menu(this);
    QAction *upgradeAction = menu.addAction(tr("Upgrade Tower"));
    QAction *sellAction = menu.addAction(tr("Sell Tower"));

    QAction *selected = menu.exec(globalPos);
    if (!selected)
    {
        return;
    }

    const QPointF relativePos = m_towerBases[baseIndex].relativePosition;
    if (selected == upgradeAction)
    {
        emit towerUpgradeRequested(relativePos);
        QMessageBox::information(this, tr("Upgrade"), tr("Upgrade logic not implemented yet."));
    }
    else if (selected == sellAction)
    {
        emit towerSellRequested(relativePos);
        QMessageBox::information(this, tr("Sell"), tr("Sell logic not implemented yet."));
    }
}

bool MainWindow::baseHasTower(int baseIndex) const
{
    if (baseIndex < 0 || baseIndex >= m_towerBases.size())
    {
        return false;
    }

    const QPointF center = toAbsolutePosition(m_towerBases[baseIndex].relativePosition);
    const QRectF area(center.x() - m_baseRadius, center.y() - m_baseRadius, m_baseRadius * 2.0, m_baseRadius * 2.0);
    const QList<QGraphicsItem *> items = m_scene->items(area);
    for (QGraphicsItem *item : items)
    {
        if (dynamic_cast<Tower *>(item))
        {
            return true;
        }
    }
    return false;
}

void MainWindow::synchronizeLogicScreenSize() const
{
    if (!m_scene)
    {
        return;
    }
    GameManager::instance()->setScreenSize(m_scene->sceneRect().size());
}

void MainWindow::fitViewToScene()
{
    if (!m_scene || !m_view)
    {
        return;
    }

    m_view->resetTransform();

    m_view->fitInView(m_scene->sceneRect(), Qt::KeepAspectRatio);
}

QSizeF MainWindow::defaultSceneSize() const
{

    return QSizeF(1024, 768);
}

void MainWindow::onTowerBaseClicked(int baseIndex, const QPointF &scenePos)
{
    if (baseIndex < 0 || baseIndex >= m_towerBases.size())
    {
        return;
    }

    const QPoint globalPos = sceneToGlobalPoint(scenePos);
    if (baseHasTower(baseIndex))
    {
        showUpgradeMenu(baseIndex, globalPos);
    }
    else
    {
        showBuildMenu(baseIndex, globalPos);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    QTimer::singleShot(0, this, [this]()
                       { fitViewToScene(); });
}
