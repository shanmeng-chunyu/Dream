#ifndef WIDGET_BUILDING_LIST_H
#define WIDGET_BUILDING_LIST_H

#include "auto_widget.h"
#include <QVector>
#include <QString>

namespace Ui {
class widget_building_list;
}

class widget_building_list : public auto_widget
{
    Q_OBJECT

public:
    //传入关卡，从0开始
    explicit widget_building_list(int level,QWidget *parent = nullptr);
    ~widget_building_list();

private:
    Ui::widget_building_list *ui;
    QVector<QString> background;
};

#endif // WIDGET_BUILDING_LIST_H
