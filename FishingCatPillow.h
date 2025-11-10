#ifndef FISHINGCATPILLOW_H
#define FISHINGCATPILLOW_H

#include "Tower.h"
#include <QTimer>

class FishingCatPillow:public Tower
{
public:
    explicit FishingCatPillow(QGraphicsItem* parent = nullptr);
    void attack()override;
    void upgrade()override;
private slots:
    void applyControl();//控制效果函数
private:
    QTimer *controlTimer;//控制效果定时器
    double controlDuration;//控制时长
    double controlInterval;//冷却时间
    QString upgradedPixmapPath;

};

#endif // FISHINGCATPILLOW_H
