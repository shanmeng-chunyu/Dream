#include "KnowledgeTree.h"

KnowledgeTree::KnowledgeTree(QGraphicsItem* parent):Tower(160,3.5,2.0,200,250,QPixmap( "resources/towers/level1/KnowledgeTree.png"),parent){}

void KnowledgeTree::upgrade()
{
    if(!upgraded)
    {
        damage=200;
        range=4.0;
        fireRate=1.6;
        upgraded=true;
        setPixmap(QPixmap("resources/towers/level1/KnowledgeTree_upgrade.png"));
        if(fireTimer)
        {
            fireTimer->stop();
            fireTimer->start(1000*fireRate);
        }
    }
}
