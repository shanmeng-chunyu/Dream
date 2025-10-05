#include "Enemy.h"
#include <QPixmap>
#include <QLineF>
#include <qmath.h>

Enemy::Enemy(const std::vector<QPointF> &path, QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsPixmapItem(parent), m_health(100), m_speed(2), m_path(path), m_pathIndex(0) {
    // 设置敌人图像 (这里用一个简单的颜色块代替)
    QPixmap pixmap(40, 40);
    pixmap.fill(Qt::red);
    setPixmap(pixmap);

    if (!m_path.empty()) {
        setPos(m_path[0]);
    }
}

void Enemy::takeDamage(int damage) {
    m_health -= damage;
    if (m_health <= 0) {
        emit died(this);
    }
}

void Enemy::advance(int phase) {
    if (!phase) return; // 在第一阶段不做任何事

    if (m_pathIndex >= m_path.size() - 1) {
        emit reachedEnd(this);
        return;
    }

    QPointF target = m_path[m_pathIndex + 1];
    QLineF line(pos(), target);

    if (line.length() < m_speed) {
        m_pathIndex++;
        if (m_pathIndex >= m_path.size()) {
            // 已到达路径末端
            return;
        }
    }

    // 移动敌人
    double angle = qAtan2(target.y() - y(), target.x() - x());
    double dx = m_speed * qCos(angle);
    double dy = m_speed * qSin(angle);

    setPos(x() + dx, y() + dy);
}
