#include "Bullet.h"
#include "Enemy.h"
#include "Obstacle.h"
#include <QLineF>
#include <QtMath>
#include <QGraphicsScene>

Bullet::Bullet(int damage, double speed, QGraphicsPixmapItem* target, const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsPixmapItem(pixmap, parent),
      damage(damage),
      speed(speed),
      target(target) {

    setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
}

void Bullet::update() {
    // 每帧移动一次
    if (moveCounter-- > 0) {
        return;
    }
    moveCounter = 1;

    if (!target || !scene()->items().contains(target)) {
        // 目标不存在，触发击中事件
        emit hitTarget(this);
        return;
    }

    QLineF line(pos(), target->pos());
    if (line.length() < speed) {
        //子弹击中目标
        emit hitTarget(this);
        return;
    }

    // 子弹移动的角度和偏移量
    double angle = atan2(target->pos().y() - pos().y(), target->pos().x() - pos().x());
    double dx = speed * cos(angle);
    double dy = speed * sin(angle);
    setPos(pos().x() + dx, pos().y() + dy);

    if (!scene()->sceneRect().contains(target->pos())) {
        emit outOfBounds(this);
    }
}

int Bullet::getDamage() const {
    return damage;
}

QGraphicsPixmapItem* Bullet::getTarget() const {
    return target;
}
