#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPointer>

class Enemy;

class Bullet : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    //伤害类型
    enum DamageType {
        SingleTarget,
        AreaOfEffect,
        Piercing
    };
    explicit Bullet(int damage, double speed, QGraphicsPixmapItem* target,
                    DamageType type, QPointF startPos, double aoeRadius,
                    const QPixmap& pixmap, QGraphicsItem* parent = nullptr);
    int getDamage() const;
    QGraphicsPixmapItem* getTarget() const;
    DamageType getDamageType() const;
    QPointF getStartPos() const;
    double getAoeRadius() const;

    public slots:
        void move();

    signals:
        void hitTarget(Bullet* bullet);
        void hitEnemy(Bullet* bullet, Enemy* enemy);

private:
    int damage;
    double speed;
    QPointer<QObject> m_targetObject;
    QGraphicsPixmapItem* m_targetPixmap;
    bool m_isTracking;//是否正在追踪
    QPointF m_lastKnownPos;//储存目标位置

    DamageType m_damageType;
    QPointF m_startPos;     // 穿透伤害的起始点 (塔的中心)
    double m_aoeRadius;     // 范围伤害的半径
    QSet<QGraphicsItem*> m_hitTargets;//穿透子弹经过的敌人
};

#endif // BULLET_H
