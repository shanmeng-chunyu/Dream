#ifndef INSPIRATIONBULB_H
#define INSPIRATIONBULB_H

#include "Tower.h"

class InspirationBulb:public Tower
{
public:
    explicit InspirationBulb(double range,const QString &gif_path, QSize pexelSize,QGraphicsItem* parent = nullptr);
    void upgrade()override;
};

#endif // INSPIRATIONBULB_H


