#ifndef WIDGET_POST_GAME_H
#define WIDGET_POST_GAME_H

#include <QWidget>

namespace Ui {
class widget_post_game;
}

class widget_post_game : public QWidget
{
    Q_OBJECT

public:
    //初始化时传入是否胜利、稳定度、消灭怪物数量
    explicit widget_post_game(bool win,int stability,int kill_nums,QWidget *parent = nullptr);
    ~widget_post_game();

signals:
    void repeat();
    void next();
    void backToMenu();

private:
    Ui::widget_post_game *ui;
};

#endif // WIDGET_POST_GAME_H
