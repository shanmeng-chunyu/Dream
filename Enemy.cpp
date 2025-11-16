#include "Enemy.h"
#include "LiveCoffee.h"
#include <QGraphicsScene>
#include <QLineF>
#include <QDebug>
#include <QGraphicsScene>
#include <QPainter>
#include <QtMath>
#include <QMovie>

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
      m_stunTicksRemainimng(0),
      m_baseSpeed(speed),
      m_isFlipped(false),
      m_pixelSize(pixelSize),
      m_effectItem(nullptr),
      m_effectTicksRemaining(0)
{
    if (!absolutePath.empty()) {
        setPos(absolutePath[0]);
    }
    m_movie = new QMovie(gifPath, QByteArray(), this);

    // 推荐：对于大量重复的GIF，开启缓存
    m_movie->setCacheMode(QMovie::CacheAll);

    // 连接 QMovie 的 frameChanged 信号到我们的槽
    connect(m_movie, &QMovie::frameChanged, this, &Enemy::updatePixmapFromMovie);

    m_movie->start();

    // 立即设置第一帧，防止敌人隐形
    if (m_movie->isValid()) {
        updatePixmapFromMovie();
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
    if (type == "bug" || type == "bugmini") {
        // 假设 "bug" 默认朝左
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
    // 检查 m_movie 是否存在，并且正在运行
    if (m_movie && m_movie->state() == QMovie::Running) {
        m_movie->setPaused(true);
    }
}

void Enemy::resumeAnimation()
{
    // 检查 m_movie 是否存在，并且之前是暂停状态
    if (m_movie && m_movie->state() == QMovie::Paused) {
        m_movie->setPaused(false);
    }
}

void Enemy::playDeathAnimation()
{
    if (!m_movie) return;

    // 停止并断开旧的循环动画
    m_movie->stop();
    disconnect(m_movie, &QMovie::frameChanged, this, &Enemy::updatePixmapFromMovie);
    disconnect(m_movie, &QMovie::finished, this, &Enemy::onDeathAnimationFinished); // 断开可能的旧连接

    // 加载死亡动画
    m_movie->setFileName(":/enemies/resources/enemies/EnemyDie.gif");

    // 确保动画只播放一次
    m_movie->setCacheMode(QMovie::CacheAll);
    m_movie->setSpeed(100);

    // 重新连接 frameChanged 以显示动画
    connect(m_movie, &QMovie::frameChanged, this, &Enemy::updatePixmapFromMovie);

    // !!! 核心：连接 movie 的 finished 信号到我们的槽 !!!
    connect(m_movie, QOverload<int>::of(&QMovie::frameChanged), this, &Enemy::checkDeathFrame);

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