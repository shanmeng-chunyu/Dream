#include "Bullet.h"
#include "Enemy.h"
#include <QLineF>
#include <qmath.h>
#include "GameManager.h"

Bullet::Bullet(Enemy *target, int damage, QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsPixmapItem(parent), m_target(target), m_damage(damage), m_speed(10) {
    // �����ӵ�ͼ�� (������һ���򵥵���ɫ�����)
    QPixmap pixmap(10, 10);
    pixmap.fill(Qt::yellow);
    setPixmap(pixmap);
}

void Bullet::advance(int phase) {
    if (!phase) return;

    if (!m_target || !scene() || !m_target->scene()) {
        // Ŀ��ʧЧ
        GameManager::instance()->removeBullet(this);
        return;
    }

    QLineF line(pos(), m_target->pos());
    if (line.length() < m_speed) {
        m_target->takeDamage(m_damage);
        emit hitTarget(this);
        return;
    }

    // �ƶ��ӵ�
    double angle = qAtan2(m_target->pos().y() - y(), m_target->pos().x() - x());
    double dx = m_speed * qCos(angle);
    double dy = m_speed * qSin(angle);

    setPos(x() + dx, y() + dy);
}
