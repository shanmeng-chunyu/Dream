#include "widget_ingame.h"
#include "ui_widget_ingame.h"
#include <QDebug>
#include <QPainter>
#include <QFont>
#include <QColor>
#include <QPixmap>
#include <QFontDatabase>

namespace
{
    QString resolvedFontFamily(const QString &preferred)
    {
        const QStringList available = QFontDatabase::families();
        if (available.contains(preferred))
        {
            return preferred;
        }

        static const QStringList fallbacks = {
            QStringLiteral("PingFang SC"),
            QStringLiteral("Hiragino Sans GB"),
            QStringLiteral("Microsoft YaHei"),
            QStringLiteral("Source Han Sans SC"),
            QStringLiteral("Arial")};
        for (const QString &family : fallbacks)
        {
            if (available.contains(family))
            {
                return family;
            }
        }
        return QFont().defaultFamily();
    }

    QIcon makeDimmedIcon(const QString &source, qreal opacity)
    {
        QPixmap base(source);
        if (base.isNull())
        {
            return QIcon();
        }
        QPixmap tinted(base.size());
        tinted.fill(Qt::transparent);
        QPainter painter(&tinted);
        painter.setOpacity(opacity);
        painter.drawPixmap(0, 0, base);
        painter.end();
        return QIcon(tinted);
    }
}

widget_ingame::widget_ingame(int level,QWidget *parent)
    : auto_widget(parent)
    , ui(new Ui::widget_ingame)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAutoFillBackground(false);
    setStyleSheet(QStringLiteral("background: transparent;"));
    if (ui->background)
    {
        ui->background->setVisible(false);
    }
    if (ui->label)
    {
        ui->label->setVisible(false);
    }
    if (ui->label_2)
    {
        ui->label_2->setVisible(false);
    }
    const QIcon pauseIcon = QIcon(QStringLiteral(":/button/resources/button/pause.png"));
    const QIcon resumeIcon = QIcon(QStringLiteral(":/button/resources/button/continue.png"));
    const QIcon speedFastIcon = QIcon(QStringLiteral(":/button/resources/button/accelerate.png"));
    const QIcon speedNormalIcon = QIcon(QStringLiteral(":/button/resources/button/accelerate.png"));

    if (ui->pause)
    {
        ui->pause->setIcon(pauseIcon);
        ui->pause->raise();
    }
    if (ui->speed)
    {
        ui->speed->setIcon(speedNormalIcon);
        ui->speed->raise();
    }
    if (ui->menu)
    {
        ui->menu->raise();
    }

    progress=ui->progress;
    progress_bar=ui->progressbar;
    resource_value=ui->resource_value;
    // connect(this, &widget_ingame::clicked,
    //                  [](double x,double y) {
    //                      qDebug() << "点击位置（相对窗口）：x=" << x << ", y=" << y;
    //                  });
    connect(ui->pause,&QPushButton::clicked,this,[=](){
        if(!Pause){
            Pause=1;
            ui->pause->setIcon(resumeIcon);
            emit pause();
        }
        else{
            Pause=0;
            ui->pause->setIcon(pauseIcon);
            emit begin();
        }
    });
    connect(ui->speed,&QPushButton::clicked,this,[=](){
        if(speed){
            speed=0;
            ui->speed->setIcon(speedNormalIcon);
            emit speed_normal();
        }
        else{
            speed=1;
        ui->speed->setIcon(speedFastIcon);
            emit speed_up();
        }
    });


    initialSize = this->size(); // 从ui文件中获取的初始尺寸

    // 保存各个组件的初始几何信息
    initialGeometries[ui->background] = ui->background->geometry();
    initialGeometries[ui->description] = ui->description->geometry();
    initialGeometries[ui->pause] = ui->pause->geometry();
    initialGeometries[ui->speed] = ui->speed->geometry();
    initialGeometries[ui->menu] = ui->menu->geometry();
    initialGeometries[ui->resourse] = ui->resourse->geometry();
    initialGeometries[ui->stability] = ui->stability->geometry();
    initialGeometries[ui->label] = ui->label->geometry();
    initialGeometries[ui->label_2] = ui->label_2->geometry();
    initialGeometries[ui->progress] = ui->progress->geometry();
    initialGeometries[ui->progressbar] = ui->progressbar->geometry();
    initialGeometries[ui->stability_progressbar] = ui->stability_progressbar->geometry();
    initialGeometries[ui->stability_value] = ui->stability_value->geometry();
    initialGeometries[ui->resource_value] = ui->resource_value->geometry();


    // 保存图标按钮的初始图标大小
    initialIconSizes[ui->pause] = ui->pause->iconSize();
    initialIconSizes[ui->speed] = ui->speed->iconSize();
    initialIconSizes[ui->menu] = ui->menu->iconSize();
    initialIconSizes[ui->resourse] = ui->resourse->iconSize();
    initialIconSizes[ui->stability] = ui->stability->iconSize();

    // 保存需要缩放字体的控件
    auto applyFontFamily = [&](QWidget *widget, const QString &preferred)
    {
        if (!widget)
        {
            return;
        }
        QFont f = widget->font();
        f.setFamily(resolvedFontFamily(preferred));
        widget->setFont(f);
        initialFonts[widget] = f;
    };

    applyFontFamily(ui->description, QStringLiteral("幼圆"));
    applyFontFamily(ui->progress, QStringLiteral("幼圆"));

    auto applyFancyNumberStyle = [&](QLabel *label)
    {
        if (!label)
        {
            return;
        }
        QFont f(resolvedFontFamily(QStringLiteral("HYQiHei")));
        f.setPointSize(16);
        f.setBold(true);
        label->setFont(f);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet(QStringLiteral("QLabel { background-color: rgba(255,255,255,0.92); color: #1c1c1c; border-radius: 8px; padding: 2px 8px; }"));
        label->setText(QStringLiteral("0"));
        initialFonts[label] = f;
    };

    applyFancyNumberStyle(ui->resource_value);
    applyFancyNumberStyle(ui->stability_value);
}

widget_ingame::~widget_ingame()
{
    delete ui;
}

void widget_ingame::set_progress(int now,int total){
    progress->setText(QString("%1/%2").arg(now).arg(total));
    progress_bar->setValue(100*now/total);
}

void widget_ingame::set_resource_value(int value){
    if (resource_value)
    {
        resource_value->setText(QString::number(value));
    }
}
