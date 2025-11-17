#include "widget_ingame.h"
#include "ui_widget_ingame.h"
#include <QDebug>

widget_ingame::widget_ingame(int level,QWidget *parent)
    : auto_widget(parent)
    , ui(new Ui::widget_ingame)
{
    ui->setupUi(this);
    progress=ui->progress;
    progress_bar=ui->progressbar;
    resource_value=ui->resource_value;

    QVector<QString> resources={":/resource/resources/resource/Inspiration.png",":/resource/resources/resource/Comfort.png",":/resource/resources/resource/Courage.png"};
    ui->resourse->setIcon(QPixmap(resources[level]));

    connect(ui->menu,&QPushButton::clicked,this,&widget_ingame::pause_menu);
    connect(ui->pause,&QPushButton::clicked,this,[=](){
        if(Pause){
            Pause=0;
            ui->pause->setIcon(QPixmap(":/button/resources/button/pause.png"));
            emit pause();
        }
        else{
            Pause=1;
            ui->pause->setIcon(QPixmap(":/button/resources/button/begin.png"));
            emit begin();
        }
    });
    connect(ui->speed,&QPushButton::clicked,this,[=](){
        if(speed){
            speed=0;
            ui->speed->setIcon(QPixmap(":/button/resources/button/speed.png"));
            emit speed_normal();
        }
        else{
            speed=1;
            ui->speed->setIcon(QPixmap(":/button/resources/button/speed_pressed.png"));
            emit speed_up();
        }
    });

    ui->label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->description->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->progress->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->progressbar->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    ui->label_2->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->stability->setAttribute(Qt::WA_TransparentForMouseEvents, true); // (这是个图标)
    ui->stability_progressbar->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    ui->stability_value->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    ui->resourse->setAttribute(Qt::WA_TransparentForMouseEvents, true); // (这是个图标)
    ui->resource_value->setAttribute(Qt::WA_TransparentForMouseEvents, true);


    initialSize = this->size(); // 从ui文件中获取的初始尺寸

    // 保存各个组件的初始几何信息
    initialGeometries[ui->description] = ui->description->geometry();
    initialGeometries[ui->pause] = ui->pause->geometry();
    initialGeometries[ui->speed] = ui->speed->geometry();
    initialGeometries[ui->menu] = ui->menu->geometry();
    initialGeometries[ui->resourse] = ui->resourse->geometry();
    initialGeometries[ui->stability] = ui->stability->geometry();
    initialGeometries[ui->label] = ui->label->geometry();
    initialGeometries[ui->label_2] = ui->label_2->geometry();
    initialGeometries[ui->progress] = ui->progress->geometry();
    initialGeometries[ui->progressbar] = ui->progressbar->geometry();
    initialGeometries[ui->stability_progressbar] = ui->stability_progressbar->geometry();
    initialGeometries[ui->stability_value] = ui->stability_value->geometry();
    initialGeometries[ui->resource_value] = ui->resource_value->geometry();


    // 保存图标按钮的初始图标大小
    initialIconSizes[ui->pause] = ui->pause->iconSize();
    initialIconSizes[ui->speed] = ui->speed->iconSize();
    initialIconSizes[ui->menu] = ui->menu->iconSize();
    initialIconSizes[ui->resourse] = ui->resourse->iconSize();
    initialIconSizes[ui->stability] = ui->stability->iconSize();
}

widget_ingame::~widget_ingame()
{
    delete ui;
}

void widget_ingame::set_progress(int now,int total){
    progress->setText(QString("%1/%2").arg(QString::number(now)).arg(QString::number(total)));
    progress_bar->setValue(100*now/total);
}

void widget_ingame::set_resource_value(int value){
    resource_value->display(value);
}

