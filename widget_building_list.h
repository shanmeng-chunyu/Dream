#ifndef WIDGET_BUILDING_LIST_H
#define WIDGET_BUILDING_LIST_H

#include "auto_widget.h"
#include <QVector>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

namespace Ui {
class widget_building_list;
}

class widget_building_list : public auto_widget
{
    Q_OBJECT

public:
    //传入关卡，从0开始。传入是否是升级某个塔，如果不是，后面的数组大小为4；如果是，后面的数组大小为1。传入塔的名字，图片路径，价格
    explicit widget_building_list(int level,bool upgrade,QVector<QString> &name,QVector<QString> &pixmap,QVector<QString> &price,QWidget *parent = nullptr);
    ~widget_building_list();

signals:
    void buy(int type); //点击购买后传出种类的序号，按传入的顺序

private:
    Ui::widget_building_list *ui;
    QVector<QString> background;
    QVector<QString> name_;
    QVector<QString> pixmap_;
    QVector<QString> price_;
};

#endif // WIDGET_BUILDING_LIST_H
