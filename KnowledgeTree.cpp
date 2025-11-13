#include "KnowledgeTree.h"

KnowledgeTree::KnowledgeTree(double range,QGraphicsItem* parent):Tower(160,range,2.0,200,250,QPixmap( ":/towers/resources/towers/level1/KnowledgeTree.png"),parent) {
    type = "KnowledgeTree";
}

void KnowledgeTree::upgrade()
{
    if(!upgraded)
    {
        damage=200;
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
