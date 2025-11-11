#include "KnowledgeTree.h"

KnowledgeTree::KnowledgeTree(QGraphicsItem* parent):Tower(160,3.5,2.0,200,250,QPixmap( ":/towers/resources/towers/level1/KnowledgeTree.png"),parent){}

void KnowledgeTree::upgrade()
{
    if(!upgraded)
    {
        damage=200;
        range=4.0;
        fireRate=1.6;
        fireInterval=fireRate*60;
        originalFireInterval=fireInterval;
        originalDamage=damage;
        if(fireCount>fireInterval)
            fireCount=fireInterval;
        upgraded=true;
        setPixmap(QPixmap(":/towers/resources/towers/level1/KnowledgeTree_upgrade.png"));
    }
}
