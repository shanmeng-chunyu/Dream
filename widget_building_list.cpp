#include "widget_building_list.h"
#include "ui_widget_building_list.h"

widget_building_list::widget_building_list(int level,int resource_value,bool upgrade,QVector<QString> &name,QVector<QString> &pixmap,QVector<QString> &price,QWidget *parent)
    : auto_widget(parent)
    , ui(new Ui::widget_building_list)
    ,background({":/frame/resources/frame/building1.png",":/frame/resources/frame/building2.png",":/frame/resources/frame/building3.png"})
    ,name_(name),pixmap_(pixmap),price_(price)
    ,resource({":/resource/resources/resource/Inspiration.png",":/resource/resources/resource/Comfort.png",":/resource/resources/resource/Courage.png"})
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
        int itemsToShow = name_.size();

        // 1. 定义我们希望的 x 坐标
        // (这些坐标是基于您 .ui 文件中的 "tower" 按钮位置)
        QVector<int> target_x_name;
        QVector<int> target_x_tower;
        QVector<int> target_x_price;

        if (itemsToShow == 1) {
            // 居中 1 个项目 (使用 tower0 的 x=360)
            target_x_name = {360};
            target_x_tower = {360};
            target_x_price = {360};
        } else if (itemsToShow == 2) {
            // 居中 2 个项目 (使用 tower2 的 x=300 和 tower3 的 x=420)
            target_x_name = {300, 420};
            target_x_tower = {300, 420};
            target_x_price = {300, 420};
        } else {
            // 默认 4 个项目 (使用 tower1,2,3,4 的 180, 300, 420, 540)
            target_x_name = {180, 300, 420, 540};
            target_x_tower = {180, 300, 420, 540};
            target_x_price = {180, 300, 420, 540};
        }

        // 2. 遍历所有4个UI槽位
        for(int i = 0; i < 4; i++) {

            if (i < itemsToShow) {
                // 如果当前索引 (i) 小于我们传入的选项数，则设置并显示它
                l_name[i]->setText(name_[i]);
                btn_tower[i]->setIcon(QPixmap(pixmap_[i]));
                btn_price[i]->setText(price_[i]);

                // 3. (新) 移动到我们计算好的 x 坐标
                //    我们只修改 X 坐标，Y 坐标保持 .ui 中的原样
                l_name[i]->move(target_x_name[i], l_name[i]->y());
                btn_tower[i]->move(target_x_tower[i], btn_tower[i]->y());
                btn_price[i]->move(target_x_price[i], btn_price[i]->y());

                // 确保它们是可见的
                l_name[i]->show();
                btn_tower[i]->show();
                btn_price[i]->show();

                // 4. (新) 检查是否是“出售”选项（价格以+开头）
                bool isSellOption = price_[i].startsWith("+");

                connect(btn_price[i], &QPushButton::clicked, this, [=](){

                    // 5. (新) 如果是出售，则跳过资源检查
                    if(isSellOption || resource_value >= price_[i].toInt()){
                        emit widget_building_list::buy(i); // 发出正确的索引 (0 或 1)
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

            } else {
                // 如果 'i' 超出了我们的选项数 (例如 i=2 或 i=3)，则隐藏这些多余的槽位
                l_name[i]->hide();
                btn_tower[i]->hide();
                btn_price[i]->hide();
            }
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
