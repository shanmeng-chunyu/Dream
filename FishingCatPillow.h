#ifndef FISHINGCATPILLOW_H
#define FISHINGCATPILLOW_H

#include "Tower.h"
#include <QTimer>

class FishingCatPillow:public Tower
{
public:
    explicit FishingCatPillow(QGraphicsItem* parent = nullptr);
    void upgrade()override;
private slots:
    void controlTarget();//索敌--控制逻辑
signals:
    void applyControl(Enemy *enemy,double duration);//给enemy发出的控制enemy效果信号
private:
    QTimer *controlTimer;//控制效果定时器
    double controlDuration;//控制时长
    double controlInterval;//冷却时间
};

#endif // FISHINGCATPILLOW_H
