#include "widget_reference_book.h"
#include "ui_widget_reference_book.h"

widget_reference_book::widget_reference_book(QWidget *parent)
    : auto_widget(parent)
    , ui(new Ui::widget_reference_book)
{
    ui->setupUi(this);

    QVector<QPushButton*> items={
        nullptr,
        ui->item1,ui->item2,ui->item3,ui->item4,ui->item5,ui->item6,
        ui->item7,ui->item8,ui->item9,ui->item10,ui->item11,ui->item12};

    connect(ui->back_to_menu,&QPushButton::clicked,this,&widget_reference_book::back_to_menu);

    //切换按钮的逻辑
    connect(ui->next,&QPushButton::clicked,this,[=](){
        if(tower){
            switch_to_monster(items);
            ui->upgrade->hide();
        }
        else{
            switch_to_tower(items);
            ui->upgrade->show();
        }
    });

    //升级按钮的逻辑

    ui->next->click();
    for(int i=9;i<=12;i++) items[i]->hide();

    initialSize = this->size(); // 从ui文件中获取的初始尺寸

    // 保存各个组件的初始几何信息
    initialGeometries[ui->background] = ui->background->geometry();
    for(int i=1;i<=12;i++) initialGeometries[items[i]] = items[i]->geometry();
    initialGeometries[ui->next] = ui->next->geometry();
    initialGeometries[ui->back_to_menu] = ui->back_to_menu->geometry();
    initialGeometries[ui->upgrade] = ui->upgrade->geometry();
    initialGeometries[ui->label] = ui->label->geometry();
    initialGeometries[ui->label1] = ui->label1->geometry();
    initialGeometries[ui->label2] = ui->label2->geometry();
    initialGeometries[ui->description] = ui->description->geometry();
    initialGeometries[ui->value1] = ui->value1->geometry();
    initialGeometries[ui->value2] = ui->value2->geometry();

    // 保存图标按钮的初始图标大小
    for(int i=1;i<=12;i++) initialIconSizes[items[i]] = items[i]->iconSize();
    initialIconSizes[ui->next] = ui->next->iconSize();
    initialIconSizes[ui->back_to_menu] = ui->back_to_menu->iconSize();
    initialIconSizes[ui->upgrade] = ui->upgrade->iconSize();
}

widget_reference_book::~widget_reference_book()
{
    delete ui;
}

void widget_reference_book::switch_to_tower(const QVector<QPushButton*> &items){

}

void widget_reference_book::switch_to_monster(const QVector<QPushButton*> &items){

}
