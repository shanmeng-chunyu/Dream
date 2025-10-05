#ifndef DREAM_BULLET_H
#define DREAM_BULLET_H

#include <QObject>
#include <QGraphicsPixmapItem>

class Enemy; // 前向声明

class Bullet : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

    public:
    explicit Bullet(Enemy *target, int damage, QGraphicsItem *parent = nullptr);

    // QGraphicsItem 的虚函数，用于动画
    void advance(int phase) override;

    signals:

        void hitTarget(Bullet *bullet);

private:
    Enemy *m_target;
    int m_damage;
    int m_speed;
};

#endif //DREAM_BULLET_H
