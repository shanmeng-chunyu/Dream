#ifndef TOWER_H
#define TOWER_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <Qstring>
#include "Enemy.h" // 包含头文件以使用Enemy类

class Bullet;
class LiveCoffee;
class FriendCompanion;
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
    void setRange(int newrange){range=newrange;}

    //升级
    virtual void upgrade();

public slots:
    void findAndAttackTarget();
    //应对enemy功能
    void slowAttack(double slowFactor);//降低攻速
    void slowAttackStop();//停止降低攻速
    void destroy();//摧毁tower，接收到信号直接删除对象

signals:
    void newBullet(Tower* tower, Enemy* target);
    void towerDestroyed(Tower* tower);  //由GameManager处理实际删除

private:
    //光环类tower检测，光环类Tower不需要这些函数
    void coffeeEffect();//实现
    void friendEffect();
    QList<LiveCoffee*> findCoffeeInRange();//查找
    QList<FriendCompanion*> findFriendInRange();

protected:
    //属性
    int damage;
    double range;
    double fireRate;//攻击间隔
    int cost;
    int upgradeCost;
    bool upgraded;//升级标志
    QString type;

    //状态属性
    Enemy* currentTarget;
    int fireInterval;//开火间隔(帧数)
    int fireCount;//距离下次攻击的帧数

    //为了防止无限叠加，用于记录原始数值的变量
    int originalFireInterval;
    int originalDamage;

    friend class GameManager;
};
#endif // TOWER_H
