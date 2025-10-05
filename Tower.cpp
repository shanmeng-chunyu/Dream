#include "Tower.h"
#include <QTimer>
#include <QLineF>
#include <QList>
#include "GameManager.h"
#include "Enemy.h"

Tower::Tower(QGraphicsItem *parent)
    : QObject(nullptr), QGraphicsPixmapItem(parent), m_target(nullptr), m_damage(10), m_range(150), m_fireRate(1000) {
    // 设置塔的图像 (这里用一个简单的颜色块代替)
    QPixmap pixmap(50, 50);
    pixmap.fill(Qt::cyan);
    setPixmap(pixmap);

    m_fireTimer = new QTimer(this);
    connect(m_fireTimer, &QTimer::timeout, this, &Tower::findTarget);
    m_fireTimer->start(m_fireRate);
}

void Tower::setTarget(Enemy *enemy) {
    m_target = enemy;
}

double Tower::distanceTo(QGraphicsItem *item) {
    if(!item) return 0;
    QLineF line(pos(), item->pos());
    return line.length();
}

void Tower::findTarget() {
    // 这里需要从GameManager获取敌人列表来查找目标
    // 这是一个简化的逻辑，实际应遍历所有敌人
    // if(m_target && distanceTo(m_target) <= m_range){
    //     attack();
    // } else {
    //     // 寻找新目标
    // }
    attack(); // 简化为一直攻击
}
