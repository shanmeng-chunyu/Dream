#ifndef FISHINGCATPILLOW_H
#define FISHINGCATPILLOW_H

#include "Tower.h"

class FishingCatPillow:public Tower
{
public:
    explicit FishingCatPillow(QGraphicsItem* parent = nullptr);
    void upgrade()override;
    void attack()override;//通过fireRate来表示controlRate，但是不作出攻击
signals:
    void applyControl(Enemy *enemy,double duration);//给enemy发出的控制enemy效果信号
private:
    double controlDuration;//控制时长
};

#endif // FISHINGCATPILLOW_H
