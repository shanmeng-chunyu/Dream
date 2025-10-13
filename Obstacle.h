#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QObject>
#include <QGraphicsPixmapItem>

class Obstacle : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Obstacle(int health, int resourceValue, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);

    // 接受伤害
    void takeDamage(int damage);
    // 获取摧毁后提供的资源值
    int getResourceValue() const;

    signals:
        // 当障碍物被摧毁时发出此信号
        void destroyed(Obstacle* obstacle, int resourceValue);

private:
    int maxHealth;
    int currentHealth;
    int resourceValue;
};

#endif // OBSTACLE_H
