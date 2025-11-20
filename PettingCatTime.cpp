#include "PettingCatTime.h"
#include <QMovie>
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
        damage = m_damageUpgradeValue;
        fireRate = 0.25;
        fireInterval = fireRate * 60;
        originalFireInterval = fireInterval;
        originalDamage = damage;
        if(fireCount > fireInterval)
            fireCount = fireInterval;
        upgraded = true;
        const QString upgradedGifPath = ":/towers/resources/towers/level2/PettingCatTime_upgrade.gif"; // <--- 假设的 .gif 路径
        // 2. 停止当前正在播放的 QMovie
        m_movie->stop();
        // 3. (重要) 给 m_movie 设置新的GIF文件路径
        m_movie->setFileName(upgradedGifPath);
        // 4. 重新启动 QMovie
        m_movie->start();
        // 5. (可选但推荐) 立即更新一帧，防止短暂的空白或旧帧残留
        if (m_movie->isValid()) {
            // updatePixmapFromMovie() 是 Tower 基类中更新贴图的槽函数
            updatePixmapFromMovie();
        }
    }
}

