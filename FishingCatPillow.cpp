#include "FishingCatPillow.h"
#include <QPixmap>
#include <QList>
#include <algorithm>
#include <QLineF>
#include <QRandomGenerator>
#include <QGraphicsScene>

FishingCatPillow::FishingCatPillow(QGraphicsItem* parent):Tower(0,2.5,0,120,180,QPixmap( "resources/towers/level1/FishingCatPillow.png"),parent)
{
    controlDuration=1.5;
    controlInterval=5.0;
    //设置控制效果定时器
    controlTimer=new QTimer(this);
    connect(controlTimer,&QTimer::timeout,this,&FishingCatPillow::applyControl);
    controlTimer->start(controlInterval*1000);
    upgradedPixmapPath = "resources/towers/level1/FishingCatPillow_upgrade.png";
}
void FishingCatPillow::attack()//不做攻击操作，直接返回
{
    return;
}
void FishingCatPillow::applyControl()
{
    QList<QPair<Enemy*,double>> targets;//存储range内的敌人，从近到远排布
    QList<QGraphicsItem*> items=scene()->items();//获得场景中所有图形项
    for(auto item:items)//找到所有在范围内的enemy
    {
        Enemy *enemy= dynamic_cast<Enemy*>(item);//把图像项转化为enemy类
        if(enemy)//不为空说明此项确实是enemy
        {
            QLineF line(pos(),enemy->pos());//计算距离
            if(line.length()<=range)
                targets.append(qMakePair(enemy,line.length()));
        }
    }
    if(!targets.isEmpty())
    {
        std::sort(targets.begin(),targets.end(),[](const QPair<Enemy*,double>&a,const QPair<Enemy*,double>&b){return a.second<b.second;});//按照距离升序排列
        Enemy* target1=targets.at(0).first;
        target1->applyStop(controlDuration);//实施敌人眩晕效果，函数名未知!!!!!!!假定为applyStop，传入停止时间
        if(upgraded&&targets.size()>1)//tower已经升级并且范围内存在第二个敌人
        {
            if(QRandomGenerator::global()->bounded(100) <20)//20%的机率对target2操作
            {
                Enemy* target2=targets.at(1).first;
                target2->applyStop(controlDuration);
            }

        }
    }
}
void FishingCatPillow::upgrade()
{
    if(!upgraded)
    {
        controlDuration=2;
        controlInterval=4.0;
        upgraded=true;
        setPixmap(QPixmap(upgradedPixmapPath));
        if(controlTimer)
        {
            controlTimer->stop();
            controlTimer->start(controlInterval*1000);
        }
    }
}
