#include "widget_pause_menu.h"
#include "ui_widget_pause_menu.h"

widget_pause_menu::widget_pause_menu(QWidget *parent)
    : auto_widget(parent)
    , ui(new Ui::widget_pause_menu)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    connect(ui->back_to_game,&QPushButton::clicked,this,[=](){
        emit widget_pause_menu::back_to_game();
        close();
    });
    connect(ui->back_to_menu,&QPushButton::clicked,this,[=](){
        emit widget_pause_menu::back_to_menu();
        close();
    });

    initialSize = this->size(); // 从ui文件中获取的初始尺寸

    // 保存各个组件的初始几何信息
    initialGeometries[ui->background] = ui->background->geometry();
    initialGeometries[ui->back_to_game] = ui->back_to_game->geometry();
    initialGeometries[ui->back_to_menu] = ui->back_to_menu->geometry();
    initialGeometries[ui->game_pause] = ui->game_pause->geometry();

    // 保存图标按钮的初始图标大小
    initialIconSizes[ui->back_to_game] = ui->back_to_game->iconSize();
    initialIconSizes[ui->back_to_menu] = ui->back_to_menu->iconSize();
    initialIconSizes[ui->game_pause] = ui->game_pause->iconSize();
}

widget_pause_menu::~widget_pause_menu()
{
    delete ui;
}
