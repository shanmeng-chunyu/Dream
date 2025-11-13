#include "Bullet.h"
#include "Enemy.h"
#include <QLineF>
#include <QtMath>

Bullet::Bullet(int damage, double speed, QGraphicsPixmapItem* target, const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsPixmapItem(pixmap, parent),
      damage(damage),
      speed(speed),
      m_targetPixmap(target) {
    // 2. 将传入的 target 转换为 QObject*
    //    因为 Enemy 和 Obstacle 都继承自 QObject，这个转换是安全的
    QObject* targetAsQObject = dynamic_cast<QObject*>(target);

    // 3. 将 QObject* 存入 QPointer
    m_targetObject = targetAsQObject;

    // 4. (重要) 添加一个断言，确保我们的目标确实是一个 QObject
    //    如果这个断言触发，说明传入了非 QObject 的目标
    Q_ASSERT(m_targetObject != nullptr && "Bullet target must inherit from QObject to be tracked!");

    setTransformOriginPoint(pixmap.width() / 2.0, pixmap.height() / 2.0);
}

void Bullet::move() {
    // 5. 【关键】我们只检查 QObject 追踪器！
    if (m_targetObject.isNull()) {
        // 目标已被销毁，安全地自我销毁
        emit hitTarget(this);
        return;
    }

    // 6. 如果程序执行到这里，说明 m_targetObject 仍然存活
    //    因此，我们可以 100% 安全地使用 m_targetPixmap
    QPointF targetPos = m_targetPixmap->pos();

    // 2. 获取子弹的当前中心点
    QPointF bulletCenter = pos() + transformOriginPoint();

    // 3. 检查是否到达
    QLineF line(bulletCenter, targetPos);
    if (line.length() < speed) {
        // 到达目标
        emit hitTarget(this);
        return;
    }

    // 4. 计算角度 (弧度)
    double angle_rad = atan2(targetPos.y() - bulletCenter.y(), targetPos.x() - bulletCenter.x());

    // 5. 设置旋转
    double angle_deg = qRadiansToDegrees(angle_rad);
    setRotation(angle_deg);

    // 6. 朝目标移动
    double dx = speed * cos(angle_rad);
    double dy = speed * sin(angle_rad);
    setPos(pos().x() + dx, pos().y() + dy);
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
