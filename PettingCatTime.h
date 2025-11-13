#ifndef PETTINGCATTIME_H
#define PETTINGCATTIME_H

#include "Tower.h"

class PettingCatTime:public Tower
{
public:
    explicit PettingCatTime(QGraphicsItem* parent = nullptr);
    void upgrade() override;
    void attack()override;
};

#endif // PETTINGCATTIME_H
