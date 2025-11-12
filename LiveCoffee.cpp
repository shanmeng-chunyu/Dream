#include "LiveCoffee.h"
#include <QGraphicsScene>
#include <QLineF>
LiveCoffee::LiveCoffee(QGraphicsItem* parent):Tower(0,2.5,1,80,120,QPixmap(":/towers/resources/towers/level1/LiveCoffee.png"),parent),enemies()
{
    //续命咖啡的fireRate应该是存在的，因为有对应的bullet，应该有一个发射频率(只是对应的功能不是“攻击”，先设为1）
    slowFactor=0.8;
    increaseFactor=0.9;//增加10%攻速，fireRate*0.9
}

void LiveCoffee::attack()
{
    if(currentTarget){
        emit newBullet(this, currentTarget);
        QList<QGraphicsItem*> items=scene()->items();//获得场景中所有图形项
        QSet<Enemy*>currentEnemies;
        for(auto& item:items)
        {
            Enemy* enemy = dynamic_cast<Enemy*>(item);
            if(enemy)//找范围内的enemy
            {
                QLineF line(pos(),enemy->pos());
                if(line.length()<=range)
                    currentEnemies.insert(enemy);
            }
        }
        QSet<Enemy*>enemiesToAdd=currentEnemies-enemies;//存储已经受到影响的敌人
        QSet<Enemy*>enemiesToRemove=enemies-currentEnemies;//存储离开的敌人
        for(auto& enemy:enemiesToAdd)
        {
            emit slowEnemyStart(enemy,slowFactor);
        }
        for(auto& enemy:enemiesToRemove)
        {
            emit slowEnemyStop(enemy);
        }
        enemies=currentEnemies;
    }
}
void LiveCoffee::upgrade()
{
    if(!upgraded)
    {
        slowFactor=0.7;
        increaseFactor=0.85;
        upgraded=true;
        setPixmap(QPixmap(":/towers/resources/towers/level1/LiveCoffee_upgrade.png"));

    }
}
