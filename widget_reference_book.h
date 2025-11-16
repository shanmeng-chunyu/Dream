#ifndef WIDGET_REFERENCE_BOOK_H
#define WIDGET_REFERENCE_BOOK_H

#include "auto_widget.h"
#include <QVector>
#include <QPushButton>
#include <QLabel>

namespace Ui {
class widget_reference_book;
}

class widget_reference_book : public auto_widget
{
    Q_OBJECT

public:
    explicit widget_reference_book(QWidget *parent = nullptr);
    ~widget_reference_book();

signals:
    void back_to_menu();

private:
    Ui::widget_reference_book *ui;
    bool tower=0;

    void switch_to_tower(const QVector<QPushButton*> &items);
    void switch_to_monster(const QVector<QPushButton*> &items);
};

#endif // WIDGET_REFERENCE_BOOK_H
