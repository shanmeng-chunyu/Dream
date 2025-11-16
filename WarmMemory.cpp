#include "WarmMemory.h"

WarmMemory::WarmMemory(double range,const QString &gif_path,const QSize pixelSize, QGraphicsItem* parent):Tower(200,range,10,120,150,gif_path,pixelSize,parent)
{
    //通过fireRate来表示充能的时间
    type="WarmMemory";
}

void WarmMemory::attack()
{
    if(currentTarget)
    {
        Enemy* enemy = dynamic_cast<Enemy*>(currentTarget);
        if(enemy)
        {
            emit applyControl(enemy,stopDuration);
        }
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
        const QSize towerPixelSize(76, 76);
        QPixmap originalUpgradePixmap(":/towers/resources/towers/level2/WarmMemory_upgrade.png");
        QPixmap scaledPixmap = originalUpgradePixmap.scaled(towerPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(scaledPixmap);
    }
}


