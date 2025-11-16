#include "PettingCatTime.h"

PettingCatTime::PettingCatTime(double range,const QString &gif_path,QSize pixelSize,QGraphicsItem* parent)
    : Tower(60, range, 0.5, 180, 220, gif_path,pixelSize, parent)
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
        const QSize towerPixelSize(76, 76);
        QPixmap originalUpgradePixmap(":/towers/resources/towers/level2/PettingCatTime_upgrade.png");
        QPixmap scaledPixmap = originalUpgradePixmap.scaled(towerPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(scaledPixmap);
    }
}

