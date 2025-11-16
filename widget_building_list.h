#ifndef WIDGET_BUILDING_LIST_H
#define WIDGET_BUILDING_LIST_H

#include "auto_widget.h"
#include <QVector>
#include <QString>
#include <QTimer>

namespace Ui {
class widget_building_list;
}

class widget_building_list : public auto_widget
{
    Q_OBJECT

public:
    explicit widget_building_list(int level,//传入关卡，从0开始。
                                  int resource_value,//传入现有资源量
                                  bool upgrade,//传入是否是升级某个塔，如果不是，后面的数组大小为4；如果是，后面的数组大小为1。
                                  QVector<QString> &name,//塔的名字
                                  QVector<QString> &pixmap,//图片路径
                                  QVector<QString> &price,//价格
                                  QWidget *parent = nullptr);
    ~widget_building_list();


signals:
    void buy(int type); //点击购买后传出种类的序号，按传入的顺序。

private:
    Ui::widget_building_list *ui;
    QVector<QString> background;
    QVector<QString> name_;
    QVector<QString> pixmap_;
    QVector<QString> price_;
    QVector<QString> resource;
};

#endif // WIDGET_BUILDING_LIST_H
