#include "IconButton.h"
#include <QEnterEvent>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QShowEvent>

IconButton::IconButton(QWidget *parent)
    : QPushButton(parent)
    , m_scale(1)
    , m_initialized(false)
    , m_sizePolicy(FixedRatio)
    , m_widthRatio(1)
    , m_heightRatio(1)
{
    setupStyle();
}

void IconButton::setIcon(const QIcon &icon)
{
    m_icon = icon;
    QPushButton::setIcon(icon);
    if (m_initialized) {
        updateIconSize();
    }
}

qreal IconButton::scale() const
{
    return m_scale;
}

void IconButton::setScale(qreal scale)
{
    m_scale = scale;
    if (m_initialized) {
        updateIconSize();
    }
    update();
}

void IconButton::setIconSizePolicy(SizePolicy policy)
{
    m_sizePolicy = policy;
    if (m_initialized) {
        updateIconSize();
    }
}

void IconButton::setIconWidthRatio(qreal ratio)
{
    m_widthRatio = qBound(0.1, ratio, 0.9);
    if (m_initialized) {
        updateIconSize();
    }
}

void IconButton::setIconHeightRatio(qreal ratio)
{
    m_heightRatio = qBound(0.1, ratio, 0.9);
    if (m_initialized) {
        updateIconSize();
    }
}

void IconButton::enterEvent(QEnterEvent *event)
{
    startScaleAnimation(1.3);
    QPushButton::enterEvent(event);
}

void IconButton::leaveEvent(QEvent *event)
{
    startScaleAnimation(1.0);
    QPushButton::leaveEvent(event);
}

void IconButton::mousePressEvent(QMouseEvent *event)
{
    startScaleAnimation(0.9);
    QPushButton::mousePressEvent(event);
}

void IconButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (underMouse()) {
        startScaleAnimation(1.3);
    } else {
        startScaleAnimation(1.0);
    }
    QPushButton::mouseReleaseEvent(event);
}

void IconButton::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);
    if (m_initialized) {
        updateIconSize();
    }
}

void IconButton::showEvent(QShowEvent *event)
{
    QPushButton::showEvent(event);
    if (!m_initialized && width() > 0 && height() > 0) {
        m_initialized = true;
        updateIconSize();
    }
}

void IconButton::setupStyle()
{
    setStyleSheet(R"(
        IconButton {
            background-color: transparent;
            border: none;
        }
    )");
    setCursor(Qt::PointingHandCursor);
}

void IconButton::startScaleAnimation(qreal targetScale)
{
    QPropertyAnimation *animation = new QPropertyAnimation(this, "scale");
    animation->setDuration(150);
    animation->setStartValue(m_scale);
    animation->setEndValue(targetScale);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void IconButton::updateIconSize()
{
    if (width() <= 0 || height() <= 0) return;

    QSize iconSize = calculateIconSize() * m_scale;
    setIconSize(iconSize);
}

QSize IconButton::calculateIconSize() const
{
    switch (m_sizePolicy) {
    case FitToButton: {
        // 图标适应按钮，保持宽高比
        int baseSize = qMin(width(), height()) ;
        return QSize(baseSize, baseSize);
    }
    case FillButton: {
        // 图标填充按钮（可能变形）
        return QSize(width() * m_widthRatio, height() * m_heightRatio);
    }
    case FixedRatio:
    default: {
        // 固定比例（默认）
        return QSize(width() * m_widthRatio, height() * m_heightRatio);
    }
    }
}
