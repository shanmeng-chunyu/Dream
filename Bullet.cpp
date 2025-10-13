#include "Bullet.h"
#include "Enemy.h"
#include <QLineF>
#include <QtMath>

Bullet::Bullet(int damage, double speed, Enemy* target, const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsPixmapItem(pixmap, parent),
      damage(damage),
      speed(speed),
      target(target) {}

void Bullet::move() {
    if (!target) {
        // Ŀ�겻���ڣ���������
        emit hitTarget(this);
        return;
    }

    QLineF line(pos(), target->pos());
    if (line.length() < speed) {
        // ����Ŀ��
        emit hitTarget(this);
        return;
    }

    // ��Ŀ���ƶ�
    double angle = atan2(target->pos().y() - pos().y(), target->pos().x() - pos().x());
    double dx = speed * cos(angle);
    double dy = speed * sin(angle);
    setPos(pos().x() + dx, pos().y() + dy);
}

int Bullet::getDamage() const {
    return damage;
}

Enemy* Bullet::getTarget() const {
    return target;
}
