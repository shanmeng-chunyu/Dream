#ifndef WIDGET_PAUSE_MENU_H
#define WIDGET_PAUSE_MENU_H

#include "auto_widget.h"

namespace Ui {
class widget_pause_menu;
}

class widget_pause_menu : public auto_widget
{
    Q_OBJECT

public:
    explicit widget_pause_menu(QWidget *parent = nullptr);
    ~widget_pause_menu();

signals:
    void back_to_game();
    void back_to_menu();

private:
    Ui::widget_pause_menu *ui;
};

#endif // WIDGET_PAUSE_MENU_H
