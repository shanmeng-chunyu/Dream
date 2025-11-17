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
    ui->resource->setPixmap(QPixmap(resource[level]));

    auto connectSignals = [&](QPushButton* button, QPushButton* priceButton, int itemIndex) {

        // 检查价格字符串 (处理 "100" 和 "+100" 两种情况)
        bool conversionOk = false;
        int priceValue = price_[itemIndex].toInt(&conversionOk);

        if (!conversionOk) {
            // 如果转换失败 (可能是 "+200" 这种格式)，尝试去掉第一个字符再转换
            priceValue = price_[itemIndex].mid(1).toInt(&conversionOk);
        }
        if (!conversionOk) {
            priceValue = 99999; // 设置一个极大值作为安全回退
        }

        // 定义点击时执行的动作
        auto onClick = [=, this]() { // 捕获所需变量
            if(resource_value >= priceValue){
                emit widget_building_list::buy(itemIndex); // 发射正确的索引
                close();
            }
            else{
                timer->start();
                ui->resource_value->setStyleSheet("background-color:white;"
                                                  "color:red;"
                                                  "border-radius: 6px;"
                                                  "padding: 5px;            ");
            }
        };

        // 连接两个按钮的点击事件
        connect(button, &QPushButton::clicked, this, onClick);
        connect(priceButton, &QPushButton::clicked, this, onClick);
    };


    // 2. 获取传入的选项数量
    int itemsToShow = name_.size();

    // 3. 根据选项数量，选择不同的布局
    if (itemsToShow == 1)
    {
        // --- 情况 1：只有 1 个选项 (例如 "出售") ---
        // 我们使用居中的 "slot 0"

        // a. 隐藏 4 宫格
        for(int i=0; i<4; i++) {
            l_name[i]->hide(); btn_tower[i]->hide(); btn_price[i]->hide();
        }

        // b. 设置并显示 "slot 0"
        ui->name0->setText(name_[0]);
        ui->tower0->setIcon(QPixmap(pixmap_[0]));
        ui->price0->setText(price_[0]);

        connectSignals(ui->tower0, ui->price0, 0); // 连接索引 0

        ui->name0->show(); ui->tower0->show(); ui->price0->show();
    }
    else if (itemsToShow == 2)
    {
        // --- 情况 2：有 2 个选项 (例如 "升级" 和 "出售") ---
        // 我们使用居中的 "slot 2" 和 "slot 3"

        // a. 隐藏不用的槽位 (0, 1, 4)
        ui->name0->hide(); ui->tower0->hide(); ui->price0->hide();
        l_name[0]->hide(); btn_tower[0]->hide(); btn_price[0]->hide(); // 隐藏 slot 1
        l_name[3]->hide(); btn_tower[3]->hide(); btn_price[3]->hide(); // 隐藏 slot 4

        // b. 设置并显示 "slot 2" (l_name[1])，使用数据[0]
        l_name[1]->setText(name_[0]);
        btn_tower[1]->setIcon(QPixmap(pixmap_[0]));
        btn_price[1]->setText(price_[0]);
        connectSignals(btn_tower[1], btn_price[1], 0); // 发射索引 0
        l_name[1]->show(); btn_tower[1]->show(); btn_price[1]->show();

        // c. 设置并显示 "slot 3" (l_name[2])，使用数据[1]
        l_name[2]->setText(name_[1]);
        btn_tower[2]->setIcon(QPixmap(pixmap_[1]));
        btn_price[2]->setText(price_[1]);
        connectSignals(btn_tower[2], btn_price[2], 1); // 发射索引 1
        l_name[2]->show(); btn_tower[2]->show(); btn_price[2]->show();
    }
    else
    {
        // --- 情况 3：有 3 或 4 个选项 (正常的建造菜单) ---
        // 我们使用 4 宫格 "slot 1-4"

        // a. 隐藏 "slot 0"
        ui->name0->hide();ui->tower0->hide();ui->price0->hide();

        // b. 遍历 4 宫格
        for(int i=0; i < 4; i++) {
            if (i < itemsToShow) {
                // 设置并显示
                l_name[i]->setText(name_[i]);
                btn_tower[i]->setIcon(QPixmap(pixmap_[i]));
                btn_price[i]->setText(price_[i]);

                connectSignals(btn_tower[i], btn_price[i], i); // 发射索引 i

                l_name[i]->show(); btn_tower[i]->show(); btn_price[i]->show();
            } else {
                // 隐藏多余的
                l_name[i]->hide(); btn_tower[i]->hide(); btn_price[i]->hide();
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
