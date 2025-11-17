// Dream - 副本/auto_widget.cpp

#include "auto_widget.h"

auto_widget::auto_widget(QWidget *parent)
    : QWidget{parent}
{
    // 只有当这个 widget 是一个 "顶级窗口" (我们通过 parent == nullptr 来判断) 时，
    // 我们才设置黑色背景来实现 Pillarbox。
    if (parent == nullptr) 
    {
        this->setAutoFillBackground(true);
        QPalette pal = this->palette();
        pal.setColor(QPalette::Window, Qt::white);
        this->setPalette(pal);
    }
}

void auto_widget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateComponentsSize();
}

void auto_widget::updateComponentsSize()
{
    // 安全检查，防止在 initialSize 未初始化时调用
    if (initialSize.width() <= 0 || initialSize.height() <= 0) {
        return;
    }

    // --- 1. 计算 4:3 安全区域 ---
    QSize newSize = this->size(); // 窗口的实际尺寸 (e.g., 1920x1080)
    qreal targetRatio = 4.0 / 3.0; // 我们的 UI 设计比例
    
    // 检查 initialSize 是否有效，避免除零
    if (initialSize.height() == 0) return; 
    
    qreal currentRatio = static_cast<qreal>(newSize.width()) / newSize.height();

    qreal scale;     // 统一的缩放因子
    qreal xOffset = 0; // 水平偏移（Pillarbox 黑边）
    qreal yOffset = 0; // 垂直偏移（Letterbox 黑边）

    if (currentRatio > targetRatio) {
        // Pillarbox (当前窗口比 4:3 更宽, e.g., 16:9)
        scale = static_cast<qreal>(newSize.height()) / initialSize.height();
        qreal safeWidth = initialSize.width() * scale;
        xOffset = (newSize.width() - safeWidth) / 2.0;
    } else {
        // Letterbox (当前窗口比 4:3 更高, e.g., 9:16)
        scale = static_cast<qreal>(newSize.width()) / initialSize.width();
        qreal safeHeight = initialSize.height() * scale;
        yOffset = (newSize.height() - safeHeight) / 2.0;
    }

    const qreal fontRatio = scale;

    // --- 2. 遍历所有控件并应用布局 ---
    for (auto it = initialGeometries.constBegin(); it != initialGeometries.constEnd(); ++it) {
        QWidget *widget = it.key();
        QRect initialRect = it.value();

        // 所有控件 (包括背景) 都被统一缩放并放置在居中的 4:3 安全区域内
        int newX = static_cast<int>(initialRect.x() * scale + xOffset);
        int newY = static_cast<int>(initialRect.y() * scale + yOffset);
        int newWidth = static_cast<int>(initialRect.width() * scale);
        int newHeight = static_cast<int>(initialRect.height() * scale);
        
        widget->setGeometry(newX, newY, newWidth, newHeight);

        // --- 3. 更新图标和字体（逻辑保持不变） ---
        QPushButton *btn = qobject_cast<QPushButton*>(widget);
        IconButton *iconButton = qobject_cast<IconButton*>(widget);

        if (iconButton) {
            iconButton->updateIconSize();
        }
        
        if (btn && initialIconSizes.contains(btn)) {
            QSize initialIconSize = initialIconSizes[btn];
            double iconScale = scale;
            
            if (iconButton && initialRect.height() > 0) {
                 iconScale = static_cast<qreal>(widget->height()) / initialRect.height();
            }

            int newIconWidth = static_cast<int>(initialIconSize.width() * iconScale);
            int newIconHeight = static_cast<int>(initialIconSize.height() * iconScale);
            
            if (!iconButton) {
                btn->setIconSize(QSize(newIconWidth, newIconHeight));
            }
        }

        QLabel *label = qobject_cast<QLabel*>(widget);
        if (label) {
            label->setScaledContents(true);
        }

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