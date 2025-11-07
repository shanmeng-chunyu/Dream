/********************************************************************************
** Form generated from reading UI file 'widget_menu.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_MENU_H
#define UI_WIDGET_MENU_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include "IconButton.h"

QT_BEGIN_NAMESPACE

class Ui_widget_menu
{
public:
    QLabel *background;
    IconButton *btn_choose_level;
    QLabel *label;
    IconButton *btn_exit_game;
    IconButton *title;
    IconButton *btn_choose_level_2;

    void setupUi(QWidget *widget_menu)
    {
        if (widget_menu->objectName().isEmpty())
            widget_menu->setObjectName("widget_menu");
        widget_menu->resize(800, 600);
        widget_menu->setStyleSheet(QString::fromUtf8(""));
        background = new QLabel(widget_menu);
        background->setObjectName("background");
        background->setGeometry(QRect(0, 0, 800, 600));
        background->setPixmap(QPixmap(QString::fromUtf8(":/background/resources/background/menu_.png")));
        btn_choose_level = new IconButton(widget_menu);
        btn_choose_level->setObjectName("btn_choose_level");
        btn_choose_level->setGeometry(QRect(540, 110, 200, 100));
        btn_choose_level->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }\n"
"\n"
"\n"
""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/button/resources/button/button_.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btn_choose_level->setIcon(icon);
        btn_choose_level->setIconSize(QSize(192, 108));
        label = new QLabel(widget_menu);
        label->setObjectName("label");
        label->setGeometry(QRect(30, 70, 741, 461));
        label->setStyleSheet(QString::fromUtf8("QLabel {\n"
"        background-color: rgba(150, 150,150,128); /* \346\265\205\347\201\260 */\n"
"        border-radius: 4px; /* \345\234\206\350\247\222\357\274\210\345\217\257\351\200\211\357\274\211 */\n"
"    }\n"
"    /* \351\274\240\346\240\207\346\202\254\345\201\234\347\212\266\346\200\201\357\274\232\345\212\240\346\267\261\344\270\272\346\267\261\347\201\260\350\211\262 */\n"
""));
        btn_exit_game = new IconButton(widget_menu);
        btn_exit_game->setObjectName("btn_exit_game");
        btn_exit_game->setGeometry(QRect(540, 380, 200, 100));
        btn_exit_game->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }\n"
""));
        btn_exit_game->setIcon(icon);
        btn_exit_game->setIconSize(QSize(192, 108));
        title = new IconButton(widget_menu);
        title->setObjectName("title");
        title->setGeometry(QRect(0, 0, 550, 550));
        title->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }\n"
"\n"
""));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/background/resources/background/title.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        title->setIcon(icon1);
        title->setIconSize(QSize(600, 600));
        btn_choose_level_2 = new IconButton(widget_menu);
        btn_choose_level_2->setObjectName("btn_choose_level_2");
        btn_choose_level_2->setGeometry(QRect(540, 240, 200, 100));
        btn_choose_level_2->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }\n"
"\n"
"\n"
""));
        btn_choose_level_2->setIcon(icon);
        btn_choose_level_2->setIconSize(QSize(192, 108));
        background->raise();
        label->raise();
        btn_choose_level->raise();
        btn_exit_game->raise();
        title->raise();
        btn_choose_level_2->raise();

        retranslateUi(widget_menu);

        QMetaObject::connectSlotsByName(widget_menu);
    } // setupUi

    void retranslateUi(QWidget *widget_menu)
    {
        widget_menu->setWindowTitle(QCoreApplication::translate("widget_menu", "menu", nullptr));
        background->setText(QString());
        btn_choose_level->setText(QString());
        label->setText(QString());
        btn_exit_game->setText(QString());
        title->setText(QString());
        btn_choose_level_2->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class widget_menu: public Ui_widget_menu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_MENU_H
