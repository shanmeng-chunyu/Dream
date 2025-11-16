#ifndef WIDGET_MENU_H
#define WIDGET_MENU_H

#include <QWidget>
#include <auto_widget.h>

namespace Ui {
class widget_menu;
}

class widget_menu : public auto_widget
{
    Q_OBJECT

public:
    explicit widget_menu(QWidget *parent = nullptr);
    ~widget_menu();

signals:
    //按钮按下触发的信号
    void choose_level(); //选择关卡
    void reference_book(); //图鉴
    void exit_game(); //退出游戏

private:
    Ui::widget_menu *ui;

};

#endif // WIDGET_MENU_H
