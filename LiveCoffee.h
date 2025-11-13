#ifndef LIVECOFFEE_H
#define LIVECOFFEE_H

#include "Tower.h"
#include <QSet>
#include "GameManager.h"
class QGraphicsPixmapItem;
class LiveCoffee:public Tower
{
    Q_OBJECT
public:
    explicit LiveCoffee(double range,QGraphicsItem* parent = nullptr);
    void attack()override;
    void upgrade()override;
    double getFactor(){return increaseFactor;};
private slots:
    void findAndAttackTarget() override;
signals:
    void slowEnemyStart(QGraphicsPixmapItem* enemy,double slowFactor);//对范围内所有敌人都减速
    void slowEnemyStop(QGraphicsPixmapItem*enemy);
private:
    double slowFactor;//敌人速度降低率
    double increaseFactor;//塔攻击速度提升率
    QSet <Enemy*> enemies;//范围内的敌人

    QGraphicsPixmapItem* m_auraItem;
};

#endif // LIVECOFFEE_H
