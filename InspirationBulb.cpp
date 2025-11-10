#include "InspirationBulb.h"
#include<QPixmap>

InspirationBulb::InspirationBulb(QGraphicsItem* parent):Tower(40,2.5,0.8,100,150,QPixmap("resources/towers/level1/InspirationBulb.png"),parent)
{
    upgradedPixmapPath = "resources/towers/level1/InspirationBulb_upgrade.png";
}
void InspirationBulb::upgrade()
{
//好像没有升级后的数据先随便设置了一点
    if(!upgraded)
    {
        damage=60;
        range=4.0;
        fireRate=0.6;
        upgraded=true;
        setPixmap(QPixmap(upgradedPixmapPath));
        if(fireTimer)
        {
            fireTimer->stop();
            fireTimer->start(1000/fireRate);
        }
    }
}
