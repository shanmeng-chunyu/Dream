#include "Enemy.h"
#include "LiveCoffee.h"
#include <QGraphicsScene>
#include <QLineF>
#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>
#include <QtMath>
#include <QMovie>


QMap<QString, QPair<QVector<QPixmap>, int>> Enemy::s_staticFrameCache;
Enemy::Enemy(int health, double speed, int damage, const std::vector<QPointF>& path, QString type, const QString& gifPath, const QSize& pixelSize, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsPixmapItem(parent),
      type(type),
      m_health(health),
      m_speed(speed),
      damage(damage),
      absolutePath(path),
      m_currentPathIndex(0),
      m_maxHealth(health),
      m_movie(nullptr),
      m_stunTicksRemainimng(0),
      m_baseSpeed(speed),
      m_isFlipped(false),
      m_pixelSize(pixelSize),
      m_currentFrameIndex(0),
      m_effectItem(nullptr),
      m_effectTicksRemaining(0),
      m_animTickCounter(0),
      m_ticksPerFrame(1)
{
    if (!absolutePath.empty()) {
        setPos(absolutePath[0]);
    }
    QString cacheKey = type;
    if (!s_staticFrameCache.contains(cacheKey)) {

        // --- 缓存未命中 (第一次创建该类型敌人) ---
        // 执行原本繁重的加载逻辑

        QVector<QPixmap> loadedFrames;
        int calculatedTicks = 1;

        QMovie tempMovie(gifPath);
        if (tempMovie.isValid()) {
            tempMovie.setCacheMode(QMovie::CacheAll);
            int frameCount = tempMovie.frameCount();
            if (frameCount <= 0) {
                tempMovie.jumpToFrame(0);
                frameCount = tempMovie.frameCount();
                if (frameCount <= 0) frameCount = 1;
            }

            for (int i = 0; i < frameCount; ++i) {
                tempMovie.jumpToFrame(i);
                QPixmap rawFrame = tempMovie.currentPixmap();
                if (!rawFrame.isNull()) {
                    // 昂贵的缩放操作，全游戏生命周期内只对该类型执行这一次
                    QPixmap scaledFrame = rawFrame.scaled(m_pixelSize,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation);
                    loadedFrames.append(scaledFrame);
                }
            }

            // 计算 ticks
            int gifDelay = tempMovie.nextFrameDelay();
            if (gifDelay <= 0) gifDelay = 100;
            calculatedTicks = gifDelay / 16;
            if (calculatedTicks < 1) calculatedTicks = 1;
        } else {
            qWarning() << "Failed to load GIF for cache:" << gifPath;
        }

        // 将处理好的数据存入全局静态缓存
        s_staticFrameCache.insert(cacheKey, qMakePair(loadedFrames, calculatedTicks));
    }

    // B. 从缓存读取数据 (无论是刚加载的，还是本来就有的)
    // 利用 Qt 的隐式共享 (Implicit Sharing)，这里是浅拷贝，非常快，不占额外内存
    const auto& cachedData = s_staticFrameCache[cacheKey];
    m_frames = cachedData.first;       // 共享图片数据
    m_ticksPerFrame = cachedData.second; // 复制 tick 数值

    // 3. 设置初始图像
    if (!m_frames.isEmpty()) {
        setPixmap(m_frames[0]);
    }
}

void Enemy::setAbsolutePath(const std::vector<QPointF>& path) {
    absolutePath = path;
    m_currentPathIndex = 0;
    if (!absolutePath.empty()) {
        setPos(absolutePath[0]);
    }
}


void Enemy::move() {
    if (m_isDying) return; // <-- 如果正在死亡，则不执行任何移动
    if (m_effectTicksRemaining > 0) {
        m_effectTicksRemaining--;
        if (m_effectTicksRemaining == 0) {
            removeVisualEffect();
        }
    }
    if (m_stunTicksRemainimng > 0) {
        m_stunTicksRemainimng--;
        return;
    }
    applyAuraEffects();
    if (m_currentPathIndex>= absolutePath.size() - 1) {
        emit reachedEnd(this);
        return;
    }

    if (!m_frames.isEmpty()) {
        m_animTickCounter++;

        // 如果积累的帧数达到了阈值，就切换图片
        if (m_animTickCounter >= m_ticksPerFrame) {
            m_animTickCounter = 0; // 重置计数器

            // 切换到下一帧
            m_currentFrameIndex = (m_currentFrameIndex + 1) % m_frames.size();
            setPixmap(m_frames[m_currentFrameIndex]);
        }
    }

    QPointF targetPoint = absolutePath[m_currentPathIndex + 1];
    QLineF line(pos(), targetPoint);

    if (line.length() < m_speed) {
        ++m_currentPathIndex;
        setPos(targetPoint);
        if (m_currentPathIndex >= absolutePath.size() - 1) {
            emit reachedEnd(this);
        }
        return;
    }

    double angle = atan2(targetPoint.y() - pos().y(), targetPoint.x() - pos().x());
    double dx = m_speed * cos(angle);
    //反转贴图
    bool shouldBeFlipped = false;

    // 1. 根据类型判断 "期望的" 翻转状态
    if (type == "bug" || type == "bugmini" || type == "loneliness" || type == "regret") {
        // 当向右移动 (dx > 0.01) 时，它们 "应该被翻转"
        shouldBeFlipped = (dx > 0.01);
    }
    else {
        // 假设 "其他" 默认朝右
        // 当向左移动 (dx < -0.01) 时，它们 "应该被翻转"
        shouldBeFlipped = (dx < -0.01);
    }

    // 2. 检查 "期望状态" 是否与 "当前状态" (m_isFlipped) 不符
    if (shouldBeFlipped && !m_isFlipped) {
        // 期望翻转，但当前未翻转 -> 应用翻转
        QTransform t = transform();
        t.scale(-1, 1);
        setTransform(t);
        m_isFlipped = true;
    }
    else if (!shouldBeFlipped && m_isFlipped) {
        // 期望不翻转，但当前已翻转 -> 翻转回来
        QTransform t = transform();
        t.scale(-1, 1); // 再次翻转以抵消
        setTransform(t);
        m_isFlipped = false;
    }
    // --- 翻转逻辑结束 ---
    double dy =m_speed * sin(angle);
    setPos(pos().x() + dx, pos().y() + dy);
}

void Enemy::takeDamage(int damageAmount) {
    if (m_isDying) return; // <-- 新增：死亡时不再承受伤害
    m_health -= damageAmount;
    if (m_health <= 0) {
        m_health = 0;      // 保证血量不为负
        m_isDying = true;         // <-- 设置死亡状态
        playDeathAnimation();   // <-- 播放死亡动画
        emit died(this);
    } else {
        update();          // 受伤刷新血条
    }
}


int Enemy::getDamage() const {
    return damage;
}

void Enemy::stopFor(double duration) {
    m_stunTicksRemainimng = static_cast<int>(duration * 60);
}
int Enemy::getHealth() const {
    return m_health;
}

void Enemy::heal(int amount) {
    if (amount <= 0) return;
    m_health = std::min(m_health + amount, m_maxHealth);
    update();
}

void Enemy::setBaseSpeed(double v) {
    m_baseSpeed = v;
}

int Enemy::getCurrentPathIndex() const {
    return m_currentPathIndex;
}

QList<LiveCoffee*> Enemy::findCoffeeInRange() const
{
    QList<LiveCoffee*> coffeeTowers;
    if (!scene()) return coffeeTowers; // 安全检查

    QList<QGraphicsItem*> items = scene()->items();
    for(auto& item : items)
    {
        LiveCoffee* tower = dynamic_cast<LiveCoffee*>(item);
        if(tower) // 如果它是一个咖啡塔
        {
            QLineF line(pos(), tower->pos());
            // 检查敌人是否在塔的光环范围内
            if(line.length() <= tower->getRange())
                coffeeTowers.append(tower);
        }
    }
    return coffeeTowers;
}

// 【新增】应用所有光环效果
void Enemy::applyAuraEffects()
{
    // 1. 速度重置为基础速度 (可能是原始速度，也可能是Boss狂暴后的速度)
    m_speed = m_baseSpeed;

    // 2. 搜索咖啡塔
    QList<LiveCoffee*> coffees = findCoffeeInRange();

    // 3. 叠乘所有减速效果
    for(LiveCoffee* coffee : coffees)
    {
        m_speed *= coffee->getEnemyDebuffFactor();
    }
    // (未来还可以添加其他光环, e.g., m_speed *= findOtherDebuff())
}


void Enemy::paint(QPainter* painter,
                  const QStyleOptionGraphicsItem* option,
                  QWidget* widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);

    if (m_isDying || m_maxHealth <= 0 || m_health <= 0) return;

    qreal ratio = (qreal)m_health / (qreal)m_maxHealth;
    if (ratio < 0) ratio = 0;
    if (ratio > 1) ratio = 1;

    // 关键：计算敌人贴图的“实际显示宽度”
    qreal displayW = pixmap().width() * this->scale();
    qreal displayH = pixmap().height() * this->scale();

    if (displayW <= 1) displayW = 30;   // 避免意外情况

    // 血条宽度 = 显示宽度的 60%，并限制最大 40
    qreal barW = qMin(displayW * 0.6, 40.0);
    qreal barH = 3.0;

    // 显示坐标系里贴图左上角的位置 = (-displayW/2, -displayH/2)
    qreal offsetX = -displayW / 2.0;
    qreal offsetY = -displayH / 2.0;

    // 血条位置：怪物头顶
    qreal x = offsetX + (displayW - barW) / 2.0;
    qreal y = offsetY - barH - 2.0;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    // 背景条
    painter->setBrush(QColor(0,0,0,150));
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(QRectF(x, y, barW, barH), 2, 2);

    // 前景条
    QColor front =
        ratio >= 0.6 ? QColor(76,175,80) :
            ratio >= 0.3 ? QColor(255,193,7) :
            QColor(244,67,54);

    painter->setBrush(front);
    painter->drawRoundedRect(QRectF(x, y, barW * ratio, barH), 2, 2);

    painter->restore();
}

