#include "Enemy.h"
#include "LiveCoffee.h"
#include <QGraphicsScene>
#include <QLineF>
#include <QDebug>
#include <QGraphicsScene>

Enemy::Enemy(int health, double speed, int damage,const std::vector<QPointF>& path,QString type, const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsPixmapItem(pixmap, parent),
      type(type),
      m_health(health),
      m_speed(speed),
      damage(damage),
      absolutePath(path),
      m_currentPathIndex(0),
      m_maxHealth(health),
      m_stunTicksRemainimng(0),
      m_baseSpeed(speed),
      m_isFlipped(false)
{
    if (!absolutePath.empty()) {
        setPos(absolutePath[0]);
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
    m_health -= damageAmount;
    if (m_health <= 0) {
        emit died(this);
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