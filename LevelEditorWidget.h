#ifndef DREAM_LEVELEDITORWIDGET_H
#define DREAM_LEVELEDITORWIDGET_H

#include <QWidget>
#include <QPointF>
#include <vector>
#include "WaveManager.h"

// ǰ������ UI �ؼ��������������Ҫ��ͷ�ļ�
class QPushButton;

class LevelEditorWidget : public QWidget {
    Q_OBJECT

    public:
    explicit LevelEditorWidget(QWidget *parent = nullptr);
    ~LevelEditorWidget(); // �������������Ǻ�ϰ��

    private slots:
        void onSaveButtonPressed();
    void onLoadButtonPressed();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    // �Ƴ� Ui ָ�룬ֱ�������ؼ���Ա
    QPushButton *m_saveButton;
    QPushButton *m_loadButton;

    std::vector<QPointF> m_enemyPath;
    std::vector<QPointF> m_towerBases;
    std::vector<WaveInfo> m_waves;
};

#endif //DREAM_LEVELEDITORWIDGET_H

