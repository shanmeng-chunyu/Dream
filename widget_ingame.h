#ifndef WIDGET_INGAME_H
#define WIDGET_INGAME_H

#include "auto_widget.h"
#include <QMouseEvent>
#include <QLabel>
#include <QProgressBar>
#include <QLCDNumber>
#include <QVector>

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
        // 1. 检查鼠标点击的位置是否有子控件（如按钮）
        QWidget* child = childAt(event->pos());

        // 2. 如果 child 不是 nullptr (即点击了按钮)
        if (child) {
            // 3. 让 Qt 正常处理这个按钮点击
            QWidget::mousePressEvent(event);
        }
        else {
            // 4. 如果点击的是“空白”区域
            // 5. 【关键修复】告诉 Qt 忽略这个事件
            event->ignore();
            //    这会让事件“穿透”到 HUD 下方的 QGraphicsView，
            //    从而允许 QGraphicsView 处理它并将其发送给你的 TowerBaseItem。
        }
    }

signals:
    void pause();//暂停信号
    void begin();//开始信号
    void speed_up();//加速信号
    void speed_normal();//正常速度信号
    void pause_menu();//打开暂停菜单栏

    void clicked(double x,double y);//点击的信号，传出相对坐标，用于建造防御塔

private:
    Ui::widget_ingame *ui;
    bool Pause=1;
    bool speed=0;
    QLabel *progress;
    QProgressBar *progress_bar;
    QLCDNumber *resource_value;
};

#endif // WIDGET_INGAME_H
