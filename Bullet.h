#ifndef DREAM_BULLET_H
#define DREAM_BULLET_H

#include <QObject>
#include <QGraphicsPixmapItem>

class Enemy; // ǰ������

class Bullet : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

    public:
    explicit Bullet(Enemy *target, int damage, QGraphicsItem *parent = nullptr);

    // QGraphicsItem ���麯�������ڶ���
    void advance(int phase) override;

    signals:

        void hitTarget(Bullet *bullet);

private:
    Enemy *m_target;
    int m_damage;
    int m_speed;
};

#endif //DREAM_BULLET_H
