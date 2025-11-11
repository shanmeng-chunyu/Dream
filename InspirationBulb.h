#ifndef INSPIRATIONBULB_H
#define INSPIRATIONBULB_H

#include "Tower.h"
#include <QString>

class InspirationBulb:public Tower
{
public:
    explicit InspirationBulb(int range,QGraphicsItem* parent = nullptr);
    void upgrade()override;
};

#endif // INSPIRATIONBULB_H

