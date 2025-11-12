#ifndef ICON_BUTTON_H
#define ICON_BUTTON_H

#include <QPushButton>
#include <QPropertyAnimation>
#include <QIcon>
#include <QTimer>

class IconButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)

public:
    explicit IconButton(QWidget *parent = nullptr);

    void setIcon(const QIcon &icon);
    qreal scale() const;
    void setScale(qreal scale);

    // 设置图标大小策略
    enum SizePolicy {
        FitToButton,    // 图标适应按钮（保持宽高比）
        FillButton,     // 图标填充按钮（可能变形）
        FixedRatio      // 固定比例（默认）
    };

    void setIconSizePolicy(SizePolicy policy);
    void setIconWidthRatio(qreal ratio);  // 图标宽度占按钮宽度的比例
    void setIconHeightRatio(qreal ratio); // 图标高度占按钮高度的比例
    void updateIconSize();
    QIcon createScalableIcon(const QIcon &originalIcon);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void setupStyle();
    void startScaleAnimation(qreal targetScale);

    QSize calculateIconSize() const;

private:
    qreal m_scale;
    QIcon m_icon;
    bool m_initialized;
    SizePolicy m_sizePolicy;
    qreal m_widthRatio;
    qreal m_heightRatio;
    bool m_animationRunning;
    qreal m_targetScale;
};

#endif // ICON_BUTTON_H
