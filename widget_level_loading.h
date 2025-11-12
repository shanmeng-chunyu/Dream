#ifndef WIDGET_LEVEL_LOADING_H
#define WIDGET_LEVEL_LOADING_H

#include <QWidget>
#include <QProgressBar>
#include <Qtimer>
#include <QString>
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QPauseAnimation>

namespace Ui {
class widget_level_loading;
}

class widget_level_loading : public QWidget
{
    Q_OBJECT

public:
    //初始化需要给出对应关卡的编号，从0开始
    explicit widget_level_loading(int type,QWidget *parent = nullptr);
    ~widget_level_loading();

    void set_description(QString d); //设置关卡描述

signals:
    void finished(); //加载动画结束

private:
    Ui::widget_level_loading *ui;
    QProgressBar *bar;
    int level_type;
    QLabel *description;
    QVector<QString> map_picture;
    QVector<QString> background;
    QVector<QVector<QString>> icon;

    void start_loadding(int time=8000); //开始加载动画，给出动画持续时间，结束后触发finished信号
    void createSequentialStaggeredFloating(const QVector<QPushButton*> &btns, int distance = 20, int duration = 2000, int staggerDelay = 200);
};

#endif // WIDGET_LEVEL_LOADING_H
