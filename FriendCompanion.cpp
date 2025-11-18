#include "FriendCompanion.h"
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include <QMovie>

FriendCompanion::FriendCompanion(double range,const QString &gif_path,QSize pixelSize,QGraphicsItem* parent):Tower(0,range,1,100,150,gif_path,pixelSize,parent)
{
    //朋友陪伴的fireRate应该是存在的，因为有对应的bullet，应该有一个发射频率(只是对应敌人无功能，先设为1）
    increaseDamage=1.15;
    type="Companionship";

    if (m_movie && !m_movie->isValid())
    {
        // 加载失败 (说明 gif_path 是一个 PNG)
        // 我们手动将其加载为静态贴图
        QPixmap originalPixmap(gif_path);

        // (m_pixelSize 是从 Tower.h 继承的)
        QPixmap scaledPixmap = originalPixmap.scaled(m_pixelSize,
                                                    Qt::KeepAspectRatio,
                                                    Qt::SmoothTransformation);
        setPixmap(scaledPixmap);

        // 停止这个无效的 movie
        m_movie->stop();
    }

    // 3a. 从 tower_data.json 获取光环贴图路径
    QPixmap originalAuraPixmap(":/bullet/resources/bullet/level2/Companionship.png");
    // 3b. 设置光环大小 (我们使用和 LiveCoffee 一样的 300x300)
    const QSize auraPixelSize(300, 300);
    QPixmap scaledAuraPixmap = originalAuraPixmap.scaled(auraPixelSize,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation);

    // 3c. 创建光环 item，并将其设为塔的子项
    m_auraItem = new QGraphicsPixmapItem(scaledAuraPixmap);

    // 3d. 将光环居中 (假设塔的尺寸是 76x76)
    m_auraItem->setZValue(9);    // 确保光环在塔的身后
    m_auraItem->setOpacity(0.2);  // 设置半透明
}
void FriendCompanion::attack(){return;}
void FriendCompanion:: upgrade()
{
    increaseDamage=1.25;
    upgraded=true;
    const QSize towerPixelSize(76, 76);
    QPixmap originalUpgradePixmap(":/towers/resources/towers/level2/Companionship_upgrade.png");
    QPixmap scaledPixmap = originalUpgradePixmap.scaled(towerPixelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    setPixmap(scaledPixmap);

    // 4a. 加载升级后的光环贴图 (在这里它和原版是同一张)
    QPixmap originalAuraPixmap(":/bullet/resources/bullet/level2/Companionship.png");
    const QSize auraPixelSize(300, 300);
    QPixmap scaledAuraPixmap = originalAuraPixmap.scaled(auraPixelSize,
                                                        Qt::KeepAspectRatio,
                                                        Qt::SmoothTransformation);
    // 4b. 更新现有的 m_auraItem
    m_auraItem->setPixmap(scaledAuraPixmap);

    // 4c. 重新居中（以防万一）
    m_auraItem->setZValue(9);
    m_auraItem->setOpacity(0.4);
}



