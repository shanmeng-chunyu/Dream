#ifndef WIDGET_CHOOSE_LEVEL_H
#define WIDGET_CHOOSE_LEVEL_H

#include "auto_widget.h"
#include <QVector>
#include <Qstring>
#include <QLabel>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QGraphicsOpacityEffect>

namespace Ui {
class widget_choose_level;
}

class widget_choose_level : public auto_widget
{
    Q_OBJECT

public:
    explicit widget_choose_level(QWidget *parent = nullptr);
    ~widget_choose_level();

    //设置各关卡的描述
    void set_descriptions(QVector<QString> d);
    void reset();

signals:
    //按钮按下触发的信号
    void level1();
    void level2();
    void level3();
    void back();

private:
    Ui::widget_choose_level *ui;
    QVector<QString> titles;
    QPushButton *title;
    QVector<QString> descriptions;
    QLabel *description;
    QVector<QString> map;
    QVector<QString> frames;
    QVector<QString> icons;
    int choice=0;

    void smoothImageTransition(QLabel *imageLabel, const QString &newImagePath, int duration = 800);
    void smoothTextTransition(QLabel *label, const QString &newText, int duration = 800);
};

#endif // WIDGET_CHOOSE_LEVEL_H
