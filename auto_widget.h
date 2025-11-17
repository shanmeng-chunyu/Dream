#ifndef AUTO_WIDGET_H
#define AUTO_WIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QMap>
#include <QRect>
#include <QLabel>
#include <QFont>
#include <QPixmap>
#include <QPainter>
#include "IconButton.h"


class auto_widget : public QWidget
{
    Q_OBJECT
public:
    explicit auto_widget(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void updateComponentsSize();
    void paintEvent(QPaintEvent *event) override;
    void generateBlurredBackground();

protected:
    // 存储初始尺寸，用于计算缩放比例
    QSize initialSize;
    // 存储组件的初始几何信息
    QMap<QWidget*, QRect> initialGeometries;
    // 存储图标按钮的初始图标大小
    QMap<QPushButton*, QSize> initialIconSizes;
    // 存储需要缩放字体的控件初始字体
    QMap<QWidget*, QFont> initialFonts;
    QPixmap m_rawBg;             // 原始（清晰）的背景图
    QPixmap m_blurredBg;         // 存储模糊后的背景图
    int m_blurRadius;

};

#endif // AUTO_WIDGET_H
