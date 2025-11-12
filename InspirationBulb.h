#ifndef INSPIRATIONBULB_H
#define INSPIRATIONBULB_H

#include "Tower.h"

class InspirationBulb:public Tower
{
public:
    explicit InspirationBulb(double range,QGraphicsItem* parent = nullptr);
    void upgrade()override;
};

#endif // INSPIRATIONBULB_H


