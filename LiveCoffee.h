#ifndef LIVECOFFEE_H
#define LIVECOFFEE_H

#include "Tower.h"
class QGraphicsPixmapItem;
class LiveCoffee:public Tower
{
    Q_OBJECT
public:
    explicit LiveCoffee(double range,const QString &gif_path,QSize pixelSize,QGraphicsItem* parent = nullptr);
    void attack()override;
    void upgrade()override;
    double getTowerBuffFactor() const { return increaseFactor; }; // 给塔用的
    double getEnemyDebuffFactor() const { return slowFactor; }; // 给敌人用的
private:
    double slowFactor;//敌人速度降低率
    double increaseFactor;//塔攻击速度提升率

    QGraphicsPixmapItem* m_auraItem;
};

#endif // LIVECOFFEE_H
