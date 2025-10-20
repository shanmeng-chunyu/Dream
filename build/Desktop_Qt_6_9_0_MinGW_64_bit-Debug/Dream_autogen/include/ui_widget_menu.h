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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_widget_menu
{
public:
    QLabel *background;
    QPushButton *btn_choose_level;
    QLabel *label;
    QLabel *text1;
    QPushButton *btn_exit_game;
    QLabel *text2;

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
        btn_choose_level = new QPushButton(widget_menu);
        btn_choose_level->setObjectName("btn_choose_level");
        btn_choose_level->setGeometry(QRect(540, 180, 192, 108));
        btn_choose_level->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }\n"
""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/button/resources/button/button_.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        btn_choose_level->setIcon(icon);
        btn_choose_level->setIconSize(QSize(192, 108));
        label = new QLabel(widget_menu);
        label->setObjectName("label");
        label->setGeometry(QRect(470, 90, 311, 431));
        label->setStyleSheet(QString::fromUtf8("QLabel {\n"
"        background-color: rgba(150, 150,150,128); /* \346\265\205\347\201\260 */\n"
"        border-radius: 4px; /* \345\234\206\350\247\222\357\274\210\345\217\257\351\200\211\357\274\211 */\n"
"    }\n"
"    /* \351\274\240\346\240\207\346\202\254\345\201\234\347\212\266\346\200\201\357\274\232\345\212\240\346\267\261\344\270\272\346\267\261\347\201\260\350\211\262 */\n"
""));
        text1 = new QLabel(widget_menu);
        text1->setObjectName("text1");
        text1->setGeometry(QRect(560, 210, 151, 51));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\271\274\345\234\206")});
        font.setPointSize(18);
        text1->setFont(font);
        text1->setStyleSheet(QString::fromUtf8("color:white\n"
""));
        text1->setAlignment(Qt::AlignmentFlag::AlignCenter);
        btn_exit_game = new QPushButton(widget_menu);
        btn_exit_game->setObjectName("btn_exit_game");
        btn_exit_game->setGeometry(QRect(540, 320, 192, 108));
        btn_exit_game->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }\n"
""));
        btn_exit_game->setIcon(icon);
        btn_exit_game->setIconSize(QSize(192, 108));
        text2 = new QLabel(widget_menu);
        text2->setObjectName("text2");
        text2->setGeometry(QRect(560, 350, 151, 51));
        text2->setFont(font);
        text2->setStyleSheet(QString::fromUtf8("color:white\n"
""));
        text2->setAlignment(Qt::AlignmentFlag::AlignCenter);
        background->raise();
        label->raise();
        btn_choose_level->raise();
        btn_exit_game->raise();
        text1->raise();
        text2->raise();

        retranslateUi(widget_menu);

        QMetaObject::connectSlotsByName(widget_menu);
    } // setupUi

    void retranslateUi(QWidget *widget_menu)
    {
        widget_menu->setWindowTitle(QCoreApplication::translate("widget_menu", "menu", nullptr));
        background->setText(QString());
        btn_choose_level->setText(QString());
        label->setText(QString());
        text1->setText(QCoreApplication::translate("widget_menu", "\351\200\211 \346\213\251 \345\205\263 \345\215\241", nullptr));
        btn_exit_game->setText(QString());
        text2->setText(QCoreApplication::translate("widget_menu", "\351\200\200 \345\207\272 \346\270\270 \346\210\217", nullptr));
    } // retranslateUi

};

namespace Ui {
    class widget_menu: public Ui_widget_menu {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_MENU_H
