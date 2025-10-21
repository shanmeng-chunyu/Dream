#include "widget_post_game.h"
#include "ui_widget_post_game.h"

widget_post_game::widget_post_game(bool win,QWidget *parent)
    :QWidget(parent)
    , ui(new Ui::widget_post_game)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    if(win){
        setStyleSheet("background: url(:/frame/resources/frame/victory.png) center center no-repeat;");
    }
}

widget_post_game::~widget_post_game()
{
    delete ui;
}
