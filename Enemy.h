#ifndef DREAM_ENEMY_H
#define DREAM_ENEMY_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPointF>
#include <vector>

class Enemy : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

    public:
    explicit Enemy(const std::vector<QPointF> &path, QGraphicsItem *parent = nullptr);

    void takeDamage(int damage);

    // QGraphicsItem 的虚函数，用于动画
    void advance(int phase) override;

    signals:

        void reachedEnd(Enemy *enemy);

    void died(Enemy *enemy);

private:
    int m_health;
    int m_speed;
    std::vector<QPointF> m_path;
    int m_pathIndex;
};

#endif //DREAM_ENEMY_H
