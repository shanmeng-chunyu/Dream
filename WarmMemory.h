#ifndef WARMMEMORY_H
#define WARMMEMORY_H

#include "Tower.h"

class WarmMemory:public Tower
{
public:
   explicit WarmMemory(QGraphicsItem* parent = nullptr);
    void upgrade()override;
    void attack()override;//没有子弹
signals:
    void applyControl(Enemy *enemy,double duration);
private:
    double stopDuration;
};

#endif // WARMMEMORY_H
