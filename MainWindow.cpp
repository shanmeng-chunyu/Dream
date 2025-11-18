#include "MainWindow.h"
#include "GameManager.h"
#include "Tower.h"
#include "Player.h"
#include "WaveManager.h"
#include "widget_ingame.h"
#include "widget_pause_menu.h"
#include "widget_building_list.h"

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
#include <QFile>
#include <QGraphicsBlurEffect>
#include <QImage>
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
#include <QKeyEvent>
#include <QLabel>
#include <QProgressBar>

#include <QSet>
#include <algorithm>
#include <functional>
#include <cmath>
#include <QGraphicsOpacityEffect>

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
        setZValue(-1);
        setAcceptHoverEvents(false);
        setAcceptedMouseButtons(Qt::LeftButton);
        setFlag(QGraphicsItem::ItemIsSelectable, false);
        // setPen(Qt::NoPen);
        // setBrush(Qt::NoBrush);
        setPen(Qt::NoPen);        // 设置一个2像素宽的红色边框
        setBrush(QColor(255, 255, 255, 90)); // 设置一个半透明的白色填充 (R, G, B, Alpha)
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

    // void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    // {
    //     Q_UNUSED(painter);
    //     Q_UNUSED(option);
    //     Q_UNUSED(widget);
    // }

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
      m_sceneDesignSize(1024, 768),
      m_currentLevelIndex(-1),
      m_hudWidget(nullptr),
      m_pauseMenuWidget(nullptr),
      m_initialStability(0),
      m_totalEnemyCount(0),
      m_spawnedEnemyCount(0),
      m_currentWaveCounter(0),
      m_spawnedInCurrentWave(0),
      m_gameLoopTimer(nullptr),
      m_fastModeActive(false)
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

    m_blurRadius = 20; // 模糊半径
    if (!m_rawBg.load(":/background/resources/background/choose_.png")) {
        qWarning() << "MainWindow: Failed to load blurred background source image.";
    }

    // 1. 设置 MainWindow 自身为不自动填充 (我们将手动绘制)
    this->setAutoFillBackground(false);

    // 2. 【关键】设置 QGraphicsView 和它的视口(viewport)透明
    // 这样才能看到 MainWindow 绘制的模糊背景
    m_view->setAttribute(Qt::WA_TranslucentBackground);
    m_view->viewport()->setAttribute(Qt::WA_Hover, true);
    m_view->setStyleSheet("background: transparent; border: none;");
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
    connect(this, &MainWindow::towerUpgradeRequested, manager, &GameManager::onTowerUpgradeRequested);
    connect(this, &MainWindow::towerSellRequested, manager, &GameManager::onTowerSellRequested);
    // connect(manager, &GameManager::obstacleCleared, this, &MainWindow::onObstacleAreaCleared);
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
    appendCandidate(QStringLiteral("levels/level3.json"));

    m_levelSources.clear();
    for (const QString &candidate : levelSearchOrder)
    {
        const QString resolved = resolveLevelPath(candidate);
        if (resolved.isEmpty())
        {
            continue;
        }
        if (m_levelSources.contains(resolved))
        {
            continue;
        }
        m_levelSources.append(resolved);
    }

    // bool loaded = false;
    // for (int idx = 0; idx < m_levelSources.size(); ++idx)
    // {
    //     if (loadLevelByIndex(idx, false))
    //     {
    //         loaded = true;
    //         break;
    //     }
    // }
    //
    // if (!loaded)
    // {
    //     QMessageBox::critical(this, tr("Level Missing"), tr("Cannot find a usable level.json in the workspace."));
    //     return;
    // }

    connectHudToSystems();
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
        resetWaveTracking();
        return {};
    }

    QFile file(levelPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Unable to open level for preprocessing:" << levelPath;
        resetWaveTracking();
        return {};
    }

    QJsonParseError parseError;
    const QByteArray originalData = file.readAll();
    const QJsonDocument doc = GameMap::parseJsonWithEncodingFallback(levelPath, originalData, &parseError);
    file.close();
    if (!doc.isObject())
    {
        qWarning() << "Invalid json during preprocessing:" << parseError.errorString();
        resetWaveTracking();
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

        // 1. 获取 JSON 中指定的路径
        const QString currentPath = obstacle.value(QStringLiteral("pixmap")).toString();

        // 2. 获取备选的 Fallback 路径
        const QString fallbackPath = GameMap::fallbackObstaclePixmap(obstacle.value(QStringLiteral("type")).toString(), stage);

        bool updated = false;

        // 3. 优先尝试使用 JSON 中指定的路径
        if (!currentPath.isEmpty())
        {
            updated = updateField(obstacle, QStringLiteral("pixmap"), currentPath);
        }

        // 4. 如果 JSON 路径失败了（或为空），再尝试使用 Fallback 路径
        if (!updated && !fallbackPath.isEmpty())
        {
            updateField(obstacle, QStringLiteral("pixmap"), fallbackPath);
        }

        obstacles[i] = obstacle; // 将修改后的 obstacle 写回数组
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

    prepareWaveTrackingFromJson(root);

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
    if (path.startsWith(QStringLiteral(":/")))
    {
        return path; // 这是一个 QRC 路径, 直接返回它
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
    dismissPostGameWidget();
    m_postGameWidget = new widget_post_game(win, stability, killCount, this);
    m_postGameWidget->setAttribute(Qt::WA_DeleteOnClose);

    // 2. 获取父窗口 (MainWindow) 和子窗口 (结算界面) 的大小
    //    (根据 widget_post_game.ui，我们知道它的固定大小是 400x400)
    int childWidth = m_postGameWidget->width();   // 应该是 400
    int childHeight = m_postGameWidget->height(); // 应该是 400

    // 3. 计算中心点的 X 和 Y 坐标
    int x = (this->width() - childWidth) / 2;
    int y = (this->height() - childHeight) / 2;

    // 4. 将子窗口移动到计算出的位置
    m_postGameWidget->move(x, y);

    connect(m_postGameWidget, &widget_post_game::repeat, this, [this]()
            {
                const int targetIndex = m_currentLevelIndex;
                dismissPostGameWidget();
                if (targetIndex >= 0)
                {
                    loadLevelByIndex(targetIndex, true);
                } });

    connect(m_postGameWidget, &widget_post_game::next, this, [this]()
            {
                const int nextIndex = m_currentLevelIndex + 1;
                dismissPostGameWidget();
                if (nextIndex < m_levelSources.size())
                {
                    loadLevelByIndex(nextIndex, true);
                }
                else
                {
                    QMessageBox::information(this,
                                             tr("Level Switch"),
                                             tr("No further level is available. Please select a level manually."));
                } });

    m_postGameWidget->show();
}

void MainWindow::dismissPostGameWidget()
{
    if (!m_postGameWidget)
    {
        return;
    }
    m_postGameWidget->close();
    m_postGameWidget->deleteLater();
    m_postGameWidget = nullptr;
}

void MainWindow::updateLevelSwitchStatus(int index)
{
    if (index < 0 || index >= m_levelSources.size())
    {
        setWindowTitle(QStringLiteral("Dream Guardian"));
        return;
    }

    QFileInfo info(m_levelSources.at(index));
    setWindowTitle(QStringLiteral("Dream Guardian - %1").arg(info.fileName()));
}

bool MainWindow::loadLevelByIndex(int index, bool showError)
{
    if (index < 0 || index >= m_levelSources.size())
    {
        if (showError)
        {
            QMessageBox::information(this,
                                     tr("Level Switch"),
                                     tr("No level mapped to shortcut %1.").arg(QString::number(index + 1)));
        }
        return false;
    }

    const QString sourcePath = m_levelSources.at(index);
    const QString preparedPath = prepareRuntimeLevelFile(sourcePath);
    if (preparedPath.isEmpty())
    {
        if (showError)
        {
            QMessageBox::warning(this,
                                 tr("Level Switch"),
                                 tr("Failed to preprocess:\n%1").arg(sourcePath));
        }
        return false;
    }

    if (!loadVisualLevel(preparedPath))
    {
        if (showError)
        {
            QMessageBox::warning(this,
                                 tr("Level Switch"),
                                 tr("Unable to render level:\n%1").arg(sourcePath));
        }
        return false;
    }

    GameManager *manager = GameManager::instance();
    manager->loadLevel(preparedPath);
    manager->startGame();

    destroyHudWidget();
    m_currentLevelIndex = index;
    ensureHudWidget();
    connectHudToSystems();
    if (m_hudWidget)
    {
        m_hudWidget->show();
        m_hudWidget->raise();
        if (m_cachedPlayer)
        {
            m_hudWidget->set_resource_value(m_cachedPlayer->getResource());
            updateHudStability(m_cachedPlayer->getStability());
        }
        updateHudProgress();
    }
    positionHudWidget();


    updateLevelSwitchStatus(index);
    return true;
}

bool MainWindow::startLevelFromSource(const QString &candidatePath, bool showError)
{
    const QString resolved = resolveLevelPath(candidatePath);
    if (resolved.isEmpty())
    {
        if (showError)
        {
            QMessageBox::warning(this,
                                 tr("Level Switch"),
                                 tr("Cannot locate level file:\n%1").arg(candidatePath));
        }
        return false;
    }

    int index = m_levelSources.indexOf(resolved);
    if (index < 0)
    {
        m_levelSources.append(resolved);
        index = m_levelSources.size() - 1;
    }

    return loadLevelByIndex(index, showError);
}

void MainWindow::cycleLevel(int delta)
{
    if (m_levelSources.isEmpty() || delta == 0)
    {
        return;
    }

    int nextIndex = m_currentLevelIndex;
    if (nextIndex < 0)
    {
        nextIndex = delta > 0 ? 0 : m_levelSources.size() - 1;
    }
    else
    {
        nextIndex = (nextIndex + delta + m_levelSources.size()) % m_levelSources.size();
    }

    loadLevelByIndex(nextIndex, true);
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

    updateBaseAvailability();
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
            base.graphicsItem = nullptr;
        }
    }
    m_towerBases.clear();

    const QRectF sceneRect = m_scene->sceneRect();
    if (sceneRect.isEmpty())
    {
        return;
    }

    const double spacingX = std::max<double>(1e-4, m_visualMap.getGridSpacingX());
    const double spacingY = std::max<double>(1e-4, m_visualMap.getGridSpacingY());
    const double fallbackSpacing = 0.08;
    const double effectiveSpacingX = spacingX > 1e-4 ? spacingX : fallbackSpacing;
    const double effectiveSpacingY = spacingY > 1e-4 ? spacingY : fallbackSpacing;
    m_cellSize = QSizeF(sceneRect.width() * effectiveSpacingX,
                        sceneRect.height() * effectiveSpacingY);
    if (m_cellSize.isEmpty())
    {
        return;
    }

    recalculateBaseRadius();

    const double offsetX = estimateGridOffset(true, effectiveSpacingX);
    const double offsetY = estimateGridOffset(false, effectiveSpacingY);
    const QVector<double> columns = buildAxisCoordinates(offsetX, effectiveSpacingX);
    const QVector<double> rows = buildAxisCoordinates(offsetY, effectiveSpacingY);
    const qreal intersectionTolerance = cellIntersectionTolerance();

    auto withinUnit = [](double value)
    {
        return value >= -1e-4 && value <= 1.0 + 1e-4;
    };

    for (double relY : rows)
    {
        if (!withinUnit(relY))
        {
            continue;
        }
        for (double relX : columns)
        {
            if (!withinUnit(relX))
            {
                continue;
            }

            QPointF relative(relX, relY);
            QPointF center = toAbsolutePosition(relative);
            QRectF cellRect(center.x() - m_cellSize.width() / 2.0,
                            center.y() - m_cellSize.height() / 2.0,
                            m_cellSize.width(),
                            m_cellSize.height());

            if (!sceneRect.contains(cellRect))
            {
                continue;
            }

            if (rectIntersectsAny(m_pathTileRects, cellRect, intersectionTolerance))
            {
                continue;
            }

            TowerBaseVisual base;
            base.relativePosition = relative;
            base.cellRect = cellRect;
            base.blockedByObstacle = rectIntersectsAny(m_obstacleRects, cellRect, intersectionTolerance);
            base.graphicsItem = nullptr;
            const int index = m_towerBases.size();
            m_towerBases.append(base);

            if (!m_towerBases[index].blockedByObstacle)
            {
                spawnBaseItem(index);
            }
        }
    }

    refreshAllBaseStates();
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
    m_baseRadius = std::max<qreal>(10.0, tile * 0.5);
    if (!m_cellSize.isEmpty())
    {
        const qreal halfCell = 0.5 * std::min(m_cellSize.width(), m_cellSize.height());
        m_baseRadius = std::min(m_baseRadius, halfCell * 1.0);
    }
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
    // --- 1. 准备数据 ---
    const auto &towerChoices = m_visualMap.getAvailableTowers();
    if (towerChoices.empty())
    {
        QMessageBox::information(this, tr("No Towers"), tr("No tower types are defined in this level."));
        return;
    }

    // 确保 m_cachedPlayer 是最新的
    if (!m_cachedPlayer) {
         m_cachedPlayer = resolvePlayer();
    }
    int currentResources = m_cachedPlayer ? m_cachedPlayer->getResource() : 0;
    int currentLevel = m_currentLevelIndex;

    QVector<QString> names, pixmaps, prices;
    for (const auto &proto : towerChoices) {
        names.append(proto.name);
        pixmaps.append(proto.pixmapPath);
        prices.append(QString::number(proto.cost));
    }

    // --- 2. 创建 widget_building_list ---
    // (false = 不是升级菜单)
    widget_building_list *buildMenu = new widget_building_list(
        currentLevel,
        currentResources,
        false,
        names,
        pixmaps,
        prices,
        this // <-- 设为 this (MainWindow) 的子控件
    );
    buildMenu->setAttribute(Qt::WA_DeleteOnClose); // 关键：关闭时自动删除

    // --- 3. 【Bug 修复核心】使用 Lambda 捕获 baseIndex ---
    connect(buildMenu, &widget_building_list::buy, this,
        [this, baseIndex, towerChoices](int towerTypeIndex) { // <-- 捕获 baseIndex

        if (towerTypeIndex < 0 || (size_t)towerTypeIndex >= towerChoices.size()) return;

        // 从 towerChoices 获取被点击的塔的 "type"
        QString towerType = towerChoices[towerTypeIndex].type;

        // 使用被捕获的 baseIndex 获取正确的坐标
        const QPointF relativePos = m_towerBases[baseIndex].relativePosition;

        // 发送建造请求
        GameManager::instance()->buildTower(towerType, relativePos);

        // 立即更新塔基状态
        QTimer::singleShot(0, this, [this, baseIndex]() {
            updateSingleBaseState(baseIndex);
        });
    });

    // --- 4. 显示菜单 ---
    // (将其显示为 "Application Modal" 模态窗口，会阻止点击其他地方)
    buildMenu->setWindowModality(Qt::ApplicationModal);
    buildMenu->show();

    // 居中显示
    int x = (this->width() - buildMenu->width()) / 2;
    int y = (this->height() - buildMenu->height()) / 2;
    buildMenu->move(x, y);
}

