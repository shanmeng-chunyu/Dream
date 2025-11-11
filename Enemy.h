#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPointF>
#include <vector>
#include <string>

class Enemy : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Enemy(int health, double speed, int damage, const std::vector<QPointF>& path, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);

    void takeDamage(int damageAmount);
    int getDamage() const;
    void setAbsolutePath(const std::vector<QPointF>& path);
    QString getType() const{return type;};
    std::vector<QPointF> getAbsolutePath(){return absolutePath;};

    public slots:
        void move();

    signals:
        void reachedEnd(Enemy* enemy);
    void died(Enemy* enemy);

private:
    int currentHealth;
    double speed;
    int damage;
    std::vector<QPointF> absolutePath;
    int currentPathIndex;
    QString type;
};

#endif // ENEMY_H