void Enemy::updatePixmapFromMovie()
{
    // 关键：从 QMovie 获取当前帧，并 *缩放* 到我们期望的尺寸
    QPixmap scaledFrame = m_movie->currentPixmap().scaled(m_pixelSize,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation);

    // 将 QGraphicsPixmapItem 的贴图设置为这一帧
    setPixmap(scaledFrame);
}

void Enemy::applyVisualEffect(const QPixmap& pixmap, double duration)
{
    // 1. 如果当前已经有一个特效，先移除它
    if (m_effectItem) {
        removeVisualEffect();
    }

    // (移除 QTimer->stop() 的调用)
    const double scaleFactor = 0.7; // 特效大小是敌人大小的 80%
    const QSize targetSize(m_pixelSize.width() * scaleFactor,
                           m_pixelSize.height() * scaleFactor);
    QPixmap scaledPixmap = pixmap.scaled(targetSize,
                                        Qt::KeepAspectRatio, // 保持宽高比
                                        Qt::SmoothTransformation); // 平滑缩放

    // 3. 使用【缩放后】的 pixmap 创建特效
    m_effectItem = new QGraphicsPixmapItem(scaledPixmap, this);

    // 3. 计算特效的位置（使其在敌人贴图上居中）
    QPointF enemyCenter(0.0, -30);
    QPointF effectTopLeft(
        enemyCenter.x() - scaledPixmap.width() / 2.0,  // <-- 已修正
        enemyCenter.y() - scaledPixmap.height() / 2.0  // <-- 已修正
    );

    m_effectItem->setPos(effectTopLeft);
    m_effectItem->setZValue(1.0);
    m_effectItem->setOpacity(0.97);

    // 4. 【核心修改】将 duration 转换为 Ticks (帧数)
    //    我们参考 WaveManager::intervalToTicks 的实现
    //    它使用 16ms 作为一帧
    const double gameTickIntervalMs = 16.0;
    m_effectTicksRemaining = static_cast<int>(std::round((duration * 1000.0) / gameTickIntervalMs));
}

