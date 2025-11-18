#include "widget_level_loading.h"
#include "ui_widget_level_loading.h"

widget_level_loading::widget_level_loading(int type,QVector<QString> &tips,int duration,QWidget *parent)
    :level_type(type)
    ,auto_widget(parent)
    , ui(new Ui::widget_level_loading)
    ,tips_(tips)
    ,map_picture({":/map/resources/map/first.png",
                   ":/map/resources/map/second.png",
                   ":/map/resources/map/third.png"})
    ,background({":/background/resources/background/first_background.png",
                  ":/background/resources/background/second_background.png",
                  ":/background/resources/background/third_background.png"})
    ,icon({{":/towers/resources/towers/level1/InspirationBulb.png",":/map_items/resources/map_items/first/scratchpaper.png",":/map_items/resources/map_items/first/stationery.png",":/towers/resources/towers/level1/FishingCatPillow.png"},
           {":/towers/resources/towers/level2/PettingCatTime.png",":/map_items/resources/map_items/second/withered_flowers.png",":/map_items/resources/map_items/second/broken_friendship.png",":/towers/resources/towers/level2/WarmMemories_upgrade.png"},
           {":/map_items/resources/map_items/third/start_block.png",":/map_items/resources/map_items/first/book.png",":/map_items/resources/map_items/second/broken_friendship.png",":/map_items/resources/map_items/third/end_block.png"}})
    , m_animationDistance(20)
    , m_animationDuration(2000)
    , m_staggerDelay(200)
    , m_tipTimer(nullptr)
{
    ui->setupUi(this);
    bar=ui->progressBar;
    // 1. 允许 QLabel 自动换行
    ui->tip->setWordWrap(true);
    // 2. (推荐) 让文本从顶部开始显示，而不是垂直居中
    ui->tip->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    ui->background->setPixmap(background[type]);
    ui->map->setPixmap(map_picture[type]);
    QVector<QPushButton *> icons={ui->icon1,ui->icon2,ui->icon3,ui->icon4};
    for(int i=0;i<4;i++){
        icons[i]->setIcon(QIcon(icon[type][i]));
        m_originalPositions.append(icons[i]->pos());
    }
    createSequentialStaggeredFloating(icons);
    start_loadding(duration);

    m_dynamicPortraitPaths.push_back(":/resource/resources/resource/sleep.png");
    m_dynamicPortraitPaths.push_back(":/portrait/resources/portrait/wu_sa_qi.jpg");
    int random = QRandomGenerator::global()->bounded(tips_.size());
    ui->tip->setText(tips_[random]);

    // 5. 创建、连接并启动新的 tip 计时器
    m_tipTimer = new QTimer(this); // 'this' 作为父对象
    m_tipTimer->setInterval(2000); // 2000ms = 2 秒

    connect(m_tipTimer, &QTimer::timeout, this, [=](){
        // 确保列表不为空
        if (tips_.isEmpty()) return;

        // 每 2 秒，挑选一条新的随机 tip
        int newRandom = QRandomGenerator::global()->bounded(tips_.size());
        ui->tip->setText(tips_[newRandom]);
        if (tips[newRandom] == "很外向，说悄悄话都要用音响。") {
            ui->portrait->setIcon(QIcon(m_dynamicPortraitPaths[1]));
        }else {
            ui->portrait->setIcon(QIcon(m_dynamicPortraitPaths[0]));
        }
    });
    m_tipTimer->start();
    initialSize = this->size(); // 从ui文件中获取的初始尺寸
    ui->portrait->setIcon(QIcon(m_dynamicPortraitPaths[0]));

    // 保存各个组件的初始几何信息
    initialGeometries[ui->background] = ui->background->geometry();
    initialGeometries[ui->tip] = ui->tip->geometry();
    initialGeometries[ui->tip_label] = ui->tip_label->geometry();
    initialGeometries[ui->icon1] = ui->icon1->geometry();
    initialGeometries[ui->icon2] = ui->icon2->geometry();
    initialGeometries[ui->icon3] = ui->icon3->geometry();
    initialGeometries[ui->icon4] = ui->icon4->geometry();
    initialGeometries[ui->map] = ui->map->geometry();
    initialGeometries[ui->progressBar] = ui->progressBar->geometry();
    initialGeometries[ui->portrait] = ui->portrait->geometry();

    // 保存图标按钮的初始图标大小
    initialIconSizes[ui->icon1] = ui->icon1->iconSize();
    initialIconSizes[ui->icon2] = ui->icon2->iconSize();
    initialIconSizes[ui->icon3] = ui->icon3->iconSize();
    initialIconSizes[ui->icon4] = ui->icon4->iconSize();
    initialIconSizes[ui->portrait] = ui->portrait->iconSize();
}

widget_level_loading::~widget_level_loading()
{
    // 清理所有动画资源
    for (auto anim : m_animations) {
        if (anim) {
            anim->stop();
            delete anim;
        }
    }
    m_animations.clear();
    delete ui;
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
            if (m_tipTimer) {
                m_tipTimer->stop();
            }
        }
    });
}

void widget_level_loading::createSequentialStaggeredFloating(const QVector<QPushButton*> &btns, int distance, int duration, int staggerDelay)
{
    m_animationDistance = distance;
    m_animationDuration = duration;
    m_staggerDelay = staggerDelay;
    
    // 停止并清除现有动画
    for (auto anim : m_animations) {
        if (anim) {
            anim->stop();
            anim->deleteLater();
        }
    }
    m_animations.clear();
    
    for (int i = 0; i < btns.size(); ++i) {
        auto *btn = btns[i];
        if (!btn) continue;

        QPoint original = btn->pos();
        
        // 为每个按钮创建独立的动画序列
        QSequentialAnimationGroup *sequence = new QSequentialAnimationGroup(btn);
        m_animations.append(sequence);

        // 添加初始延迟（每个按钮递增）
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

void widget_level_loading::resizeEvent(QResizeEvent *event)
{
    auto_widget::resizeEvent(event);
    
    // 重新启动浮动动画以适应新的位置
    QVector<QPushButton *> icons={ui->icon1,ui->icon2,ui->icon3,ui->icon4};
    createSequentialStaggeredFloating(icons, m_animationDistance, m_animationDuration, m_staggerDelay);
}
