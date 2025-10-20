/********************************************************************************
** Form generated from reading UI file 'widget_choose_level.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_CHOOSE_LEVEL_H
#define UI_WIDGET_CHOOSE_LEVEL_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_widget_choose_level
{
public:
    QLabel *background;
    QLabel *label;
    QLabel *frame;
    QPushButton *btn_level;
    QLabel *map_picture;
    QLabel *description;
    QLabel *icon;
    QPushButton *next;

    void setupUi(QWidget *widget_choose_level)
    {
        if (widget_choose_level->objectName().isEmpty())
            widget_choose_level->setObjectName("widget_choose_level");
        widget_choose_level->resize(800, 600);
        background = new QLabel(widget_choose_level);
        background->setObjectName("background");
        background->setGeometry(QRect(0, 0, 800, 600));
        background->setPixmap(QPixmap(QString::fromUtf8(":/background/resources/background/choose_.png")));
        label = new QLabel(widget_choose_level);
        label->setObjectName("label");
        label->setGeometry(QRect(40, 30, 720, 540));
        label->setStyleSheet(QString::fromUtf8("QLabel {\n"
"        background-color: rgba(150, 150,150,128); /* \346\265\205\347\201\260 */\n"
"        border-radius: 4px; /* \345\234\206\350\247\222\357\274\210\345\217\257\351\200\211\357\274\211 */\n"
"    }\n"
"    /* \351\274\240\346\240\207\346\202\254\345\201\234\347\212\266\346\200\201\357\274\232\345\212\240\346\267\261\344\270\272\346\267\261\347\201\260\350\211\262 */\n"
""));
        frame = new QLabel(widget_choose_level);
        frame->setObjectName("frame");
        frame->setGeometry(QRect(260, 330, 300, 300));
        frame->setStyleSheet(QString::fromUtf8(""));
        frame->setPixmap(QPixmap(QString::fromUtf8(":/frame/resources/frame/level1.png")));
        btn_level = new QPushButton(widget_choose_level);
        btn_level->setObjectName("btn_level");
        btn_level->setGeometry(QRect(290, 410, 231, 91));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\271\274\345\234\206")});
        font.setPointSize(30);
        btn_level->setFont(font);
        btn_level->setStyleSheet(QString::fromUtf8("background: transparent;\n"
"border: 1px solid transparent;\n"
"color:white\n"
""));
        map_picture = new QLabel(widget_choose_level);
        map_picture->setObjectName("map_picture");
        map_picture->setGeometry(QRect(80, 60, 400, 300));
        map_picture->setStyleSheet(QString::fromUtf8(""));
        map_picture->setFrameShape(QFrame::Shape::Box);
        map_picture->setFrameShadow(QFrame::Shadow::Raised);
        map_picture->setLineWidth(2);
        map_picture->setPixmap(QPixmap(QString::fromUtf8(":/map/resources/map/brief_first.png")));
        description = new QLabel(widget_choose_level);
        description->setObjectName("description");
        description->setGeometry(QRect(520, 70, 191, 281));
        QFont font1;
        font1.setFamilies({QString::fromUtf8("\345\271\274\345\234\206")});
        font1.setPointSize(16);
        description->setFont(font1);
        description->setStyleSheet(QString::fromUtf8("color:white"));
        description->setAlignment(Qt::AlignmentFlag::AlignLeading|Qt::AlignmentFlag::AlignLeft|Qt::AlignmentFlag::AlignTop);
        icon = new QLabel(widget_choose_level);
        icon->setObjectName("icon");
        icon->setGeometry(QRect(100, 410, 128, 128));
        next = new QPushButton(widget_choose_level);
        next->setObjectName("next");
        next->setGeometry(QRect(660, 490, 61, 51));
        QIcon icon1(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend));
        next->setIcon(icon1);
        background->raise();
        label->raise();
        frame->raise();
        map_picture->raise();
        description->raise();
        icon->raise();
        next->raise();
        btn_level->raise();

        retranslateUi(widget_choose_level);

        QMetaObject::connectSlotsByName(widget_choose_level);
    } // setupUi

    void retranslateUi(QWidget *widget_choose_level)
    {
        widget_choose_level->setWindowTitle(QCoreApplication::translate("widget_choose_level", "choose_level", nullptr));
        background->setText(QString());
        label->setText(QString());
        frame->setText(QString());
        btn_level->setText(QCoreApplication::translate("widget_choose_level", "\347\254\254 \344\270\200 \345\205\263", nullptr));
        map_picture->setText(QString());
        description->setText(QCoreApplication::translate("widget_choose_level", "Description", nullptr));
        icon->setText(QString());
        next->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class widget_choose_level: public Ui_widget_choose_level {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_CHOOSE_LEVEL_H
