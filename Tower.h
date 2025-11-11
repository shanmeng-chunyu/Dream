#ifndef TOWER_H
#define TOWER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include "Enemy.h" // 包含头文件以使用Enemy类

class Bullet;

class Tower : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Tower(int damage, double range, double fireRate,int cost,int upgradeCost, const QPixmap& pixmap, QGraphicsItem* parent = nullptr);
    virtual ~Tower()=default;
    //获取塔属性
    int getDamage()const{return damage;}
    double getRange()const{return range;}
    double getFireRate()const {return fireRate;}
    int getCost()const{return cost;}
    int getUpgradeCost()const{return upgradeCost;}
    Enemy* getCurrentTarget()const{return currentTarget;}
    bool IsUpgraded()const{return upgraded;}

    //基本功能
    virtual void attack();
    bool targetIsInRange() const;//检查敌人是否在攻击范围内
    void setTarget(Enemy* target);

    //升级
    virtual void upgrade();

public slots:
    void findAndAttackTarget();
    //应对enemy功能
    virtual void slowAttack(double slowFactor);//降低攻速,针对摸鱼猫猫枕的实现，有所不同，设置为虚函数
    void destroy();//摧毁tower，接收到信号直接删除对象

signals:
    void newBullet(Tower* tower, Enemy* target);
    void towerDestroyed(Tower* tower);  //由GameManager处理实际删除

protected:
    //属性
    int damage;
    double range;
    double fireRate;//攻击间隔
    int cost;
    int upgradeCost;
    bool upgraded;//升级标志

    //状态属性
    Enemy* currentTarget;
    QTimer* fireTimer;

    friend class GameManager;
};
#endif // TOWER_H

