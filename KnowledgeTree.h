#ifndef KNOWLEDGETREE_H
#define KNOWLEDGETREE_H

#include "Tower.h"
#include <QString>

class KnowledgeTree:public Tower
{
public:
    explicit KnowledgeTree(QGraphicsItem* parent = nullptr);
    //范围攻击通过子弹实现
    void upgrade()override;
private:
    QString upgradedPixmapPath;
};

#endif // KNOWLEDGETREE_H
