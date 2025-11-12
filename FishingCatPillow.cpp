#include "FishingCatPillow.h"
#include <QList>
#include <QLineF>
#include <QGraphicsScene>

FishingCatPillow::FishingCatPillow(double range,QGraphicsItem* parent):Tower(0,range,5,120,180,QPixmap( ":/towers/resources/towers/level1/FishingCatPillow.png"),parent)
{
    controlDuration=1.5;
    type = "FishingCatPillow";
}
void FishingCatPillow::attack()
{    //如果存在目标，发射信号冷冻enemy
    if(currentTarget)
    {
        emit applyControl(currentTarget,controlDuration);
    }
}
void FishingCatPillow::upgrade()
{
    if(!upgraded)
    {
        controlDuration=2.0;
        fireRate=4.0;
        fireInterval=fireRate*60;
        originalFireInterval=fireInterval;
        originalDamage=damage;
        if(fireCount>fireInterval)
            fireCount=fireInterval;
        upgraded=true;
        setPixmap(QPixmap(":/towers/resources/towers/level1/FishingCatPillow_upgrade.png"));
    }
}
