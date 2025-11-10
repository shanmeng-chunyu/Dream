#ifndef INSPIRATIONBULB_H
#define INSPIRATIONBULB_H

#include "Tower.h"
#include <QString>

class InspirationBulb:public Tower
{
public:
    explicit InspirationBulb(QGraphicsItem* parent = nullptr);
    void upgrade()override;
private:
    QString upgradedPixmapPath;//升级后的图片路径
};

#endif // INSPIRATIONBULB_H
