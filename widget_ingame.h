#ifndef WIDGET_INGAME_H
#define WIDGET_INGAME_H

#include "auto_widget.h"
#include <QMouseEvent>
#include <QLabel>
#include <QProgressBar>
#include <QLCDNumber>

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

    //设置波数 格式："1/10" 即十波中的第一波
    void set_progress(int now,int total);
    //设置资源量
    void set_resource_value(int value);

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
    QLabel *progress;
    QProgressBar *progress_bar;
    QLCDNumber *resource_value;
};

#endif // WIDGET_INGAME_H
