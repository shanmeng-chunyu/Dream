#include "widget_menu.h"
#include "ui_widget_menu.h"
#include <QDebug>
widget_menu::widget_menu(QWidget *parent)
    : auto_widget(parent)
    , ui(new Ui::widget_menu)
{
    ui->setupUi(this);

    connect(ui->btn_choose_level,&QPushButton::clicked,this,&widget_menu::choose_level);
    connect(ui->btn_reference_book,&QPushButton::clicked,this,&widget_menu::reference_book);
    connect(ui->btn_exit_game,&QPushButton::clicked,this,&widget_menu::exit_game);

    // 保存初始尺寸
    initialSize = this->size(); // 从ui文件中获取的初始尺寸
    
    // 保存各个组件的初始几何信息
    initialGeometries[ui->background] = ui->background->geometry();
    initialGeometries[ui->label] = ui->label->geometry();
    initialGeometries[ui->title] = ui->title->geometry();
    initialGeometries[ui->btn_choose_level] = ui->btn_choose_level->geometry();
    initialGeometries[ui->btn_reference_book] = ui->btn_reference_book->geometry();
    initialGeometries[ui->btn_exit_game] = ui->btn_exit_game->geometry();

    // 保存图标按钮的初始图标大小
    initialIconSizes[ui->title] = ui->title->iconSize();
    initialIconSizes[ui->btn_choose_level] = ui->btn_choose_level->iconSize();
    initialIconSizes[ui->btn_reference_book] = ui->btn_reference_book->iconSize();
    initialIconSizes[ui->btn_exit_game] = ui->btn_exit_game->iconSize();

}

widget_menu::~widget_menu()
{
    delete ui;
}


