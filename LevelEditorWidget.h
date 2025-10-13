#ifndef LEVELEDITORWIDGET_H
#define LEVELEDITORWIDGET_H

#include <QWidget>

// 前向声明所有需要的Qt控件类，避免在头文件中引入大量头文件
class QLineEdit;
class QPushButton;
class QListWidget;
class QSpinBox;
class QDoubleSpinBox;
class QTabWidget;
class QListWidgetItem;

class LevelEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit LevelEditorWidget(QWidget* parent = nullptr);
    ~LevelEditorWidget() override = default;

    private slots:
        void saveLevel();
    void loadLevel();
    void addWave();
    void removeWave();
    void addEnemyToWave();
    void removeEnemyFromWave();
    void onWaveSelectionChanged();
    void onEnemySelectionChanged();
    void updateSelectedEnemy();

private:
    void setupUI();
    void clearEnemyDetails();

    // 主布局
    QTabWidget* tabWidget;

    // --- 波次编辑器 Tab ---
    QWidget* waveTab;
    QListWidget* waveListWidget;
    QListWidget* enemyInWaveListWidget;

    QPushButton* addWaveButton;
    QPushButton* removeWaveButton;
    QPushButton* addEnemyToWaveButton;
    QPushButton* removeEnemyFromWaveButton;

    // 敌人详情编辑区
    QLineEdit* enemyTypeLineEdit;
    QSpinBox* enemyCountSpinBox;
    QDoubleSpinBox* enemyIntervalSpinBox;

    // --- 关卡配置 Tab ---
    QWidget* configTab;
    // (可以扩展此处来定义关卡可用的防御塔和敌人)
    QLineEdit* levelNameEdit;


    // 底部按钮
    QPushButton* saveButton;
    QPushButton* loadButton;
};

#endif // LEVELEDITORWIDGET_H
