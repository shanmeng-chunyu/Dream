#include "InspirationBulb.h"
#include<QPixmap>

InspirationBulb::InspirationBulb(double range,QGraphicsItem* parent):Tower(40,range,0.8,100,150,QPixmap(":/towers/resources/towers/level1/InspirationBulb.png"),parent) {
    type = "InspirationBulb";
}
void InspirationBulb::upgrade()
{
    if(!upgraded)
    {
        damage=60;
        fireRate=0.6;
        upgraded=true;
        fireInterval=fireRate*60;
        originalFireInterval=fireInterval;
        originalDamage=damage;
        if(fireCount>fireInterval)
            fireCount=fireInterval;
        // 1. 定义标准尺寸
        const QSize towerPixelSize(76, 76);
        // 2. 加载原始的升级贴图
        QPixmap originalUpgradePixmap(":/towers/resources/towers/level1/InspirationBulb_upgrade.png");
        // 3. 将其缩放
        QPixmap scaledPixmap = originalUpgradePixmap.scaled(towerPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        // 4. 设置缩放后的贴图
        setPixmap(scaledPixmap);
    }
}

