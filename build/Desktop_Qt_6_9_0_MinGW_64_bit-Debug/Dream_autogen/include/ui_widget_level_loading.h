/********************************************************************************
** Form generated from reading UI file 'widget_level_loading.ui'
**
** Created by: Qt User Interface Compiler version 6.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_LEVEL_LOADING_H
#define UI_WIDGET_LEVEL_LOADING_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QWidget>
#include "IconButton.h"

QT_BEGIN_NAMESPACE

class Ui_widget_level_loading
{
public:
    QLabel *background;
    QLabel *map;
    QProgressBar *progressBar;
    QLabel *description;
    IconButton *icon1;
    IconButton *icon2;
    IconButton *icon3;
    IconButton *icon4;

    void setupUi(QWidget *widget_level_loading)
    {
        if (widget_level_loading->objectName().isEmpty())
            widget_level_loading->setObjectName("widget_level_loading");
        widget_level_loading->resize(800, 600);
        background = new QLabel(widget_level_loading);
        background->setObjectName("background");
        background->setGeometry(QRect(0, 0, 800, 600));
        background->setPixmap(QPixmap(QString::fromUtf8(":/background/resources/background/first_background.png")));
        map = new QLabel(widget_level_loading);
        map->setObjectName("map");
        map->setGeometry(QRect(200, 90, 400, 300));
        map->setFrameShape(QFrame::Shape::Box);
        map->setFrameShadow(QFrame::Shadow::Raised);
        map->setLineWidth(2);
        map->setPixmap(QPixmap(QString::fromUtf8(":/map/resources/map/brief_first.png")));
        progressBar = new QProgressBar(widget_level_loading);
        progressBar->setObjectName("progressBar");
        progressBar->setGeometry(QRect(160, 530, 500, 25));
        progressBar->setStyleSheet(QString::fromUtf8("QProgressBar {\n"
"    border: 3px solid rgba(220, 220, 220, 1) ;\n"
"    border-radius: 15px;\n"
"    background-color: rgba(255,255,255,0.1);\n"
"    text-align: center;\n"
"    color: white;\n"
"    font-weight: bold;\n"
"}\n"
"\n"
"QProgressBar::chunk {\n"
"    border-radius: 14px;\n"
"    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,\n"
"        stop:0 rgba(116, 235, 213, 0.8),\n"
"        stop:1 rgba(172, 182, 229, 0.8));\n"
"    border: 1px solid rgba(255,255,255,0.5);\n"
"}"));
        progressBar->setValue(0);
        progressBar->setTextVisible(false);
        progressBar->setOrientation(Qt::Orientation::Horizontal);
        description = new QLabel(widget_level_loading);
        description->setObjectName("description");
        description->setGeometry(QRect(160, 440, 481, 61));
        QFont font;
        font.setFamilies({QString::fromUtf8("\345\271\274\345\234\206")});
        font.setPointSize(18);
        description->setFont(font);
        description->setStyleSheet(QString::fromUtf8("color:white"));
        icon1 = new IconButton(widget_level_loading);
        icon1->setObjectName("icon1");
        icon1->setGeometry(QRect(60, 120, 85, 85));
        icon1->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/map_items/resources/map_items/first/draft_100_100.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
        icon1->setIcon(icon);
        icon1->setIconSize(QSize(100, 100));
        icon2 = new IconButton(widget_level_loading);
        icon2->setObjectName("icon2");
        icon2->setGeometry(QRect(60, 280, 85, 85));
        icon2->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }"));
        icon2->setIcon(icon);
        icon2->setIconSize(QSize(100, 100));
        icon3 = new IconButton(widget_level_loading);
        icon3->setObjectName("icon3");
        icon3->setGeometry(QRect(660, 120, 85, 85));
        icon3->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }"));
        icon3->setIcon(icon);
        icon3->setIconSize(QSize(100, 100));
        icon4 = new IconButton(widget_level_loading);
        icon4->setObjectName("icon4");
        icon4->setGeometry(QRect(660, 280, 85, 85));
        icon4->setStyleSheet(QString::fromUtf8("QPushButton {\n"
"        background: transparent;       /* \350\203\214\346\231\257\351\200\217\346\230\216 */\n"
"        border: none;                  /* \346\227\240\350\276\271\346\241\206 */\n"
"    }"));
        icon4->setIcon(icon);
        icon4->setIconSize(QSize(100, 100));

        retranslateUi(widget_level_loading);

        QMetaObject::connectSlotsByName(widget_level_loading);
    } // setupUi

    void retranslateUi(QWidget *widget_level_loading)
    {
        widget_level_loading->setWindowTitle(QCoreApplication::translate("widget_level_loading", "Form", nullptr));
        background->setText(QString());
        map->setText(QString());
        description->setText(QCoreApplication::translate("widget_level_loading", "Description", nullptr));
        icon1->setText(QString());
        icon2->setText(QString());
        icon3->setText(QString());
        icon4->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class widget_level_loading: public Ui_widget_level_loading {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_LEVEL_LOADING_H
