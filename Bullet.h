#ifndef BULLET_H
#define BULLET_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPointer>

class Enemy;

class Bullet : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Bullet(int damage, double speed, QGraphicsPixmapItem* target, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);
    int getDamage() const;
    QGraphicsPixmapItem* getTarget() const;

    public slots:
        void move();

    signals:
        void hitTarget(Bullet* bullet);

private:
    int damage;
    double speed;
    QPointer<QObject> m_targetObject;
    QGraphicsPixmapItem* m_targetPixmap;
};

#endif // BULLET_H
