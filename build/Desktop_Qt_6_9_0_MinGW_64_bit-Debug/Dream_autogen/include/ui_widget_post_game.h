/********************************************************************************
** Form generated from reading UI file 'widget_post_game.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_POST_GAME_H
#define UI_WIDGET_POST_GAME_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_widget_post_game
{
public:
    QLabel *label;

    void setupUi(QWidget *widget_post_game)
    {
        if (widget_post_game->objectName().isEmpty())
            widget_post_game->setObjectName("widget_post_game");
        widget_post_game->resize(400, 300);
        label = new QLabel(widget_post_game);
        label->setObjectName("label");
        label->setGeometry(QRect(0, 0, 400, 300));

        retranslateUi(widget_post_game);

        QMetaObject::connectSlotsByName(widget_post_game);
    } // setupUi

    void retranslateUi(QWidget *widget_post_game)
    {
        widget_post_game->setWindowTitle(QCoreApplication::translate("widget_post_game", "Form", nullptr));
        label->setText(QCoreApplication::translate("widget_post_game", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class widget_post_game: public Ui_widget_post_game {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_POST_GAME_H
