#include "Tower.h"
#include "Bullet.h"
#include "Enemy.h"
#include <QLineF>
#include <QGraphicsScene>

Tower::Tower(int damage, int range, int fireRate, const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsPixmapItem(pixmap, parent),
      damage(damage),
      range(range),
      fireRate(fireRate),
      currentTarget(nullptr) {
    fireTimer = new QTimer(this);
    connect(fireTimer, &QTimer::timeout, this, &Tower::findAndAttackTarget);
    fireTimer->start(1000 / fireRate);
}

void Tower::setTarget(Enemy* target) {
    currentTarget = target;
}

void Tower::findAndAttackTarget() {
    if (currentTarget && targetIsInRange()) {
        attack();
    }
    // ���û��Ŀ�����Ŀ�곬����Χ��GameManager�Ḻ��Ϊ��Ѱ����Ŀ��
}


bool Tower::targetIsInRange() const {
    if (!currentTarget) {
        return false;
    }
    QLineF line(pos(), currentTarget->pos());
    return line.length() <= range;
}

void Tower::attack() {
    if (currentTarget) {
        emit newBullet(this, currentTarget);
    }
}
