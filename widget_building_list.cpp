#include "widget_building_list.h"
#include "ui_widget_building_list.h"

widget_building_list::widget_building_list(int level,int resource_value,bool upgrade,QVector<QString> &name,QVector<QString> &pixmap,QVector<QString> &price,QWidget *parent)
    : auto_widget(parent)
    , ui(new Ui::widget_building_list)
    ,background({":/frame/resources/frame/building1.png",":/frame/resources/frame/building2.png",":/frame/resources/frame/building3.png"})
    ,name_(name),pixmap_(pixmap),price_(price)
    ,resource({":/resource/resources/resource/Inspiration.png",":/resource/resources/resource/Courage.png",":/resource/resources/resource/Comfort.png"})
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    ui->background->setPixmap(QPixmap(background[level]));
    ui->resource_value->display(resource_value);

    QTimer *timer=new QTimer(this);
    timer->setInterval(1000);
    connect(timer,&QTimer::timeout,this,[=]{
        ui->resource_value->setStyleSheet("background-color:white;"
            "color:black;"
            "border-radius: 6px;"
            "padding: 5px;            ");
        timer->stop();
    });

    QVector<QLabel*> l_name({ui->name1,ui->name2,ui->name3,ui->name4});
    QVector<QPushButton*> btn_tower({ui->tower1,ui->tower2,ui->tower3,ui->tower4});
    QVector<QPushButton*> btn_price({ui->price1,ui->price2,ui->price3,ui->price4});
    for(auto &p:btn_price) p->setIcon(QPixmap(resource[level]));
    ui->resource->setIcon(QPixmap(resource[level]));

    if(upgrade){
        for(int i=0;i<4;i++){
            l_name[i]->hide();btn_tower[i]->hide();btn_price[i]->hide();
        }
        ui->name0->setText(name[0]);
        ui->tower0->setIcon(QPixmap(pixmap[0]));
        ui->price0->setText(price_[0]);
        connect(ui->price0,&QPushButton::clicked,this,[=]{
            if(resource_value>=price_[0].toInt()){
                emit widget_building_list::buy(0);
                close();
            }
            else{
                timer->start();
                ui->resource_value->setStyleSheet("background-color:white;"
                                                  "color:red;"
                                                  "border-radius: 6px;"
                                                  "padding: 5px;            ");
            }
        });
    }
    else{
        ui->name0->hide();ui->tower0->hide();ui->price0->hide();
        for(int i=0;i<4;i++){
            l_name[i]->setText(name_[i]);
            btn_tower[i]->setIcon(QPixmap(pixmap_[i]));
            btn_price[i]->setText(price_[i]);
            connect(btn_price[i],&QPushButton::clicked,this,[=](){
                if(resource_value>=price_[i].toInt()){
                    emit widget_building_list::buy(i);
                    close();
                }
                else{
                    timer->start();
                    ui->resource_value->setStyleSheet("background-color:white;"
                                                      "color:red;"
                                                      "border-radius: 6px;"
                                                      "padding: 5px;            ");
                }
            });
        }
    }


    connect(ui->cancel,&QPushButton::clicked,this,&QWidget::close);

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
