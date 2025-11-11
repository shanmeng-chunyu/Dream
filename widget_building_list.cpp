#include "widget_building_list.h"
#include "ui_widget_building_list.h"

widget_building_list::widget_building_list(int level,QWidget *parent)
    : auto_widget(parent)
    , ui(new Ui::widget_building_list)
    ,background({":/frame/resources/frame/building1.png",":/frame/resources/frame/building2.png",":/frame/resources/frame/building3.png"})
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->background->setPixmap(QPixmap(background[level]));

    initialSize = this->size(); // 从ui文件中获取的初始尺寸

    // 保存各个组件的初始几何信息
    initialGeometries[ui->background] = ui->background->geometry();

    // 保存图标按钮的初始图标大小
    // initialIconSizes[ui->icon] = ui->icon->iconSize();
    // initialIconSizes[ui->btn_back] = ui->btn_back->iconSize();
    // initialIconSizes[ui->btn_level] = ui->btn_level->iconSize();
    // initialIconSizes[ui->next] = ui->next->iconSize();
}

widget_building_list::~widget_building_list()
{
    delete ui;
}
