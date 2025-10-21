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
    explicit widget_post_game(bool win,QWidget *parent = nullptr);
    ~widget_post_game();

private:
    Ui::widget_post_game *ui;
};

#endif // WIDGET_POST_GAME_H
