#include "widget_choose_level.h"
#include "ui_widget_choose_level.h"

widget_choose_level::widget_choose_level(QWidget *parent)
    : auto_widget(parent)
    , ui(new Ui::widget_choose_level)
    ,titles({"第 一 关","第 二 关","第 三 关"})
    ,descriptions(3,"Description")
    ,map({":/map/resources/map/brief_first.png",":/map/resources/map/brief_second.png",":/map/resources/map/brief_third.png"})
    ,frames({":/frame/resources/frame/level1.png",":/frame/resources/frame/level2.png",":/frame/resources/frame/level3.png"})
    ,icons({":/map_items/resources/map_items/first/books_100_100.png",":/map_items/resources/map_items/second/photo_100_100.png",":/map_items/resources/map_items/third/third_start_100_100.png"})
{
    ui->setupUi(this);
    title=ui->btn_level;
    description=ui->description;
    connect(ui->next,&QPushButton::clicked,this,[=]{
        choice=(choice+1)%3;
        smoothImageTransition(ui->map_picture,map[choice]);
        smoothTextTransition(ui->description,descriptions[choice]);
        ui->btn_level->setIcon(QIcon(frames[choice]));
        ui->icon->setIcon(QIcon(icons[choice]));
    });
    connect(ui->btn_level,&QPushButton::clicked,this,[=](){
        switch(choice){
        case 0:emit level1();break;
        case 1:emit level2();break;
        case 2:emit level3();break;
        }
    });
    connect(ui->btn_back,&QPushButton::clicked,this,&widget_choose_level::back);

    initialSize = this->size(); // 从ui文件中获取的初始尺寸

    // 保存各个组件的初始几何信息
    initialGeometries[ui->background] = ui->background->geometry();
    initialGeometries[ui->btn_back] = ui->btn_back->geometry();
    initialGeometries[ui->btn_level] = ui->btn_level->geometry();
    initialGeometries[ui->description] = ui->description->geometry();
    initialGeometries[ui->icon] = ui->icon->geometry();
    initialGeometries[ui->label] = ui->label->geometry();
    initialGeometries[ui->map_picture] = ui->map_picture->geometry();
    initialGeometries[ui->next] = ui->next->geometry();

    // 保存图标按钮的初始图标大小
    initialIconSizes[ui->icon] = ui->icon->iconSize();
    initialIconSizes[ui->btn_back] = ui->btn_back->iconSize();
    initialIconSizes[ui->btn_level] = ui->btn_level->iconSize();
    initialIconSizes[ui->next] = ui->next->iconSize();

}

widget_choose_level::~widget_choose_level()
{
    delete ui;
}


void widget_choose_level::set_descriptions(QVector<QString> d){
    descriptions=d;
    description->setText(d[choice]);
}

void widget_choose_level::smoothImageTransition(QLabel *imageLabel, const QString &newImagePath, int duration ){
    if (!imageLabel || newImagePath.isEmpty()) {
        return;
    }
    // 检查新图片是否存在
    QPixmap newPixmap(newImagePath);
    if (newPixmap.isNull()) {
        qWarning() << "Failed to load image:" << newImagePath;
        return;
    }

    // 创建透明度效果
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(imageLabel);
    imageLabel->setGraphicsEffect(effect);

    // 创建顺序动画组
    QSequentialAnimationGroup *sequence = new QSequentialAnimationGroup;

    // 第一阶段：淡出（50%时间）
    QPropertyAnimation *fadeOut = new QPropertyAnimation(effect, "opacity");
    fadeOut->setDuration(duration / 2);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.2);
    fadeOut->setEasingCurve(QEasingCurve::InOutCubic);

    // 第二阶段：淡入（50%时间）
    QPropertyAnimation *fadeIn = new QPropertyAnimation(effect, "opacity");
    fadeIn->setDuration(duration / 2);
    fadeIn->setStartValue(0.2);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InOutCubic);

    // 在淡出完成后切换图片
    QObject::connect(fadeOut, &QPropertyAnimation::finished, [imageLabel, newPixmap]() {
        // 保持原有尺寸或自适应缩放
        QPixmap scaledPixmap = newPixmap.scaled(imageLabel->size(),
                                                Qt::KeepAspectRatio,
                                                Qt::SmoothTransformation);
        imageLabel->setPixmap(scaledPixmap);
    });

    sequence->addAnimation(fadeOut);
    sequence->addAnimation(fadeIn);
    sequence->start(QAbstractAnimation::DeleteWhenStopped);
}
void widget_choose_level::smoothTextTransition(QLabel *label, const QString &newText, int duration ){
    if (!label || newText.isEmpty()) {
        return;
    }

    // 创建透明度效果
    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(label);
    label->setGraphicsEffect(effect);

    // 创建顺序动画组
    QSequentialAnimationGroup *sequence = new QSequentialAnimationGroup;

    // 第一阶段：淡出（50%时间）
    QPropertyAnimation *fadeOut = new QPropertyAnimation(effect, "opacity");
    fadeOut->setDuration(duration / 2);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::InOutCubic);

    // 第二阶段：淡入（50%时间）
    QPropertyAnimation *fadeIn = new QPropertyAnimation(effect, "opacity");
    fadeIn->setDuration(duration / 2);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InOutCubic);

    // 在淡出完成后切换文字
    QObject::connect(fadeOut, &QPropertyAnimation::finished, [label, newText]() {
        label->setText(newText);
    });

    sequence->addAnimation(fadeOut);
    sequence->addAnimation(fadeIn);
    sequence->start(QAbstractAnimation::DeleteWhenStopped);
}

