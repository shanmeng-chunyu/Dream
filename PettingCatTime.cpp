#include "PettingCatTime.h"

PettingCatTime::PettingCatTime(double range,QGraphicsItem* parent)
    : Tower(60, range, 0.5, 180, 220, QPixmap(":/towers/resources/towers/level2/PettingCatTime.png"), parent)
{
    type = "PettingCatTime";
}
void PettingCatTime::attack(){return;}//没有子弹
void PettingCatTime::upgrade()
{
    if(!upgraded)
    {
        damage = 90;
        fireRate = 0.1;
        fireInterval = fireRate * 60;
        originalFireInterval = fireInterval;
        originalDamage = damage;
        if(fireCount > fireInterval)
            fireCount = fireInterval;
        upgraded = true;
        setPixmap(QPixmap(":/towers/resources/towers/level2/PettingCatTime_upgrade.png"));
    }
}
