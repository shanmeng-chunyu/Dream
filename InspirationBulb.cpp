#include "InspirationBulb.h"
#include<QPixmap>

InspirationBulb::InspirationBulb(QGraphicsItem* parent):Tower(40,2.5,0.8,100,150,QPixmap("resources/towers/level1/InspirationBulb.png"),parent){}
void InspirationBulb::upgrade()
{
    if(!upgraded)
    {
        damage=60;
        range=3.5;
        fireRate=0.6;
        upgraded=true;
        setPixmap(QPixmap("resources/towers/level1/InspirationBulb_upgrade.png"));
        fireTimer->stop();
        fireTimer->start(1000*fireRate);
    }
}
