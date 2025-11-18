#include "widget_post_game.h"
#include "ui_widget_post_game.h"

widget_post_game::widget_post_game(bool win,int stability,int kill_nums,QWidget *parent)
    :QWidget(parent)
    , ui(new Ui::widget_post_game)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    //setStyleSheet("background: transparent;");
    if(win){
        //setStyleSheet("background: url(:/frame/resources/frame/victory.png) center center no-repeat;");
        ui->label->setPixmap(QString(":/frame/resources/frame/victory.png"));
        ui->repeat->setIcon(QPixmap(":/button/resources/button/win_repeat.png"));
        ui->next->setIcon(QPixmap(":/button/resources/button/win_continue.png"));
    }
    else{
        //setStyleSheet("background: url(:/frame/resources/frame/defeat.png) center center no-repeat;");
        ui->label->setPixmap(QString(":/frame/resources/frame/defeat.png"));
        ui->repeat->setIcon(QPixmap(":/button/resources/button/lose_repeat.png"));
        ui->next->setIcon(QPixmap(":/button/resources/button/lose_return.png"));
    }
    ui->stability->setText(QString("%1").arg(QString::number(stability)));
    ui->kill_nums->setText(QString("%1").arg(QString::number(kill_nums)));

    connect(ui->repeat,&QPushButton::clicked,this,&widget_post_game::repeat);
    connect(ui->next,&QPushButton::clicked,this,&widget_post_game::next);

}

widget_post_game::~widget_post_game()
{
    delete ui;
}