// --- 【修改】实现 removeVisualEffect (现在是私有函数) ---
void Enemy::removeVisualEffect()
{
    if (m_effectItem) {
        delete m_effectItem;
        m_effectItem = nullptr;
    }
    m_effectTicksRemaining = 0; // 确保计数器归零
}

void Enemy::pauseAnimation()
{

    // 2. 暂停死亡动画 (通常由 m_movie 驱动)
    // 如果你保留了 playDeathAnimation 中的 QMovie 逻辑，这里需要暂停它
    if (m_movie && m_movie->state() == QMovie::Running) {
        m_movie->setPaused(true);
    }
}

void Enemy::resumeAnimation()
{

    // 2. 恢复死亡动画
    // 如果 m_movie 处于暂停状态（说明之前正在播死亡动画被暂停了），则恢复它
    if (m_movie && m_movie->state() == QMovie::Paused) {
        m_movie->setPaused(false);
    }
}

void Enemy::playDeathAnimation()
{

    if (!m_movie) {
        // 如果 m_movie 是空的（第一次死），则创建它
        m_movie = new QMovie(this);
        // 只有 m_movie 被创建后，连接信号才安全
        // 注意：checkDeathFrame 的连接应该放在 start 之前
        connect(m_movie, QOverload<int>::of(&QMovie::frameChanged), this, &Enemy::checkDeathFrame);
        connect(m_movie, &QMovie::frameChanged, this, &Enemy::updatePixmapFromMovie);
    } else {
        // 如果 m_movie 已经存在（理论上不会发生，除非复活逻辑），先停止
        m_movie->stop();
        // 断开旧的连接，防止重复连接导致多次调用
        disconnect(m_movie, &QMovie::frameChanged, this, &Enemy::updatePixmapFromMovie);
        // 重新连接（或者你可以只在创建时连接一次，这里为了保险起见重连）
        connect(m_movie, &QMovie::frameChanged, this, &Enemy::updatePixmapFromMovie);
    }

    // 3. 移除之前可能存在的旧连接 (关于 finished 信号)
    // 你的代码里之前有这一行，保留它是好的习惯
    disconnect(m_movie, &QMovie::finished, this, &Enemy::onDeathAnimationFinished);

    // 4. 设置资源并播放
    m_movie->setFileName(":/enemies/resources/enemies/EnemyDie.gif");

    if (!m_movie->isValid()) {
        qWarning() << "Failed to load death animation!";
        // 如果加载失败，直接触发死亡完成逻辑，防止游戏卡住
        onDeathAnimationFinished();
        return;
    }

    m_movie->setCacheMode(QMovie::CacheAll);
    m_movie->setSpeed(100);

    // 开始播放
    m_movie->start();
}

// --- 新增函数 ---
void Enemy::onDeathAnimationFinished()
{
    // 动画播放完毕，通知 GameManager
    emit deathAnimationFinished(this);
}

void Enemy::checkDeathFrame(int frameNumber)
{
    // 检查电影是否存在，以及当前帧是否是最后一帧
    if (m_movie && frameNumber == m_movie->frameCount() - 1)
    {
        // 1. 立即停止电影，防止它循环
        m_movie->stop();

        // 2. 断开这个槽，防止它被意外再次触发
        disconnect(m_movie, QOverload<int>::of(&QMovie::frameChanged), this, &Enemy::checkDeathFrame);

        // 3. 手动调用你现有的“已完成”逻辑
        onDeathAnimationFinished();
    }
}