Tower* MainWindow::findTowerAtBase(int baseIndex) const
{
    if (baseIndex < 0 || baseIndex >= m_towerBases.size()) return nullptr;

    // (这里的代码就是之前 lambda 的内部代码，现在它们可以合法访问成员了)
    const QPointF center = toAbsolutePosition(m_towerBases[baseIndex].relativePosition);
    const QRectF area(center.x() - m_baseRadius, center.y() - m_baseRadius, m_baseRadius * 2.0, m_baseRadius * 2.0);

    const QList<QGraphicsItem *> items = m_scene->items(area);
    for (QGraphicsItem *item : items) {
        if (auto tower = dynamic_cast<Tower *>(item)) {
            return tower; // 找到了
        }
    }
    return nullptr; // 没找到
}

// 在 MainWindow.cpp 中
// 用此函数替换掉您原来的 showUpgradeMenu 函数

void MainWindow::showUpgradeMenu(int baseIndex, const QPoint &globalPos)
{
    Tower* tower = findTowerAtBase(baseIndex);
    if (!tower) {
        qWarning() << "showUpgradeMenu: Could not find tower at base index" << baseIndex;
        return;
    }
    const QPointF relativePos = m_towerBases[baseIndex].relativePosition;

    // --- 1. 准备数据 ---
    QVector<QString> names;
    QVector<QString> pixmaps;
    QVector<QString> prices;

    // --- 步骤 1.1：(新) 提前加载 tower_data.json ---
    // (我们把这段代码移到了 "if (canUpgrade)" 之前，以便 "出售" 选项也能使用它)
    QJsonObject towerData;
    QFile file(":/data/tower_data.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open tower_data.json for upgrade info";
    } else {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        QJsonArray masterTowers = doc.object()["master_towers"].toArray();
        for (const QJsonValue& val : masterTowers) {
            if (val.toObject()["type"].toString() == tower->getType()) {
                towerData = val.toObject();
                break;
            }
        }
    }
    // --- towerData 现在已加载 ---

    bool canUpgrade = !tower->IsUpgraded();
    int upgradeIndex = -1; // 用于信号槽连接
    int sellIndex = -1;    // 用于信号槽连接

    // 2a. 添加 "升级" 选项 (如果可以)
    if (canUpgrade && !towerData.isEmpty()) {
        names.append("升级: " + towerData["name"].toString());
        pixmaps.append(towerData["pixmap_upgrade"].toString());
        prices.append(QString::number(towerData["upgrade_cost"].toInt()));
        upgradeIndex = names.size() - 1; // 升级选项的索引 (现在是 0)
    }

    // 2b. 添加 "出售" 选项
    sellIndex = names.size(); // 索引 (0 或 1)
    int sellPrice = static_cast<int>(tower->getCost() * 0.7);
    QString selltext = QString("出售：%1").arg(towerData["name"].toString());
    names.append(selltext);

    // --- 步骤 1.2：(新) 修改“出售”图标路径 ---
    // (我们使用 towerData 中的基础 "pixmap" 路径)
    if (!towerData.isEmpty()) {
        pixmaps.append(towerData["pixmap"].toString());
    } else {
        pixmaps.append(":/button/resources/button/lose_repeat.png"); // (保留一个备用)
    }
    // --- 修改结束 ---

    prices.append(QString("+%1").arg(QString::number(sellPrice))); // 显示将返还的资源

    // 2c. 获取玩家当前资源和关卡
    if (!m_cachedPlayer) { m_cachedPlayer = resolvePlayer(); }
    int currentResources = m_cachedPlayer ? m_cachedPlayer->getResource() : 0;
    int currentLevel = m_currentLevelIndex;

    // --- 3. 创建 widget_building_list (使用建造模式) ---
    widget_building_list *upgradeSellMenu = new widget_building_list(
        currentLevel,
        currentResources,
        false, // <-- 关键：使用 false (建造模式)，这样它会显示列表
        names,
        pixmaps,
        prices,
        this
    );
    upgradeSellMenu->setAttribute(Qt::WA_DeleteOnClose);

    // --- 4. 连接信号槽 ---
    connect(upgradeSellMenu, &widget_building_list::buy, this,
        // 使用 Lambda 捕获关键变量
        [this, relativePos, upgradeIndex, sellIndex, baseIndex](int type) {

        if (type == upgradeIndex) {
            // "升级" 被点击
            emit towerUpgradeRequested(relativePos);

        } else if (type == sellIndex) {
            // "出售" 被点击
            emit towerSellRequested(relativePos);
            QTimer::singleShot(0, this, [this, baseIndex](){
                updateSingleBaseState(baseIndex);
            });
        }
    });

    // --- 5. 显示美化后的菜单 ---
    upgradeSellMenu->setWindowModality(Qt::ApplicationModal); // 设置为模态
    upgradeSellMenu->show();
    // 居中显示
    int x = (this->width() - upgradeSellMenu->width()) / 2;
    int y = (this->height() - upgradeSellMenu->height()) / 2;
    upgradeSellMenu->move(x, y);
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

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (!event)
    {
        QMainWindow::keyPressEvent(event);
        return;
    }

    const int key = event->key();
    const Qt::KeyboardModifiers mods = event->modifiers();
    const bool noModifier = (mods == Qt::NoModifier);

    if (noModifier && key >= Qt::Key_1 && key <= Qt::Key_9)
    {
        const int targetIndex = key - Qt::Key_1;
        if (loadLevelByIndex(targetIndex, true))
        {
            event->accept();
            return;
        }
    }

    if (noModifier && key == Qt::Key_BracketLeft)
    {
        cycleLevel(-1);
        event->accept();
        return;
    }

    if (noModifier && key == Qt::Key_BracketRight)
    {
        cycleLevel(1);
        event->accept();
        return;
    }

    if (noModifier && key == Qt::Key_M)
    {
        emit levelSelectionRequested();
        event->accept();
        return;
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    QTimer::singleShot(0, this, [this]()
                       { fitViewToScene(); });

    QTimer::singleShot(0, this, &MainWindow::positionHudWidget);

    if (m_postGameWidget && m_postGameWidget->isVisible())
    {
        int childWidth = m_postGameWidget->width();
        int childHeight = m_postGameWidget->height();
        int x = (this->width() - childWidth) / 2;
        int y = (this->height() - childHeight) / 2;
        m_postGameWidget->move(x, y);
    }

    generateBlurredBackground();
}

QVector<double> MainWindow::buildAxisCoordinates(double offset, double spacing) const
{
    QVector<double> coords;
    if (spacing <= 1e-6)
    {
        coords.append(0.5);
        return coords;
    }

    auto appendIfValid = [&](double value)
    {
        if (value < -spacing || value > 1.0 + spacing)
        {
            return;
        }
        coords.append(value);
    };

    for (double value = offset; value <= 1.0 + spacing; value += spacing)
    {
        appendIfValid(value);
    }
    for (double value = offset - spacing; value >= -spacing; value -= spacing)
    {
        appendIfValid(value);
    }

    if (coords.isEmpty())
    {
        coords.append(0.5);
    }

    std::sort(coords.begin(), coords.end());
    auto almostEqual = [](double lhs, double rhs)
    {
        return std::abs(lhs - rhs) < 1e-4;
    };
    coords.erase(std::unique(coords.begin(), coords.end(), almostEqual), coords.end());
    return coords;
}

double MainWindow::estimateGridOffset(bool horizontal, double spacing) const
{
    if (spacing <= 1e-6)
    {
        return spacing * 0.5;
    }

    const auto &pathData = m_visualMap.getPath();
    if (pathData.empty())
    {
        return spacing * 0.5;
    }

    double sum = 0.0;
    int count = 0;
    for (const QPointF &pt : pathData)
    {
        const double value = horizontal ? pt.x() : pt.y();
        double ratio = value / spacing;
        double frac = ratio - std::floor(ratio);
        if (frac < 0.0)
        {
            frac += 1.0;
        }
        sum += frac * spacing;
        ++count;
    }

    if (count == 0)
    {
        return spacing * 0.5;
    }

    double offset = sum / static_cast<double>(count);
    offset = std::fmod(offset + spacing, spacing);
    if (offset < 1e-5)
    {
        offset = 0.0;
    }
    return offset;
}

bool MainWindow::rectIntersectsAny(const QVector<QRectF> &rects, const QRectF &candidate, qreal tolerance) const
{
    if (candidate.isNull())
    {
        return false;
    }
    QRectF expanded = candidate.adjusted(-tolerance, -tolerance, tolerance, tolerance);
    for (const QRectF &rect : rects)
    {
        if (rect.intersects(expanded))
        {
            return true;
        }
    }
    return false;
}

void MainWindow::spawnBaseItem(int index)
{
    if (!m_scene || index < 0 || index >= m_towerBases.size())
    {
        return;
    }

    TowerBaseVisual &base = m_towerBases[index];
    if (base.graphicsItem || base.blockedByObstacle)
    {
        return;
    }

    auto handler = [this](int idx, const QPointF &pos)
    {
        onTowerBaseClicked(idx, pos);
    };
    auto *item = new TowerBaseItem(index, m_baseRadius, toAbsolutePosition(base.relativePosition), handler);
    m_scene->addItem(item);
    base.graphicsItem = item;
    updateSingleBaseState(index);
}

void MainWindow::updateBaseAvailability()
{
    if (!m_scene)
    {
        return;
    }
    const qreal intersectionTolerance = cellIntersectionTolerance();
    for (int i = 0; i < m_towerBases.size(); ++i)
    {
        TowerBaseVisual &base = m_towerBases[i];
        const bool blocked = rectIntersectsAny(m_obstacleRects, base.cellRect, intersectionTolerance);
        if (blocked == base.blockedByObstacle)
        {
            continue;
        }
        base.blockedByObstacle = blocked;
        if (blocked)
        {
            if (base.graphicsItem)
            {
                m_scene->removeItem(base.graphicsItem);
                delete base.graphicsItem;
                base.graphicsItem = nullptr;
            }
        }
        else
        {
            spawnBaseItem(i);
        }
    }
}

QRectF MainWindow::relativeRectToSceneRect(const QRectF &relativeRect) const
{
    if (!m_scene)
    {
        return {};
    }
    const QRectF sceneRect = m_scene->sceneRect();
    return QRectF(sceneRect.left() + relativeRect.left() * sceneRect.width(),
                  sceneRect.top() + relativeRect.top() * sceneRect.height(),
                  relativeRect.width() * sceneRect.width(),
                  relativeRect.height() * sceneRect.height());
}

qreal MainWindow::cellIntersectionTolerance() const
{
    if (m_cellSize.width() > 0.0 && m_cellSize.height() > 0.0)
    {
        return std::min(m_cellSize.width(), m_cellSize.height()) * 0.15;
    }
    if (m_scene)
    {
        const QSizeF size = m_scene->sceneRect().size();
        if (!size.isEmpty())
        {
            return std::min(size.width(), size.height()) * 0.01;
        }
    }
    return 5.0;
}

void MainWindow::cacheHudSubControls()
{
    if (!m_hudWidget)
    {
        m_hudProgressLabel.clear();
        m_hudProgressBar.clear();
        m_hudStabilityLabel.clear();
        m_hudStabilityBar.clear();
        return;
    }
    m_hudProgressLabel = m_hudWidget->findChild<QLabel *>(QStringLiteral("progress"));
    m_hudProgressBar = m_hudWidget->findChild<QProgressBar *>(QStringLiteral("progressbar"));
    m_hudStabilityLabel = m_hudWidget->findChild<QLabel *>(QStringLiteral("stability_value"));
    m_hudStabilityBar = m_hudWidget->findChild<QProgressBar *>(QStringLiteral("stability_progressbar"));
}

void MainWindow::updateHudStability(int value)
{
    if (!m_hudWidget)
    {
        return;
    }
    if (m_hudStabilityLabel)
    {
        m_hudStabilityLabel->setText(QString::number(std::max(0, value)));
    }
    if (m_hudStabilityBar)
    {
        const int baseline = (m_initialStability > 0) ? m_initialStability : std::max(1, std::max(value, 1));
        const double ratio = std::clamp(static_cast<double>(std::max(0, value)) / static_cast<double>(baseline), 0.0, 1.0);
        m_hudStabilityBar->setValue(static_cast<int>(std::round(ratio * 100.0)));
    }
}

void MainWindow::onObstacleAreaCleared(const QRectF &relativeRect)
{
    if (!m_scene)
    {
        return;
    }
    const QRectF absoluteRect = relativeRectToSceneRect(relativeRect);
    for (int i = 0; i < m_obstacleRects.size();)
    {
        if (absoluteRect.intersects(m_obstacleRects[i]))
        {
            m_obstacleRects.removeAt(i);
        }
        else
        {
            ++i;
        }
    }
    updateBaseAvailability();
}

void MainWindow::ensureHudWidget()
{
    if (m_hudWidget)
    {
        return;
    }
    m_hudWidget = new widget_ingame(m_currentLevelIndex, m_view->viewport());
    m_hudWidget->hide();
    m_hudDesignSize = m_hudWidget->size();
    if (m_hudDesignSize.isEmpty())
    {
        m_hudDesignSize = QSize(800, 600);
    }
    cacheHudSubControls();
    connect(m_hudWidget, &widget_ingame::pause, this, &MainWindow::onHudPauseRequested);
    connect(m_hudWidget, &widget_ingame::begin, this, &MainWindow::onHudResumeRequested);
    connect(m_hudWidget, &widget_ingame::speed_up, this, &MainWindow::onHudSpeedUpRequested);
    connect(m_hudWidget, &widget_ingame::speed_normal, this, &MainWindow::onHudSpeedNormalRequested);
    connect(m_hudWidget, &widget_ingame::pause_menu, this, &MainWindow::onHudMenuClicked);
}

void MainWindow::destroyHudWidget()
{
    if (!m_hudWidget)
    {
        return;
    }
    m_hudWidget->hide();
    delete m_hudWidget;
    m_hudWidget = nullptr;
    m_hudProgressLabel.clear();
    m_hudProgressBar.clear();
    m_hudStabilityLabel.clear();
    m_hudStabilityBar.clear();
    m_initialStability = 0;
}

void MainWindow::positionHudWidget()
{
    if (m_hudWidget && m_view && m_view->viewport())
    {
        // 1. 让 HUD 控件的大小与其父控件 (viewport) 保持一致
        m_hudWidget->setGeometry(m_view->viewport()->rect());

        // 2. 确保 HUD 仍然显示在最上层
        m_hudWidget->raise();
    }
}

void MainWindow::connectHudToSystems()
{
    Player *player = resolvePlayer();
    if (player != m_cachedPlayer)
    {
        if (m_cachedPlayer)
        {
            disconnect(m_cachedPlayer, nullptr, this, nullptr);
        }
        m_cachedPlayer = player;
        if (player)
        {
            connect(player, &Player::resourceChanged, this, &MainWindow::onPlayerResourceChanged);
            connect(player, &Player::stabilityChanged, this, &MainWindow::onPlayerStabilityChanged);
            m_initialStability = player->getStability();
            updateHudStability(m_initialStability);
        }
    }

    WaveManager *waveManager = resolveWaveManager();
    if (waveManager != m_cachedWaveManager)
    {
        disconnectWaveSignals();
        m_cachedWaveManager = waveManager;
        if (waveManager)
        {
            m_waveSpawnConnection = connect(waveManager, &WaveManager::spawnEnemy, this, &MainWindow::onWaveEnemySpawned);
            m_waveCompletionConnection = connect(waveManager, &WaveManager::allWavesCompleted, this, &MainWindow::onAllWavesCompleted);
        }
    }

    locateGameLoopTimer();
}

void MainWindow::disconnectWaveSignals()
{
    if (m_waveSpawnConnection)
    {
        disconnect(m_waveSpawnConnection);
        m_waveSpawnConnection = QMetaObject::Connection();
    }
    if (m_waveCompletionConnection)
    {
        disconnect(m_waveCompletionConnection);
        m_waveCompletionConnection = QMetaObject::Connection();
    }
}

void MainWindow::prepareWaveTrackingFromJson(const QJsonObject &levelRoot)
{
    m_waveEnemyTotals.clear();
    m_totalEnemyCount = 0;
    m_spawnedEnemyCount = 0;
    m_currentWaveCounter = 0;
    m_spawnedInCurrentWave = 0;

    const QJsonArray wavesArray = levelRoot.value(QStringLiteral("waves")).toArray();
    m_waveEnemyTotals.reserve(wavesArray.size());
    for (const QJsonValue &waveValue : wavesArray)
    {
        const QJsonObject waveObj = waveValue.toObject();
        int waveCount = 0;
        const QJsonArray enemyList = waveObj.value(QStringLiteral("enemies")).toArray();
        for (const QJsonValue &enemyValue : enemyList)
        {
            const QJsonObject enemyObj = enemyValue.toObject();
            waveCount += std::max(0, enemyObj.value(QStringLiteral("count")).toInt());
        }
        m_waveEnemyTotals.append(waveCount);
        m_totalEnemyCount += waveCount;
    }

    updateHudProgress();
}

void MainWindow::resetWaveTracking()
{
    m_waveEnemyTotals.clear();
    m_totalEnemyCount = 0;
    m_spawnedEnemyCount = 0;
    m_currentWaveCounter = 0;
    m_spawnedInCurrentWave = 0;
    if (m_hudProgressLabel)
    {
        m_hudProgressLabel->setText(QStringLiteral("0 / 0"));
    }
    if (m_hudProgressBar)
    {
        m_hudProgressBar->setValue(0);
    }
}

void MainWindow::updateHudProgress()
{
    if (!m_hudWidget)
    {
        return;
    }

    const int totalWaves = static_cast<int>(m_waveEnemyTotals.size());
    const int displayedWave = (totalWaves == 0 || m_currentWaveCounter < 0)
                                  ? 0
                                  : std::clamp(m_currentWaveCounter + 1, 1, totalWaves);
    const double progress = (m_totalEnemyCount > 0)
                                ? static_cast<double>(std::min(m_spawnedEnemyCount, m_totalEnemyCount)) / static_cast<double>(m_totalEnemyCount)
                                : 0.0;
    if (m_hudProgressLabel)
    {
        m_hudProgressLabel->setText(QStringLiteral("%1 / %2").arg(QString::number(displayedWave)).arg(QString::number(totalWaves)));
    }
    if (m_hudProgressBar)
    {
        m_hudProgressBar->setValue(static_cast<int>(std::round(std::clamp(progress, 0.0, 1.0) * 100.0)));
    }
}

void MainWindow::locateGameLoopTimer()
{
    if (m_gameLoopTimer)
    {
        return;
    }
    GameManager *manager = GameManager::instance();
    if (!manager)
    {
        return;
    }
    m_gameLoopTimer = manager->findChild<QTimer *>(QString(), Qt::FindDirectChildrenOnly);
}

Player *MainWindow::resolvePlayer() const
{
    GameManager *manager = GameManager::instance();
    return manager ? manager->findChild<Player *>() : nullptr;
}

WaveManager *MainWindow::resolveWaveManager() const
{
    GameManager *manager = GameManager::instance();
    return manager ? manager->findChild<WaveManager *>() : nullptr;
}

void MainWindow::applyGameSpeed(bool fastMode)
{
    locateGameLoopTimer();
    if (!m_gameLoopTimer)
    {
        return;
    }
    const int targetInterval = fastMode ? 8 : 16;
    m_gameLoopTimer->setInterval(targetInterval);
}

void MainWindow::onHudPauseRequested()
{
    GameManager::instance()->pauseGame();
    if (m_pauseMenuWidget) {
        m_pauseMenuWidget->close(); // 我们来关闭它
    }
}

void MainWindow::onHudResumeRequested()
{
    if (m_pauseMenuWidget) {
        m_pauseMenuWidget->close();
        // m_pauseMenuWidget->close() 会触发 destroyed() 信号，
        // 进而调用 onPauseMenuClosed() 来清理视图效果。
    }
    GameManager::instance()->resumeGame();
    applyGameSpeed(m_fastModeActive);
}

void MainWindow::onHudSpeedUpRequested()
{
    m_fastModeActive = true;
    applyGameSpeed(true);
}

void MainWindow::onHudSpeedNormalRequested()
{
    m_fastModeActive = false;
    applyGameSpeed(false);
}

void MainWindow::onPlayerResourceChanged(int value)
{
    if (m_hudWidget)
    {
        m_hudWidget->set_resource_value(value);
    }
}

void MainWindow::onPlayerStabilityChanged(int value)
{
    if (value > 0 && (m_initialStability <= 0 || value > m_initialStability))
    {
        m_initialStability = value;
    }
    updateHudStability(value);
}

void MainWindow::onWaveEnemySpawned()
{
    if (m_totalEnemyCount > 0)
    {
        m_spawnedEnemyCount = std::min(m_spawnedEnemyCount + 1, m_totalEnemyCount);
    }
    else
    {
        ++m_spawnedEnemyCount;
    }

    if (!m_waveEnemyTotals.isEmpty())
    {
        const int waveCount = static_cast<int>(m_waveEnemyTotals.size());
        if (m_currentWaveCounter < 0)
        {
            m_currentWaveCounter = 0;
        }
        if (m_currentWaveCounter >= waveCount)
        {
            m_currentWaveCounter = waveCount - 1;
        }

        ++m_spawnedInCurrentWave;
        const int currentWaveTotal = m_waveEnemyTotals[m_currentWaveCounter];
        if (currentWaveTotal > 0 && m_spawnedInCurrentWave >= currentWaveTotal && m_currentWaveCounter + 1 < waveCount)
        {
            ++m_currentWaveCounter;
            m_spawnedInCurrentWave = 0;
        }
    }

    updateHudProgress();
}

void MainWindow::onAllWavesCompleted()
{
    if (m_totalEnemyCount > 0)
    {
        m_spawnedEnemyCount = m_totalEnemyCount;
    }
    if (!m_waveEnemyTotals.isEmpty())
    {
        m_currentWaveCounter = static_cast<int>(m_waveEnemyTotals.size()) - 1;
    }
    updateHudProgress();
}

void MainWindow::onHudMenuClicked()
{
    // 1. 暂停游戏逻辑
    GameManager::instance()->pauseGame();

    // 2. 灰化并禁用游戏视图
    // (使用 QGraphicsOpacityEffect 是最简单的方法)
    QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect(m_view);
    opacityEffect->setOpacity(0.5); // 30% 不透明度
    m_view->setGraphicsEffect(opacityEffect);
    m_view->setEnabled(false); // 禁用视图，防止点击穿透

    if (m_hudWidget) {
        m_hudWidget->setEnabled(false);
    }

    // 3. 创建并显示暂停菜单
    if (!m_pauseMenuWidget) {
        m_pauseMenuWidget = new widget_pause_menu(this); // 父窗口设为 MainWindow
        m_pauseMenuWidget->setAttribute(Qt::WA_DeleteOnClose);
        m_pauseMenuWidget->setWindowModality(Qt::ApplicationModal);

        // 4. 连接暂停菜单的信号

        // "返回游戏" -> 恢复游戏 (我们已经修改了 onHudResumeRequested)
        connect(m_pauseMenuWidget, &widget_pause_menu::back_to_game, this, &MainWindow::onHudResumeRequested);

        // "返回主菜单" -> 触发 onReturnToMainMenu
        connect(m_pauseMenuWidget, &widget_pause_menu::back_to_menu, this, &MainWindow::onReturnToMainMenu);

        // (关键) 当菜单被关闭时，自动调用 onPauseMenuClosed 来清理效果
        connect(m_pauseMenuWidget, &QWidget::destroyed, this, &MainWindow::onPauseMenuClosed);
    }

    // 5. 居中显示菜单
    int childWidth = m_pauseMenuWidget->width();
    int childHeight = m_pauseMenuWidget->height();
    int x = (this->width() - childWidth) / 2;
    int y = (this->height() - childHeight) / 2;
    m_pauseMenuWidget->move(x, y);

    m_pauseMenuWidget->show();
    m_pauseMenuWidget->raise();
}

void MainWindow::onReturnToMainMenu()
{
    // 1. (重要) 先关闭暂停菜单
    if (m_pauseMenuWidget) {
        m_pauseMenuWidget->close();
        // close() 会触发 destroyed，进而调用 onPauseMenuClosed() 来清理效果
    }

    // 2. 恢复游戏逻辑（以便下次进入时是正常状态）
    GameManager::instance()->clearGameScene();

    // 3. 隐藏游戏主窗口
    this->hide();

    // 4. 发出信号，通知 main.cpp 显示主菜单
    emit mainMenuRequested();
}

void MainWindow::onPauseMenuClosed()
{
    // 1. 移除并删除 QGraphicsView 上的效果
    if (m_view->graphicsEffect()) {
        delete m_view->graphicsEffect();
        m_view->setGraphicsEffect(nullptr);
    }

    // 2. 重新启用游戏视图
    m_view->setEnabled(true);

    if (m_hudWidget) {
        m_hudWidget->setEnabled(true);
    }

    // 3. 重置指针 (因为 m_pauseMenuWidget 设置了 WA_DeleteOnClose)
    m_pauseMenuWidget = nullptr;
}

void MainWindow::showEvent(QShowEvent *event)
{
    // 1. 调用父类的实现
    QMainWindow::showEvent(event);

    // 2. 使用 QTimer::singleShot(0, ...) 延迟调用。
    //    这能确保在首次显示时，视口（viewport）的尺寸已经计算完毕。
    QTimer::singleShot(0, this, &MainWindow::positionHudWidget);
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (!m_blurredBg.isNull())
    {
        // 绘制我们生成的、铺满全屏的模糊背景
        painter.drawPixmap(this->rect(), m_blurredBg);
    }
    else
    {
        // 如果图片加载失败，回退到纯黑背景
        painter.fillRect(this->rect(), Qt::black);
        QMainWindow::paintEvent(event);
    }
}

void MainWindow::generateBlurredBackground()
{
    if (m_rawBg.isNull() || m_blurRadius <= 0 || this->size().isEmpty()) {
        return;
    }

    // 1. 将原始图像缩放到当前窗口大小（高质量缩放）
    QPixmap scaledRaw = m_rawBg.scaled(this->size(),
                                       Qt::KeepAspectRatioByExpanding, // 保证填满，可能会裁剪
                                       Qt::SmoothTransformation);

    // 2. 使用 QGraphicsEffect 来施加模糊
    QGraphicsScene scene;
    QGraphicsPixmapItem item(scaledRaw);

    QGraphicsBlurEffect blurEffect; // 在栈上创建
    blurEffect.setBlurRadius(m_blurRadius); // 使用成员变量设置
    item.setGraphicsEffect(&blurEffect); // 传递栈对象的地址

    scene.addItem(&item);

    // 3. 将模糊后的场景渲染到 QImage
    QImage image(scaledRaw.size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);
    QPainter painter(&image);
    scene.render(&painter, QRectF(), scaledRaw.rect());
    painter.end();

    // 4. 保存最终的 QPixmap
    m_blurredBg = QPixmap::fromImage(image);
}