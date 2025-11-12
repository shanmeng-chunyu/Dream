#include "WarmMemory.h"

WarmMemory::WarmMemory(QGraphicsItem* parent):Tower(200,2.5,10,120,150,QPixmap(":/towers/resources/towers/level2/WarmMemories.png"),parent)
{
    //通过fireRate来表示充能的时间
    type="WarmMemory";
}

void WarmMemory::attack()
{
    if(currentTarget)
    {
        emit applyControl(currentTarget,stopDuration);
    }
}

void WarmMemory::upgrade()
{
    if(!upgraded)
    {
        damage=300;
        stopDuration=3.0;
        fireRate=8.0;
        fireInterval=fireRate*60;
        originalFireInterval=fireInterval;
        originalDamage=damage;
        if(fireCount>fireInterval)
            fireCount=fireInterval;
        upgraded=true;
        setPixmap(QPixmap(":/towers/resources/towers/level2/WarmMemory_upgrade.png"));
    }
}
