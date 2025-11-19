#include "NightRadio.h"
#include <QMovie>
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
        const QString upgradedGifPath = ":/towers/resources/towers/level2/NightRadio_upgrade.gif"; // <--- 假设的 .gif 路径
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

