#ifndef NIGHTRADIO_H
#define NIGHTRADIO_H

#include "Tower.h"

class NightRadio:public Tower
{
public:
    explicit NightRadio(double range,QGraphicsItem* parent = nullptr);
    void upgrade()override;
    //穿透攻击的逻辑应该在对应的Bullet类中实现
};

#endif // NIGHTRADIO_H
