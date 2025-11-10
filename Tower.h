#ifndef TOWER_H
#define TOWER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include "Enemy.h" // 包含头文件以使用Enemy类

class Bullet;

class Tower : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    explicit Tower(int damage, double range, double fireRate,int cost,int upgradeCost, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);
    virtual ~Tower()=default;
    //获取塔属性
    int getDamage()const{return damage;}
    double getRange()const{return range;}
    double getFireRate()const {return fireRate;}
    int getCost()const{return cost;}
    Enemy* getCurrentTarget()const{return currentTarget;}
    //基本功能
    virtual void attack();
    bool targetIsInRange() const;
    void setTarget(Enemy* target);
    //升级
    virtual void upgrade()=0;
    bool IfUpgrade()const{return upgraded;}
    int getUpgradeCost()const{return upgradeCost;}

    public slots:
        void findAndAttackTarget();

    signals:
        void newBullet(Tower* tower, Enemy* target);

protected:
    int damage;
    double range;
    double fireRate;//攻击间隔
    int cost;
    Enemy* currentTarget;
    bool upgraded;//升级标志
    int upgradeCost;
    QTimer* fireTimer;
    friend class GameManager;
};

#endif // TOWER_H
