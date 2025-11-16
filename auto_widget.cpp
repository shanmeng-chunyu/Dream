#include "auto_widget.h"

auto_widget::auto_widget(QWidget *parent)
    : QWidget{parent}
{}

void auto_widget::resizeEvent(QResizeEvent *event)
{
    // 获取请求的新尺寸
    QSize newSize = event->size();

    // 计算保持4:3比例的尺寸
    qreal targetRatio = 4.0 / 3.0;
    qreal currentRatio = static_cast<qreal>(newSize.width()) / newSize.height();

    int newWidth, newHeight;

    if (currentRatio > targetRatio) {
        // 当前宽度相对过高，以高度为基准调整
        newHeight = newSize.height();
        newWidth = static_cast<int>(newHeight * targetRatio);
    } else {
        // 当前高度相对过高，以宽度为基准调整
        newWidth = newSize.width();
        newHeight = static_cast<int>(newWidth / targetRatio);
    }

    // 如果尺寸发生了变化，重新调整窗口大小
    if (newWidth != width() || newHeight != height()) {
        // 避免无限递归，暂时断开resizeEvent
        setUpdatesEnabled(false);
        resize(newWidth, newHeight);
        setUpdatesEnabled(true);
    }

    // 调用父类的resizeEvent
    QWidget::resizeEvent(event);

    // 更新所有组件的大小和位置
    updateComponentsSize();
}

void auto_widget::updateComponentsSize()
{
    // 计算缩放比例
    qreal widthRatio = static_cast<qreal>(width()) / initialSize.width();
    qreal heightRatio = static_cast<qreal>(height()) / initialSize.height();
    const qreal fontRatio = std::min(widthRatio, heightRatio);

    // 遍历更新所有组件的几何信息
    for (auto it = initialGeometries.constBegin(); it != initialGeometries.constEnd(); ++it) {
        QWidget *widget = it.key();
        QRect initialRect = it.value();

        // 计算新的几何信息
        int newX = static_cast<int>(initialRect.x() * widthRatio);
        int newY = static_cast<int>(initialRect.y() * heightRatio);
        int newWidth = static_cast<int>(initialRect.width() * widthRatio);
        int newHeight = static_cast<int>(initialRect.height() * heightRatio);

        widget->setGeometry(newX, newY, newWidth, newHeight);

        QPushButton *btn=qobject_cast<QPushButton*>(widget);
        IconButton *iconButton = qobject_cast<IconButton*>(widget);
        if (iconButton) {
            // 对于IconButton，调用其updateIconSize()方法让其内部处理图标大小
            // 而不是直接设置iconSize，这样可以保留动画效果
            iconButton->updateIconSize();
        }
        // 对于普通QPushButton，继续使用原来的处理方式
        else if (btn && initialIconSizes.contains(btn)) {
            QPushButton *button = static_cast<QPushButton*>(widget);
            QSize initialIconSize = initialIconSizes[button];
            int newIconWidth = static_cast<int>(initialIconSize.width() * widthRatio);
            int newIconHeight = static_cast<int>(initialIconSize.height() * heightRatio);
            button->setIconSize(QSize(newIconWidth, newIconHeight));
        }
        QLabel *label=qobject_cast<QLabel*>(widget);
        if(label) label->setScaledContents(1);

        if (initialFonts.contains(widget))
        {
            QFont baseFont = initialFonts.value(widget);
            QFont scaledFont = baseFont;
            if (baseFont.pointSizeF() > 0)
            {
                scaledFont.setPointSizeF(std::max(1.0, baseFont.pointSizeF() * fontRatio));
            }
            else if (baseFont.pixelSize() > 0)
            {
                scaledFont.setPixelSize(std::max(1, static_cast<int>(baseFont.pixelSize() * fontRatio)));
            }
            widget->setFont(scaledFont);
        }
    }
}

