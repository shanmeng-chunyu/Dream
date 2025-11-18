#ifndef PETTINGCATTIME_H
#define PETTINGCATTIME_H

#include "Tower.h"

class PettingCatTime:public Tower
{
    Q_OBJECT
public:
    explicit PettingCatTime(double range,const QString &gif_path,QSize pixelSize,QGraphicsItem* parent = nullptr);
    void upgrade() override;
    void attack()override;
    signals:
    void applyControl(QGraphicsPixmapItem *enemy, double duration);
};

#endif // PETTINGCATTIME_H
