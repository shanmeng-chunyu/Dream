#include "PettingCatTime.h"

PettingCatTime::PettingCatTime(double range,const QString &gif_path,QSize pixelSize,QGraphicsItem* parent)
    : Tower(60, range, 0.5, 180, 220, gif_path,pixelSize, parent)
{
    type = "PettingCatTime";
}
void PettingCatTime::attack(){
    if(currentTarget)
    {
        Enemy* enemy = dynamic_cast<Enemy*>(currentTarget);
        if(enemy)
        {
            // 2a. 立即对敌人造成伤害 (this->damage 是基类成员)
            enemy->takeDamage(this->damage);

            // 2b. (保留) 发射信号，应用视觉特效 (持续1.0秒)
            if (!IsUpgraded()) {
                emit applyControl(enemy, 0.25);
            }else {
                emit applyControl(enemy, 0.1);
            }
        }
    }
}//没有子弹
void PettingCatTime::upgrade()
{
    if(!upgraded)
    {
        damage = 90;
        fireRate = 0.25;
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

