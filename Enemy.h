#ifndef ENEMY_H
#define ENEMY_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPointF>
#include <vector>
#include <string>

class LiveCoffee;
class Enemy : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Enemy(int health, double speed, int damage, const std::vector<QPointF>& path,QString type, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);

    void takeDamage(int damageAmount);
    int getDamage() const;
    void setAbsolutePath(const std::vector<QPointF>& path);
    QString getType() const{return type;};
    std::vector<QPointF> getAbsolutePath(){return absolutePath;};
    void stopFor(double duration);
    int getHealth() const;
    void heal(int amount);
    void setBaseSpeed(double v);
    int getCurrentPathIndex() const;



    public slots:
        void move();

    signals:
        void reachedEnd(Enemy* enemy);
    void died(Enemy* enemy);

private:
    int damage;
    std::vector<QPointF> absolutePath;
    QString type;
    int m_health;
    double m_speed;
    int m_maxHealth;
    int m_currentPathIndex;
    void applyAuraEffects();
    QList<LiveCoffee*> findCoffeeInRange() const;

    int m_stunTicksRemainimng = 0;
    double m_baseSpeed;//原始速度
    bool m_isFlipped;//贴图是否翻转
};

#endif // ENEMY_H
