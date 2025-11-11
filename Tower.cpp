#include "Tower.h"
#include "Bullet.h"
#include "Enemy.h"
#include <QLineF>

Tower::Tower(int damage, double range, double fireRate,int cost,int upgradeCost,const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr),
    QGraphicsPixmapItem(pixmap, parent),
    damage(damage),
    range(range),
    fireRate(fireRate),
    cost(cost),
    upgradeCost(upgradeCost),
    upgraded(false),
    currentTarget(nullptr),fireTimer(nullptr)
{
    setPixmap(pixmap);
    if(fireRate>0)//为了后续不需要fireTimer的情况方便传入fireRate数据
    {
        fireTimer = new QTimer(this);
        connect(fireTimer, &QTimer::timeout, this, &Tower::findAndAttackTarget);
        fireTimer->start(1000 / fireRate);
    }
}

void Tower::setTarget(Enemy* target) {
    currentTarget = target;
}

bool Tower::targetIsInRange() const {
    if (!currentTarget) {
        return false;
    }
    QLineF line(pos(), currentTarget->pos());//tower和enemy之间的距离
    return line.length() <= range;
}

void Tower::findAndAttackTarget() {
    if (currentTarget && targetIsInRange()) {
        attack();
    }
    //如果没有目标或者目标超出范围，GameManager会负责为其寻找新目标
}

void Tower::attack() {
    if (currentTarget) {
        emit newBullet(this, currentTarget);
    }
}

void Tower::destroy(){
    if(fireTimer!=nullptr)
        fireTimer->stop();
     emit towerDestroyed(this);
}

void Tower::slowAttack(double slowFactor){
    //GameManager会负责检测周围是否有回忆怪物
    if(fireTimer!=nullptr){
        fireTimer->stop();
        if(fireRate>0)
        {
            fireTimer->start(1000/(fireRate*slowFactor));
        }
    }
}

void Tower::upgrade(){
    if (upgraded) {

    }
}

