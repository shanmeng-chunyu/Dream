#include "Bullet.h"
#include "Enemy.h"
#include "Obstacle.h"
#include <QLineF>
#include <QtMath>
#include <QGraphicsScene>

Bullet::Bullet(int damage, double speed, QGraphicsPixmapItem* target,
               DamageType type, QPointF startPos, double aoeRadius,
               const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsPixmapItem(pixmap, parent),
      damage(damage),
      speed(speed),
      m_targetPixmap(target),
      m_isTracking(true),
      m_damageType(type),
      m_startPos(startPos),
      m_aoeRadius(aoeRadius){
    QObject* targetAsQObject = dynamic_cast<QObject*>(target);

    // 3. 将 QObject* 存入 QPointer
    m_targetObject = targetAsQObject;

    // 4. (重要) 添加一个断言，确保我们的目标确实是一个 QObject
    //    如果这个断言触发，说明传入了非 QObject 的目标
    Q_ASSERT(m_targetObject != nullptr && "Bullet target must inherit from QObject to be tracked!");

    // [新] 在构造时立即存储目标的初始位置
    m_lastKnownPos = target->pos();

    setTransformOriginPoint(pixmap.width() / 2.0, pixmap.height() / 2.0);
    m_hitTargets.clear();
}

void Bullet::move() {
    QPointF targetPos; // 这一帧要飞向的目标点

    // 1. 确定目标点 (m_lastKnownPos)
    if (m_damageType == Piercing) {
        // 穿透弹：始终飞向最初的目标点 (m_lastKnownPos 保持不变)
        targetPos = m_lastKnownPos;
    } else {
        // 单体/AOE弹：如果目标活着，就更新目标点
        if (m_isTracking) {
            if (m_targetObject.isNull()) {
                m_isTracking = false;
            } else {
                m_lastKnownPos = m_targetPixmap->pos();
            }
        }
        targetPos = m_lastKnownPos;
    }

    // 2. 检查是否到达终点
    QPointF bulletCenter = pos() + transformOriginPoint();
    QLineF line(bulletCenter, targetPos);

    if (line.length() < speed) {
        // 到达最终目的地
        emit hitTarget(this); // 发射 "我已飞完" 信号
        return;
    }

    // 3. 计算移动 (保持不变)
    double angle_rad = atan2(targetPos.y() - bulletCenter.y(), targetPos.x() - bulletCenter.x());
    double angle_deg = qRadiansToDegrees(angle_rad);
    setRotation(angle_deg);
    double dx = speed * cos(angle_rad);
    double dy = speed * sin(angle_rad);

    // 4. [修改] 在移动之前，先不 setPos()

    // 5. [新] 穿透伤害的碰撞检测
    if (m_damageType == Piercing) {
        // 先移动
        setPos(pos().x() + dx, pos().y() + dy);

        // 检查移动后我们与谁碰撞了
        QList<QGraphicsItem*> hits = collidingItems();
        for (QGraphicsItem* item : hits) {
            Enemy* enemy = dynamic_cast<Enemy*>(item);

            // 如果是一个敌人 并且 我们以前没有击中过它
            if (enemy && !m_hitTargets.contains(enemy)) {
                // 发射 "我打中了一个" 信号
                emit hitEnemy(this, enemy);
                // 标记为 "已击中"，这样我们就不会在下一帧再次伤害它
                m_hitTargets.insert(enemy);
            }
            Obstacle* obstacle = dynamic_cast<Obstacle*>(item);
            if (obstacle && !m_hitTargets.contains(obstacle)) {
                emit hitObstacle(this, obstacle);
                m_hitTargets.insert(obstacle);
            }
        }
    } else {
        // 非穿透弹：直接移动 (原逻辑)
        setPos(pos().x() + dx, pos().y() + dy);
    }
}

int Bullet::getDamage() const {
    return damage;
}

QGraphicsPixmapItem* Bullet::getTarget() const {
    // 7. 只有当目标存活时才返回指针，否则返回 nullptr
    if (m_targetObject.isNull()) {
        return nullptr;
    }
    return m_targetPixmap;
}

Bullet::DamageType Bullet::getDamageType() const {
    return m_damageType;
}

QPointF Bullet::getStartPos() const {
    return m_startPos;
}

double Bullet::getAoeRadius() const {
    return m_aoeRadius;
}
