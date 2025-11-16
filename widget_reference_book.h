#ifndef WIDGET_REFERENCE_BOOK_H
#define WIDGET_REFERENCE_BOOK_H

#include "auto_widget.h"
#include <QVector>
#include <QPushButton>
#include <QLabel>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QDebug>

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
    QVector<QPushButton*> items;
    int chosen=-1;
    QJsonArray towers;
    QJsonArray enemies;

    QJsonArray get_json_array(QString path,QString name_array);
    void switch_to_tower();
    void switch_to_enemy();
};

#endif // WIDGET_REFERENCE_BOOK_H
