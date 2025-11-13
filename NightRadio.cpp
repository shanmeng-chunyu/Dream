#include "NightRadio.h"

NightRadio::NightRadio(QGraphicsItem* parent):Tower(100,4.5,1.5,150,200,QPixmap(":/Tower/resources/towers/level2/NightRadio.png"),parent)
{
    type="NightRadio";
}
void NightRadio::upgrade()
{
    if(!upgraded)
    {
        damage=150;
        fireRate=1.0;
        fireInterval = fireRate * 60;
        originalFireInterval = fireInterval;
        originalDamage = damage;
        if(fireCount > fireInterval)
            fireCount = fireInterval;
        upgraded = true;
        setPixmap(QPixmap(":/towers/resources/towers/level2/NightRadio_upgrade.png"));
    }
}
