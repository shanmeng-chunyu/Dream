#include "WarmMemory.h"

WarmMemory::WarmMemory(double range,const QString &gif_path, const QString &charge_gif_path, QSize pixelSize, QGraphicsItem* parent)
    // 注意：我们将 base 和 upgrade 的 charge path 传递给基类
    : Tower(200, range, 10, 120, 150, gif_path, pixelSize, parent)
{
    // 强制设置 type 为 WarmMemories
    type = "WarmMemories";

    // 【新增】在这里保存 WarmMemory 特有的所有 GIF 路径
    m_baseGifPath = gif_path; // 未充能
    m_chargeGifPath = charge_gif_path; // 充能

    // 假设升级后的路径：
    m_upgradedGifPath = ":/towers/resources/towers/level2/WarmMemories_upgrade.gif";
    m_upgradedChargeGifPath = ":/towers/resources/towers/level2/WarmMemories_upgrade_charge.gif";

    stopDuration = 2.0;

    // 初始显示为未充能状态
    updateVisualState(false);
}

void WarmMemory::attack()
{
    updateVisualState(true);
    if(currentTarget)
    {
        Enemy* enemy = dynamic_cast<Enemy*>(currentTarget);
        if(enemy)
        {
            enemy->takeDamage(this->damage);
            emit applyControl(enemy,stopDuration);
            updateVisualState(false);
        }
    }
}

void WarmMemory::upgrade()
{
    if(!upgraded)
    {
        damage=m_damageUpgradeValue;
        stopDuration=3.0;
        fireRate=4.0;
        fireInterval=fireRate*60;
        originalFireInterval=fireInterval;
        originalDamage=damage;
        if(fireCount>fireInterval)
            fireCount=fireInterval;
        upgraded=true;
        updateVisualState(false);
    }
}


