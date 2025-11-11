#ifndef KNOWLEDGETREE_H
#define KNOWLEDGETREE_H

#include "Tower.h"
#include <QString>

class KnowledgeTree:public Tower
{
public:
    explicit KnowledgeTree(QGraphicsItem* parent = nullptr);
    //子弹爆炸的范围攻击通过Bullet实现，此处的攻击范围是属于tower的
    void upgrade()override;
};

#endif // KNOWLEDGETREE_H

