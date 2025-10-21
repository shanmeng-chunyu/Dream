#include "widget_level_loading.h"
#include "ui_widget_level_loading.h"


widget_level_loading::widget_level_loading(int type,QWidget *parent)
    :level_type(type)
    ,QWidget(parent)
    , ui(new Ui::widget_level_loading)
    ,map_picture({":/map/resources/map/brief_first.png",
                   ":/map/resources/map/brief_second.png",
                   ":/map/resources/map/brief_third.png"})
    ,background({":/background/resources/background/first_background.png",
                  ":/background/resources/background/second_background.png",
                  ":/background/resources/background/third_background.png"})
    ,icon({{":/map_items/resources/map_items/first/books_100_100.png",":/map_items/resources/map_items/first/draft_100_100.png",":/map_items/resources/map_items/first/stationary_100_100.png",":/map_items/resources/map_items/first/usb_100_100.png"},
           {":/map_items/resources/map_items/second/box_100_100.png",":/map_items/resources/map_items/second/flower_100_100.png",":/map_items/resources/map_items/second/photo_100_100.png",":/map_items/resources/map_items/second/ring_100_100.png"},
           {":/map_items/resources/map_items/third/third_start_100_100.png",":/map_items/resources/map_items/first/books_100_100.png",":/map_items/resources/map_items/second/photo_100_100.png",":/map_items/resources/map_items/third/third_end_100_100.png"}})
{
    ui->setupUi(this);
    bar=ui->progressBar;
    ui->background->setPixmap(background[type]);
    ui->map->setPixmap(map_picture[type]);
    QVector<QPushButton *> icons={ui->icon1,ui->icon2,ui->icon3,ui->icon4};
    for(int i=0;i<4;i++){
        icons[i]->setIcon(QIcon(icon[type][i]));
    }
    createSequentialStaggeredFloating(icons);
    start_loadding();
}

widget_level_loading::~widget_level_loading()
{
    delete ui;
}

void widget_level_loading::set_description(QString d){
    description->setText(d);
}

void widget_level_loading::start_loadding(int time){
    auto timer=new QTimer(this);
    timer->setInterval(time/10);
    timer->start();
    connect(timer,&QTimer::timeout,this,[=]{
        bar->setValue(bar->value()+10);
        if(bar->value()>=100){
            emit finished();
            timer->stop();
        }
    });
}

void widget_level_loading::createSequentialStaggeredFloating(const QVector<QPushButton*> &btns, int distance,int duration,int staggerDelay)
{
    for (int i = 0; i < btns.size(); ++i) {
        auto *btn = btns[i];
        if (!btn) continue;

        QPoint original = btn->pos();

        // 为每个标签创建独立的动画序列
        QSequentialAnimationGroup *sequence = new QSequentialAnimationGroup(btn);

        // 添加初始延迟（每个标签递增）
        QPauseAnimation *initialDelay = new QPauseAnimation();
        initialDelay->setDuration(i * staggerDelay);
        sequence->addAnimation(initialDelay);

        // 创建上下浮动动画
        QPropertyAnimation *floatUp = new QPropertyAnimation(btn, "pos");
        floatUp->setDuration(duration / 2);
        floatUp->setStartValue(original);
        floatUp->setEndValue(original + QPoint(0,-distance));
        floatUp->setEasingCurve(QEasingCurve::InOutSine);

        QPropertyAnimation *floatDown = new QPropertyAnimation(btn, "pos");
        floatDown->setDuration(duration / 2);
        floatDown->setStartValue(original + QPoint(0,-distance));
        floatDown->setEndValue(original);
        floatDown->setEasingCurve(QEasingCurve::InOutSine);

        // 添加到序列
        sequence->addAnimation(floatUp);
        sequence->addAnimation(floatDown);
        sequence->setLoopCount(-1);

        sequence->start();
    }
}
