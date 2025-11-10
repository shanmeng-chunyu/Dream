#include "KnowledgeTree.h"
#include <QPixmap>

KnowledgeTree::KnowledgeTree(QGraphicsItem* parent):Tower(160,3.5,2.0,200,250,QPixmap( "resources/towers/level1/KnowledgeTree.png"),parent)
{
    upgradedPixmapPath = "resources/towers/level1/KnowledgeTree_upgrade.png";
}

void KnowledgeTree::upgrade()
{
    if(!upgraded)
    {
        damage=200;
        range=4.0;
        fireRate=1.6;
        upgraded=true;
        setPixmap(QPixmap(upgradedPixmapPath));
        if(fireTimer)
        {
            fireTimer->stop();
            fireTimer->start(1000/fireRate);
        }
    }
}
