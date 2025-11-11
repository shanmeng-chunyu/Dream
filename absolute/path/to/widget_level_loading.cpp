// 添加私有成员到widget_level_loading类
srivate:
    QVector<QSequentialAnimationGroup*> m_animations;
    QVector<QPoint> m_originalPositions;
    int m_animationDistance;
    int m_animationDuration;
    int m_staggerDelay;

// 修改构造函数，初始化新成员变量
widget_level_loading::widget_level_loading(int type,QWidget *parent)
    :level_type(type)
    ,auto_widget(parent)
    , ui(new Ui::widget_level_loading)
    // ... 现有代码 ...
    , m_animationDistance(20)
    , m_animationDuration(2000)
    , m_staggerDelay(200)
{
    // ... 现有代码 ...
    QVector<QPushButton *> icons={ui->icon1,ui->icon2,ui->icon3,ui->icon4};
    for(int i=0;i<4;i++){
        icons[i]->setIcon(QIcon(icon[type][i]));
        m_originalPositions.append(icons[i]->pos());
    }
    createSequentialStaggeredFloating(icons);
    // ... 现有代码 ...
}

// 修改createSequentialStaggeredFloating方法，保存动画对象
void widget_level_loading::createSequentialStaggeredFloating(const QVector<QPushButton*> &btns, int distance, int duration, int staggerDelay)
{
    m_animationDistance = distance;
    m_animationDuration = duration;
    m_staggerDelay = staggerDelay;
    
    // 停止并清除现有动画
    for (auto anim : m_animations) {
        if (anim) {
            anim->stop();
            anim->deleteLater();
        }
    }
    m_animations.clear();
    
    for (int i = 0; i < btns.size(); ++i) {
        auto *btn = btns[i];
        if (!btn) continue;

        QPoint original = btn->pos();
        
        // 为每个标签创建独立的动画序列
        QSequentialAnimationGroup *sequence = new QSequentialAnimationGroup(btn);
        m_animations.append(sequence);

        // 添加初始延迟（每个标签递增）
        QPauseAnimation *initialDelay = new QPauseAnimation();
        initialDelay->setDuration(i * staggerDelay);
        sequence->addAnimation(initialDelay);

        // 创建上下浮动动画
        QPropertyAnimation *floatUp = new QPropertyAnimation(btn, "pos");
        floatUp->setDuration(duration / 2);
        floatUp->setStartValue(original);
        floatUp->setEndValue(original + QPoint(0,-distance));
        floatUp->setEasingCurve(QEasingCurve::InOutSine);

        QPropertyAnimation *floatDown = new QPropertyAnimation(btn, "pos");
        floatDown->setDuration(duration / 2);
        floatDown->setStartValue(original + QPoint(0,-distance));
        floatDown->setEndValue(original);
        floatDown->setEasingCurve(QEasingCurve::InOutSine);

        // 添加到序列
        sequence->addAnimation(floatUp);
        sequence->addAnimation(floatDown);
        sequence->setLoopCount(-1);

        sequence->start();
    }
}

// 重写resizeEvent方法，在窗口大小变化后重新启动动画
void widget_level_loading::resizeEvent(QResizeEvent *event)
{
    auto_widget::resizeEvent(event);
    
    // 重新启动浮动动画以适应新的位置
    QVector<QPushButton *> icons={ui->icon1,ui->icon2,ui->icon3,ui->icon4};
    createSequentialStaggeredFloating(icons, m_animationDistance, m_animationDuration, m_staggerDelay);
}