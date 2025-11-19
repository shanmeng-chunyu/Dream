#include "LiveCoffee.h"
#include <QGraphicsScene>
#include <QLineF>
#include <QGraphicsPixmapItem>
#include <QMovie>

// --- 构造函数 (和我们之前修复的一样，保持不变) ---
LiveCoffee::LiveCoffee(double range,const QString &gif_path,QSize pixelSize,QGraphicsItem* parent):Tower(0,range,1,80,120,gif_path,pixelSize,parent)
{
    slowFactor=0.8;
    increaseFactor=0.9;
    type = "LiveCoffee";

    QPixmap originalAuraPixmap(":/bullet/resources/bullet/level1/LiveCoffee.png");
    const QSize auraPixelSize(300, 300);
    QPixmap scaledAuraPixmap = originalAuraPixmap.scaled(auraPixelSize,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation);
    m_auraItem = new QGraphicsPixmapItem(scaledAuraPixmap);
    m_auraItem->setZValue(9);
    m_auraItem->setOpacity(0.4);
}

// --- 【修改】attack() 函数现在什么都不做 ---
void LiveCoffee::attack()
{
    return;
}



// --- upgrade() 函数 (和我们之前修复的一样，保持不变) ---
void LiveCoffee::upgrade()
{
    if(!upgraded)
    {
        slowFactor=0.7;
        increaseFactor=0.85;
        upgraded=true;

        const QString upgradedGifPath = ":/towers/resources/towers/level1/LiveCoffee_upgrade.gif"; // <--- 假设的 .gif 路径
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

        QPixmap originalAuraPixmap(":/bullet/resources/bullet/level1/LiveCoffee.png");
        const QSize auraPixelSize(300, 300);
        QPixmap scaledAuraPixmap = originalAuraPixmap.scaled(auraPixelSize,
                                                            Qt::KeepAspectRatio,
                                                            Qt::SmoothTransformation);
        m_auraItem->setPixmap(scaledAuraPixmap);
        m_auraItem->setZValue(9);
        m_auraItem->setOpacity(0.4);
    }
}