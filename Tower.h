#ifndef TOWER_H
#define TOWER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include "Enemy.h" // 包含头文件以使用Enemy类

class Bullet;

class Tower : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Tower(int damage, int range, int fireRate, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);
    virtual void attack();
    bool targetIsInRange() const;

    public slots:
        void findAndAttackTarget();
    void setTarget(Enemy* target);

    signals:
        void newBullet(Tower* tower, Enemy* target);

protected:
    int damage;
    double range;
    int fireRate;
    Enemy* currentTarget;
    QTimer* fireTimer;friend class GameManager;
};

#endif // TOWER_H
