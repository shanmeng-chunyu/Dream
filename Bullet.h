#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QGraphicsPixmapItem>

class Enemy;
class Obstacle;

class Bullet : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Bullet(int damage, double speed, QGraphicsPixmapItem* target, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);
    int getDamage() const;
    QGraphicsPixmapItem* getTarget() const;

    void update();

    signals:
        void hitTarget(Bullet* bullet);
        void outOfBounds(Bullet* bullet);

private:
    int damage;
    double speed;
    QGraphicsPixmapItem* target;
    int moveCounter = 1;
};

#endif // BULLET_H
