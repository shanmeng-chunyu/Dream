#ifndef AUTO_WIDGET_H
#define AUTO_WIDGET_H

#include <QWidget>
#include <QResizeEvent>
#include <QMap>
#include <QRect>
#include <QLabel>
#include "IconButton.h"

class auto_widget : public QWidget
{
    Q_OBJECT
public:
    explicit auto_widget(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void updateComponentsSize();

protected:
    // 存储初始尺寸，用于计算缩放比例
    QSize initialSize;
    // 存储组件的初始几何信息
    QMap<QWidget*, QRect> initialGeometries;
    // 存储图标按钮的初始图标大小
    QMap<QPushButton*, QSize> initialIconSizes;
};

#endif // AUTO_WIDGET_H
