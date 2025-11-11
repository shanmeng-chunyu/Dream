#include "InspirationBulb.h"
#include<QPixmap>

InspirationBulb::InspirationBulb(double range,QGraphicsItem* parent):Tower(40,range,0.8,100,150,QPixmap(":/towers/resources/towers/level1/InspirationBulb.png"),parent){}
void InspirationBulb::upgrade()
{
    if(!upgraded)
    {
        damage=60;
        range=4.0;
        fireRate=0.6;
        upgraded=true;
        setPixmap(QPixmap( ":/towers/resources/towers/level1/InspirationBulb_upgrade.png"));
        if(fireTimer)
        {
            fireTimer->stop();
            fireTimer->start(1000*fireRate);
        }
    }
}

