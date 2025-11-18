#ifndef FRIENDCOMPANION_H
#define FRIENDCOMPANION_H

#include "Tower.h"

class QGraphicsPixmapItem;
class FriendCompanion:public Tower
{
public:
    explicit FriendCompanion(double range,const QString &gif_path,QSize pixelSize,QGraphicsItem* parent = nullptr);
    void upgrade() override;
    void attack()override;
    double getFactor(){return increaseDamage;};
private:
    double increaseDamage;
};

#endif // FRIENDCOMPANION_H

