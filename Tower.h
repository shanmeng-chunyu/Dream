#ifndef DREAM_TOWER_H
#define DREAM_TOWER_H

#include <QObject>
#include <QGraphicsPixmapItem>

class Enemy; // ǰ������

class Tower : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

    public:
    explicit Tower(QGraphicsItem *parent = nullptr);

    virtual void attack() = 0; // ���麯�������幥����Ϊ

    void setTarget(Enemy *enemy);

    double distanceTo(QGraphicsItem *item);

    public slots:

        void findTarget();

protected:
    Enemy *m_target;
    int m_damage;
    int m_range;
    int m_fireRate; // ����Ƶ��
    QTimer *m_fireTimer;
};

#endif //DREAM_TOWER_H
