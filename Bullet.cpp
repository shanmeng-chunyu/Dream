#include "Bullet.h"
#include "Enemy.h"
#include <QLineF>
#include <QtMath>

Bullet::Bullet(int damage, double speed, QGraphicsPixmapItem* target, const QPixmap& pixmap, QGraphicsItem* parent)
    : QObject(nullptr),
      QGraphicsPixmapItem(pixmap, parent),
      damage(damage),
      speed(speed),
      target(target) {
    setTransformOriginPoint(pixmap.width() / 2.0, pixmap.height() / 2.0);
}

void Bullet::move() {
    if (!target) {
        // 目标不存在，自我销毁
        emit hitTarget(this);
        return;
    }

    // 1. 获取目标位置
    //    (我们在修复障碍物对齐方式后，target->pos() 已统一为目标的中心点)
    QPointF targetPos = target->pos();

    // 2. 获取子弹的当前中心点
    //    (pos() 是左上角, transformOriginPoint() 是中心点)
    QPointF bulletCenter = pos() + transformOriginPoint();

    // 3. 检查是否到达
    QLineF line(bulletCenter, targetPos);
    if (line.length() < speed) {
        // 到达目标
        emit hitTarget(this);
        return;
    }

    // 4. 计算角度 (弧度)
    //    使用 atan2 计算从子弹指向目标的精确角度
    double angle_rad = atan2(targetPos.y() - bulletCenter.y(), targetPos.x() - bulletCenter.x());

    // 5. 【新功能】设置旋转
    //    qRadiansToDegrees 将弧度转为角度。
    //    因为你的贴图原始设计是朝右的 (Qt的0度方向)，所以可以直接使用这个角度。
    double angle_deg = qRadiansToDegrees(angle_rad);
    setRotation(angle_deg);

    // 6. 朝目标移动
    //    (移动逻辑保持不变)
    double dx = speed * cos(angle_rad);
    double dy = speed * sin(angle_rad);
    setPos(pos().x() + dx, pos().y() + dy);
}

int Bullet::getDamage() const {
    return damage;
}

QGraphicsPixmapItem* Bullet::getTarget() const {
    return target;
}
