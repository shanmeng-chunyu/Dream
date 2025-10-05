#ifndef DREAM_LEVELEDITORWIDGET_H
#define DREAM_LEVELEDITORWIDGET_H

#include <QWidget>
#include <QPointF>
#include <vector>
#include "WaveManager.h"

// 前向声明 UI 控件，避免包含不必要的头文件
class QPushButton;

class LevelEditorWidget : public QWidget {
    Q_OBJECT

    public:
    explicit LevelEditorWidget(QWidget *parent = nullptr);
    ~LevelEditorWidget(); // 析构函数声明是好习惯

    private slots:
        void onSaveButtonPressed();
    void onLoadButtonPressed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    // 移除 Ui 指针，直接声明控件成员
    QPushButton *m_saveButton;
    QPushButton *m_loadButton;

    std::vector<QPointF> m_enemyPath;
    std::vector<QPointF> m_towerBases;
    std::vector<WaveInfo> m_waves;
};

#endif //DREAM_LEVELEDITORWIDGET_H

