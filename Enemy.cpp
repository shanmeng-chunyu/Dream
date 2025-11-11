#include "Enemy.h"
#include <QLineF>
#include <QDebug>

Enemy::Enemy(int health, double speed, int damage, const std::vector<QPointF>& path, const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsPixmapItem(pixmap, parent),
      currentHealth(health),
      speed(speed),
      damage(damage),
      absolutePath(path),
      currentPathIndex(0) {
    if (!absolutePath.empty()) {
        setPos(absolutePath[0]);
    }
}

void Enemy::setAbsolutePath(const std::vector<QPointF>& path) {
    absolutePath = path;
    currentPathIndex = 0;
    if (!absolutePath.empty()) {
        setPos(absolutePath[0]);
    }
}


void Enemy::move() {
    if (m_stunTicksRemainimng > 0) {
        m_stunTicksRemainimng--;
        return;
    }
    if (currentPathIndex >= absolutePath.size() - 1) {
        emit reachedEnd(this);
        return;
    }

    QPointF targetPoint = absolutePath[currentPathIndex + 1];
    QLineF line(pos(), targetPoint);

    if (line.length() < speed) {
        currentPathIndex++;
        setPos(targetPoint);
        if (currentPathIndex >= absolutePath.size() - 1) {
            emit reachedEnd(this);
        }
        return;
    }

    double angle = atan2(targetPoint.y() - pos().y(), targetPoint.x() - pos().x());
    double dx = speed * cos(angle);
    double dy = speed * sin(angle);
    setPos(pos().x() + dx, pos().y() + dy);
}

void Enemy::takeDamage(int damageAmount) {
    currentHealth -= damageAmount;
    if (currentHealth <= 0) {
        emit died(this);
    }
}

int Enemy::getDamage() const {
    return damage;
}

void Enemy::stopFor(double duration) {
    m_stunTicksRemainimng = static_cast<int>(duration * 60);
}