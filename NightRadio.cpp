#include "NightRadio.h"

NightRadio::NightRadio(double range,const QString &gif_path,QSize pixelSize,QGraphicsItem* parent):Tower(100,range,1.5,150,200,gif_path,pixelSize,parent)
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
        const QSize towerPixelSize(76, 76);
        QPixmap originalUpgradePixmap(":/towers/resources/towers/level2/NightRadio_upgrade.png");
        QPixmap scaledPixmap = originalUpgradePixmap.scaled(towerPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(scaledPixmap);
    }
}

