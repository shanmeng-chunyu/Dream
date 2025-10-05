#ifndef DREAM_TOWER_H
#define DREAM_TOWER_H

#include <QObject>
#include <QGraphicsPixmapItem>

class Enemy; // 前向声明

class Tower : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

    public:
    explicit Tower(QGraphicsItem *parent = nullptr);

    virtual void attack() = 0; // 纯虚函数，定义攻击行为

    void setTarget(Enemy *enemy);

    double distanceTo(QGraphicsItem *item);

    public slots:

        void findTarget();

protected:
    Enemy *m_target;
    int m_damage;
    int m_range;
    int m_fireRate; // 攻击频率
    QTimer *m_fireTimer;
};

#endif //DREAM_TOWER_H
