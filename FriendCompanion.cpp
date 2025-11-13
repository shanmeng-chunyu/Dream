#include "FriendCompanion.h"

FriendCompanion::FriendCompanion(double range,QGraphicsItem* parent):Tower(0,range,1,100,150,QPixmap(":/towers/resources/towers/level2/Companionship.png"),parent)
{
    //朋友陪伴的fireRate应该是存在的，因为有对应的bullet，应该有一个发射频率(只是对应敌人无功能，先设为1）
    increaseDamage=1.15;
    type="FriendCompanion";
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
}



