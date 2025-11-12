#include "widget_menu.h"
#include "ui_widget_menu.h"

widget_menu::widget_menu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::widget_menu)
{
    ui->setupUi(this);
    connect(ui->btn_choose_level,&QPushButton::clicked,this,&widget_menu::choose_level);
    connect(ui->btn_exit_game,&QPushButton::clicked,this,&widget_menu::exit_game);

}

widget_menu::~widget_menu()
{
    delete ui;
}
