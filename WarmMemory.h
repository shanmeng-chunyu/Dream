#ifndef WARMMEMORY_H
#define WARMMEMORY_H

#include "Tower.h"

class WarmMemory:public Tower
{
    Q_OBJECT
public:
    explicit WarmMemory(double range,const QString &gif_path, const QString &charge_gif_path,
                        QSize pixelSize,QGraphicsItem* parent = nullptr);
    void upgrade()override;
    void attack()override;//没有子弹
signals:
    void applyControl(QGraphicsPixmapItem *enemy,double duration);
private:
    double stopDuration;
};

#endif // WARMMEMORY_H
