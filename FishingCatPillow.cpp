#include "FishingCatPillow.h"
#include <QList>
#include <QLineF>
#include <QGraphicsScene>

FishingCatPillow::FishingCatPillow(double range,QGraphicsItem* parent):Tower(0,range,0,120,180,QPixmap( ":/towers/resources/towers/level1/FishingCatPillow.png"),parent)
{
    controlDuration=1.5;
    controlInterval=5.0;
    //设置控制效果定时器
    controlTimer=new QTimer(this);
    connect(controlTimer,&QTimer::timeout,this,&FishingCatPillow::controlTarget);
    controlTimer->start(controlInterval*1000);
}
void FishingCatPillow::controlTarget()
{
    //如果存在目标，发射信号冷冻enemy
    if(currentTarget)
    {
        emit applyControl(currentTarget,controlDuration);
    }

}
void FishingCatPillow::upgrade()
{
    if(!upgraded)
    {
        controlDuration=2.0;
        controlInterval=4.0;
        upgraded=true;
        setPixmap(QPixmap(":/towers/resources/towers/level1/FishingCatPillow_upgrade.png"));
        if(controlTimer)
        {
            controlTimer->stop();
            controlTimer->start(controlInterval*1000);
        }
    }
}


