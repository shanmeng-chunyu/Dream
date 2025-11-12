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
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include "IconButton.h"

QT_BEGIN_NAMESPACE

class Ui_widget_post_game
{
public:
    QLabel *label;
    IconButton *repeat;
    IconButton *next;
    QLabel *description;
    QLabel *description_2;
    QLabel *stability;
    QLabel *kill_nums;

    void setupUi(QWidget *widget_post_game)
    {
        if (widget_post_game->objectName().isEmpty())
            widget_post_game->setObjectName("widget_post_game");
        widget_post_game->resize(400, 400);
        label = new QLabel(widget_post_game);
        label->setObjectName("label");
        label->setGeometry(QRect(0, 0, 400, 300));
        label->setPixmap(QPixmap(QString::fromUtf8(":/frame/resources/frame/victory.png")));
        repeat = new IconButton(widget_post_game);
        repeat->setObjectName("repeat");
        repeat->setGeometry(QRect(80, 300, 115, 55));
        repeat->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/button/resources/button/win_repeat.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        repeat->setIcon(icon);
        repeat->setIconSize(QSize(120, 60));
        next = new IconButton(widget_post_game);
        next->setObjectName("next");
        next->setGeometry(QRect(200, 300, 115, 55));
        next->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/button/resources/button/win_continue.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        next->setIcon(icon1);
        next->setIconSize(QSize(120, 60));
        description = new QLabel(widget_post_game);
        description->setObjectName("description");
        description->setGeometry(QRect(100, 150, 111, 51));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\271\274\345\234\206")});
        font.setPointSize(16);
        font.setBold(true);
        font.setItalic(false);
        description->setFont(font);
        description->setStyleSheet(QString::fromUtf8("color:grey"));
        description_2 = new QLabel(widget_post_game);
        description_2->setObjectName("description_2");
        description_2->setGeometry(QRect(100, 190, 121, 51));
        description_2->setFont(font);
        description_2->setStyleSheet(QString::fromUtf8("color:grey"));
        stability = new QLabel(widget_post_game);
        stability->setObjectName("stability");
        stability->setGeometry(QRect(230, 150, 111, 51));
        stability->setFont(font);
        stability->setStyleSheet(QString::fromUtf8("color:grey"));
        kill_nums = new QLabel(widget_post_game);
        kill_nums->setObjectName("kill_nums");
        kill_nums->setGeometry(QRect(230, 190, 111, 51));
        kill_nums->setFont(font);
        kill_nums->setStyleSheet(QString::fromUtf8("color:grey"));

        retranslateUi(widget_post_game);

        QMetaObject::connectSlotsByName(widget_post_game);
    } // setupUi

    void retranslateUi(QWidget *widget_post_game)
    {
        widget_post_game->setWindowTitle(QCoreApplication::translate("widget_post_game", "Form", nullptr));
        label->setText(QString());
        repeat->setText(QString());
        next->setText(QString());
        description->setText(QCoreApplication::translate("widget_post_game", "\347\250\263 \345\256\232 \345\272\246\357\274\232", nullptr));
        description_2->setText(QCoreApplication::translate("widget_post_game", "\346\266\210\347\201\255\346\200\252\347\211\251\346\225\260\357\274\232", nullptr));
        stability->setText(QCoreApplication::translate("widget_post_game", "0", nullptr));
        kill_nums->setText(QCoreApplication::translate("widget_post_game", "0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class widget_post_game: public Ui_widget_post_game {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_POST_GAME_H
