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
    int getCurrentHealth()const{return currentHealth;}
    int getMaxHealth()const{return maxHealth;}

    signals:
        // 当障碍物被摧毁时发出此信号
        void destroyedObstacle(Obstacle* obstacle, int resourceValue);
        //障碍物生命值减少时的信号(用于ui中进度条更新，如果不需要可以直接删去
        void healthChanged(int currentHealth,int maxHealth);

private:
    int maxHealth;
    int currentHealth;
    int resourceValue;
};

#endif // OBSTACLE_H
