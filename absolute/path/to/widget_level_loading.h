class widget_level_loading : public auto_widget
{
    Q_OBJECT

public:
    // ... 现有代码 ...

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    // ... 现有代码 ...
    QVector<QSequentialAnimationGroup*> m_animations;
    QVector<QPoint> m_originalPositions;
    int m_animationDistance;
    int m_animationDuration;
    int m_staggerDelay;
};