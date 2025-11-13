#include "LiveCoffee.h"
#include <QGraphicsScene>
#include <QLineF>
#include <QGraphicsPixmapItem>

// --- 构造函数 (和我们之前修复的一样，保持不变) ---
LiveCoffee::LiveCoffee(double range,QGraphicsItem* parent):Tower(0,range,1,80,120,QPixmap(":/towers/resources/towers/level1/LiveCoffee.png"),parent)
{
    slowFactor=0.8;
    increaseFactor=0.9;
    type = "LiveCoffee";

    QPixmap originalAuraPixmap(":/bullet/resources/bullet/level1/LiveCoffee.png");
    const QSize auraPixelSize(300, 300);
    QPixmap scaledAuraPixmap = originalAuraPixmap.scaled(auraPixelSize,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation);
    m_auraItem = new QGraphicsPixmapItem(scaledAuraPixmap, this);
    const QPointF towerCenter(76.0 / 2.0, 76.0 / 2.0);
    const QPointF auraTopLeft(towerCenter.x() - auraPixelSize.width() / 2.0,
                                towerCenter.y() - auraPixelSize.height() / 2.0);
    m_auraItem->setPos(auraTopLeft);
    m_auraItem->setZValue(-1);
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

        const QSize towerPixelSize(76, 76);
        QPixmap originalUpgradePixmap(":/towers/resources/towers/level1/LiveCoffee_upgrade.png");
        QPixmap scaledPixmap = originalUpgradePixmap.scaled(towerPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(scaledPixmap);

        QPixmap originalAuraPixmap(":/bullet/resources/bullet/level1/LiveCoffee.png");
        const QSize auraPixelSize(300, 300);
        QPixmap scaledAuraPixmap = originalAuraPixmap.scaled(auraPixelSize,
                                                            Qt::KeepAspectRatio,
                                                            Qt::SmoothTransformation);
        m_auraItem->setPixmap(scaledAuraPixmap);
        const QPointF towerCenter(76.0 / 2.0, 76.0 / 2.0);
        const QPointF auraTopLeft(towerCenter.x() - auraPixelSize.width() / 2.0,
                                    towerCenter.y() - auraPixelSize.height() / 2.0);
        m_auraItem->setPos(auraTopLeft);
        m_auraItem->setOpacity(0.4);
    }
}