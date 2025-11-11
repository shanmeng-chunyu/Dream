#include "FishingCatPillow.h"
#include <QList>
#include <QLineF>
#include <QGraphicsScene>

FishingCatPillow::FishingCatPillow(QGraphicsItem* parent):Tower(0,2.5,0,120,180,QPixmap( "resources/towers/level1/FishingCatPillow.png"),parent)
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
    QList<QGraphicsItem*> items = scene()->items();//所有图形项
    //寻找最近的敌人
    Enemy* closestEnemy=nullptr;
    double closestdistance=std::numeric_limits<double>::max();
    for(auto& item:items)
    {
        Enemy *enemy=dynamic_cast<Enemy*>(item);
        if(enemy)
        {
            QLineF line(pos(),enemy->pos());
            if(line.length()<=range)
            {
                if(line.length()< closestdistance)
                {
                    closestdistance=line.length();
                    closestEnemy=enemy;
                }
            }
        }
    }
    //如果寻找到，发射信号冷冻enemy
    if(closestEnemy)
    {
        emit applyControl(closestEnemy,controlDuration);
    }

}
void FishingCatPillow::upgrade()
{
    if(!upgraded)
    {
        controlDuration=2.0;
        controlInterval=4.0;
        upgraded=true;
        setPixmap(QPixmap("resources/towers/level1/FishingCatPillow_upgrade.png"));
        if(controlTimer)
        {
            controlTimer->stop();
            controlTimer->start(controlInterval*1000);
        }
    }
}
