#include "Enemy.h"
#include <QLineF>
#include <QDebug>

Enemy::Enemy(int health, double speed, int damage,const std::vector<QPointF>& path,const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsPixmapItem(pixmap, parent),
      m_health(health),
      m_speed(speed),
      damage(damage),
      absolutePath(path),
      m_currentPathIndex(0),
      m_stunTicksRemainimng(0)
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

void Enemy::setSpeed(double v) {
    m_speed = v;
}

int Enemy::getCurrentPathIndex() const {
    return m_currentPathIndex;
}
