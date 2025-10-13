#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QGraphicsPixmapItem>

class Enemy;

class Bullet : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Bullet(int damage, double speed, Enemy* target, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);
    int getDamage() const;
    Enemy* getTarget() const;

    public slots:
        void move();

    signals:
        void hitTarget(Bullet* bullet);

private:
    int damage;
    double speed;
    Enemy* target;
};

#endif // BULLET_H
