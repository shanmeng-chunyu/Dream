#ifndef WIDGET_INGAME_H
#define WIDGET_INGAME_H

#include "auto_widget.h"
#include<QMouseEvent>

namespace Ui {
class widget_ingame;
}

class widget_ingame : public auto_widget
{
    Q_OBJECT

public:
    //传入关卡序号，从0开始
    explicit widget_ingame(int level,QWidget *parent = nullptr);
    ~widget_ingame();

protected:
    void mousePressEvent(QMouseEvent *event) override {
        QWidget::mousePressEvent(event);
        auto p=event->pos();
        emit clicked((double)p.x()/width(),(double)p.y()/height());
    }

signals:
    void pause();//暂停信号
    void begin();//开始信号
    void speed_up();//加速信号
    void speed_normal();//正常速度信号

    void clicked(double x,double y);//点击的信号，传出相对坐标，用于建造防御塔

private:
    Ui::widget_ingame *ui;
    bool Pause=0;
    bool speed=0;
};

#endif // WIDGET_INGAME_H
